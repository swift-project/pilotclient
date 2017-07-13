/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackmisc/sequence.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/networkutils.h"
#include "infodatareader.h"

#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Db;
using namespace BlackCore::Data;

namespace BlackCore
{
    namespace Db
    {
        CInfoDataReader::CInfoDataReader(QObject *owner, const CDatabaseReaderConfigList &config, CDbFlags::DataRetrievalModeFlag mode) :
            CDatabaseReader(owner, config, "CInfoDataReader"), m_mode(mode)
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

        bool CInfoDataReader::areAllDataRead() const
        {
            return getInfoObjectCount() > 4;
        }

        void CInfoDataReader::synchronizeCaches(CEntityFlags::Entity entities)
        {
            // no caching used here
            Q_UNUSED(entities);
        }

        void CInfoDataReader::admitCaches(CEntityFlags::Entity entities)
        {
            // no caching used here
            Q_UNUSED(entities);
        }

        void CInfoDataReader::invalidateCaches(CEntityFlags::Entity entities)
        {
            // no caching used here
            Q_UNUSED(entities);
        }

        QDateTime CInfoDataReader::getCacheTimestamp(CEntityFlags::Entity entity) const
        {
            // no caching used here
            Q_UNUSED(entity);
            return QDateTime();
        }

        int CInfoDataReader::getCacheCount(CEntityFlags::Entity entity) const
        {
            // no caching used here
            Q_UNUSED(entity);
            return 0;
        }

        bool CInfoDataReader::hasChangedUrl(CEntityFlags::Entity entity) const
        {
            // not implemented
            Q_UNUSED(entity);
            return false;
        }

        CUrl CInfoDataReader::getDbServiceBaseUrl() const
        {
            return sApp->getGlobalSetup().getDbInfoReaderUrl();
        }

        void CInfoDataReader::read()
        {
            const CUrl url(this->getInfoObjectsUrl());
            if (!url.isEmpty())
            {
                sApp->getFromNetwork(url, { this, &CInfoDataReader::parseInfoObjectsData});
                emit dataRead(this->getEntityForMode(), CEntityFlags::StartRead, 0);
            }
            else
            {
                CLogMessage(this).error("No URL for '%1'") << CEntityFlags::flagToString(this->getEntityForMode());
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
                emit dataRead(this->getEntityForMode(), CEntityFlags::ReadFailed, 0);
                return;
            }

            // get all or incremental set
            const CDbInfoList infoObjects = CDbInfoList::fromDatabaseJson(res.getJsonArray());

            // this part needs to be synchronized
            const int n = infoObjects.size();
            {
                QWriteLocker wl(&this->m_lockInfoObjects);
                this->m_infoObjects = infoObjects;
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
            if (this->m_mode == CDbFlags::DbReading) return CEntityFlags::DbInfoObjectEntity;
            if (this->m_mode == CDbFlags::Shared) return CEntityFlags::SharedInfoObjectEntity;
            qFatal("Wrong mode");
            return CEntityFlags::NoEntity;
        }

        CUrl CInfoDataReader::getInfoObjectsUrl() const
        {
            switch (m_mode)
            {
            case CDbFlags::DbReading: return getDbInfoObjectsUrl();
            case CDbFlags::Shared: return getSharedInfoObjectsUrl();
            default:
                qFatal("Wrong mode");
            }
            return CUrl();
        }

        CEntityFlags::Entity CInfoDataReader::getSupportedEntities() const
        {
            return this->getEntityForMode();
        }
    } // namespace
} // namespace
