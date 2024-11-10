// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/db/infodatareader.h"
#include "blackcore/application.h"
#include "blackcore/webdataservices.h"
#include "misc/sequence.h"
#include "misc/logmessage.h"
#include "misc/network/networkutils.h"
#include "misc/verify.h"

#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>

using namespace swift::misc;
using namespace swift::misc::network;
using namespace swift::misc::db;
using namespace BlackCore::Data;

namespace BlackCore::Db
{
    CInfoDataReader::CInfoDataReader(QObject *owner, const CDatabaseReaderConfigList &config, CDbFlags::DataRetrievalModeFlag mode) : CDatabaseReader(owner, config, "CInfoDataReader"), m_mode(mode)
    {
        Q_ASSERT_X(mode == CDbFlags::DbReading || mode == CDbFlags::Shared, Q_FUNC_INFO, "Wrong mode");
    }

    CDbInfoList CInfoDataReader::getInfoObjects() const
    {
        QReadLocker l(&m_lockInfoObjects);
        return m_infoObjects;
    }

    int CInfoDataReader::getInfoObjectCount() const
    {
        QReadLocker l(&m_lockInfoObjects);
        return m_infoObjects.size();
    }

    bool CInfoDataReader::areAllInfoObjectsRead() const
    {
        return this->getInfoObjectCount() > 4;
    }

    void CInfoDataReader::synchronizeCaches(CEntityFlags::Entity entities)
    {
        // no caching used here
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, "Using this for CInfoDataReader makes no sense");
        Q_UNUSED(entities);
    }

    void CInfoDataReader::admitCaches(CEntityFlags::Entity entities)
    {
        // no caching used here
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, "Using this for CInfoDataReader makes no sense");
        Q_UNUSED(entities);
    }

    void CInfoDataReader::invalidateCaches(CEntityFlags::Entity entities)
    {
        // no caching used here
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, "Using this for CInfoDataReader makes no sense");
        Q_UNUSED(entities);
    }

    QDateTime CInfoDataReader::getCacheTimestamp(CEntityFlags::Entity entity) const
    {
        // no own caching used here
        Q_UNUSED(entity);
        Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "Need single entity");
        Q_ASSERT_X(sApp, Q_FUNC_INFO, "Need sApp");

        if (entity == CEntityFlags::DbInfoObjectEntity || entity == CEntityFlags::SharedInfoObjectEntity)
        {
            SWIFT_VERIFY_X(false, Q_FUNC_INFO, "Using this for CInfoDataReader makes no sense");
            return QDateTime();
        }

        // Forward to web data services so I get cache data from other readers
        //! \fixme bit of a hack to use web data services here
        return sApp->getWebDataServices()->getCacheTimestamp(entity);
    }

    int CInfoDataReader::getCacheCount(CEntityFlags::Entity entity) const
    {
        // no own caching used here
        Q_UNUSED(entity);
        Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "Need single entity");
        Q_ASSERT_X(sApp, Q_FUNC_INFO, "Need sApp");

        if (entity == CEntityFlags::DbInfoObjectEntity || entity == CEntityFlags::SharedInfoObjectEntity)
        {
            SWIFT_VERIFY_X(false, Q_FUNC_INFO, "Using this for CInfoDataReader makes no sense");
            return 0;
        }

        // Forward to web data services so I get cache data from other readers
        //! \fixme bit of a hack to use web data services here
        return sApp->getWebDataServices()->getCacheCount(entity);
    }

    CEntityFlags::Entity CInfoDataReader::getEntitiesWithCacheCount() const
    {
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, "Using this for CInfoDataReader makes no sense");
        return CEntityFlags::NoEntity;
    }

    CEntityFlags::Entity CInfoDataReader::getEntitiesWithCacheTimestampNewerThan(const QDateTime &threshold) const
    {
        Q_UNUSED(threshold);
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, "Using this for CInfoDataReader makes no sense");
        return CEntityFlags::NoEntity;
    }

    bool CInfoDataReader::hasChangedUrl(CEntityFlags::Entity entity, CUrl &oldUrlInfo, CUrl &newUrlInfo) const
    {
        // not implemented
        Q_UNUSED(entity);

        // init the URLs
        oldUrlInfo = this->getBaseUrl(CDbFlags::DbReading);
        newUrlInfo = this->getBaseUrl(CDbFlags::DbReading);
        return false;
    }

    CUrl CInfoDataReader::getDbServiceBaseUrl() const
    {
        return sApp->getGlobalSetup().getDbInfoReaderUrl();
    }

    void CInfoDataReader::readInfoData()
    {
        if (!this->doWorkCheck()) { return; }

        const CUrl url(this->getInfoObjectsUrl());
        const CUrlLogList urlLogList(this->getUrlLogList()); // thread safe copy

        if (urlLogList.hasPending())
        {
            CLogMessage(this).info(u"Info data reading still pending, summary: '%1'") << urlLogList.getSummary();
            return;
        }
        if (!url.isEmpty())
        {
            this->getFromNetworkAndLog(url, { this, &CInfoDataReader::parseInfoObjectsData });
            emit this->dataRead(this->getEntityForMode(), CEntityFlags::ReadStarted, 0, url);
        }
        else
        {
            this->logNoWorkingUrl(this->getEntityForMode());
        }
    }

    void CInfoDataReader::parseInfoObjectsData(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        if (!this->doWorkCheck()) { return; }

        const CDatabaseReader::JsonDatastoreResponse res = this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
        if (res.hasErrorMessage())
        {
            CLogMessage::preformatted(res.lastWarningOrAbove());
            emit this->dataRead(this->getEntityForMode(), CEntityFlags::ReadFailed, 0, res.getUrl());
            return;
        }

        // get all or incremental set
        const CDbInfoList infoObjects = CDbInfoList::fromDatabaseJson(res.getJsonArray());
        const int n = infoObjects.size();

        // Service URL => DB data
        // DB data directory => shared files
        const QString urlStr = nwReply->url().toString();
        Q_UNUSED(urlStr) // debug only

        // this part needs to be synchronized
        {
            QWriteLocker wl(&m_lockInfoObjects);
            m_infoObjects = infoObjects;
        }

        this->emitAndLogDataRead(this->getEntityForMode(), n, res);
    }

    CUrl CInfoDataReader::getDbInfoObjectsUrl() const
    {
        return getBaseUrl(CDbFlags::DbReading).withAppendedPath("jsondbinfo.php");
    }

    CUrl CInfoDataReader::getSharedInfoObjectsUrl() const
    {
        return getBaseUrl(CDbFlags::Shared).withAppendedPath(CDbInfo::sharedInfoFileName());
    }

    CEntityFlags::EntityFlag CInfoDataReader::getEntityForMode() const
    {
        if (m_mode == CDbFlags::DbReading) { return CEntityFlags::DbInfoObjectEntity; }
        if (m_mode == CDbFlags::Shared) { return CEntityFlags::SharedInfoObjectEntity; }
        qFatal("Wrong mode");
        return CEntityFlags::NoEntity;
    }

    void CInfoDataReader::read(CEntityFlags::Entity entities, CDbFlags::DataRetrievalModeFlag mode, const QDateTime &newerThan)
    {
        Q_UNUSED(entities)
        Q_UNUSED(mode)
        Q_UNUSED(newerThan)
        Q_ASSERT_X(false, Q_FUNC_INFO, "Not implemented for CInfoDataReader");
    }

    CUrl CInfoDataReader::getInfoObjectsUrl() const
    {
        switch (m_mode)
        {
        case CDbFlags::DbReading: return getDbInfoObjectsUrl();
        case CDbFlags::Shared: return getSharedInfoObjectsUrl();
        default: qFatal("Wrong mode");
        }
        return CUrl();
    }

    CStatusMessageList CInfoDataReader::readFromJsonFiles(const QString &dir, CEntityFlags::Entity whatToRead, bool overrideNewer)
    {
        Q_UNUSED(dir)
        Q_UNUSED(whatToRead)
        Q_UNUSED(overrideNewer)
        Q_ASSERT_X(false, Q_FUNC_INFO, "Not supported");

        return CStatusMessage(this).error(u"Not supported");
    }

    bool CInfoDataReader::readFromJsonFilesInBackground(const QString &dir, CEntityFlags::Entity whatToRead, bool overrideNewer)
    {
        Q_UNUSED(dir)
        Q_UNUSED(whatToRead)
        Q_UNUSED(overrideNewer)
        Q_ASSERT_X(false, Q_FUNC_INFO, "Not supported");
        return false;
    }

    CEntityFlags::Entity CInfoDataReader::getSupportedEntities() const
    {
        return this->getEntityForMode();
    }
} // namespace
