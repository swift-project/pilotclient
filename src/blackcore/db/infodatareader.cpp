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
#include <QRegularExpression>
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
        CInfoDataReader::CInfoDataReader(QObject *owner, const CDatabaseReaderConfigList &config) :
            CDatabaseReader(owner, config, "CInfoDataReader")
        {
            // init to avoid threading issues
            getBaseUrl();
        }

        CDbInfoList CInfoDataReader::getDbInfoObjects() const
        {
            QReadLocker l(&m_lockInfoObjects);
            return m_infoObjects;
        }

        int CInfoDataReader::getDbInfoObjectCount() const
        {
            QReadLocker l(&m_lockInfoObjects);
            return m_infoObjects.size();
        }

        bool CInfoDataReader::areAllDataRead() const
        {
            return getDbInfoObjectCount() > 4;
        }

        void CInfoDataReader::syncronizeCaches(CEntityFlags::Entity entities)
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

        void CInfoDataReader::read(CEntityFlags::Entity entities, const QDateTime &newerThan)
        {
            this->ps_read(entities, newerThan);
        }

        void CInfoDataReader::ps_read(CEntityFlags::Entity entities, const QDateTime &newerThan)
        {
            CEntityFlags::Entity triggeredRead = CEntityFlags::NoEntity;
            CUrl url(getInfoObjectsUrl());
            if (entities.testFlag(CEntityFlags::InfoObjectEntity))
            {
                if (!url.isEmpty())
                {
                    if (!newerThan.isNull())
                    {
                        const QString tss(newerThan.toString(Qt::ISODate));
                        url.appendQuery(QString(parameterLatestTimestamp() + "=" + tss));
                    }
                    sApp->getFromNetwork(url, { this, &CInfoDataReader::ps_parseInfoObjectsData});
                    triggeredRead |= CEntityFlags::InfoObjectEntity;
                }
                else
                {
                    CLogMessage(this).error("No URL for %1") << CEntityFlags::flagToString(CEntityFlags::InfoObjectEntity);
                }
            }

            if (triggeredRead != CEntityFlags::NoEntity)
            {
                emit dataRead(triggeredRead, CEntityFlags::StartRead, 0);
            }
        }

        void CInfoDataReader::ps_parseInfoObjectsData(QNetworkReply *nwReplyPtr)
        {
            // wrap pointer, make sure any exit cleans up reply
            // required to use delete later as object is created in a different thread
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
            if (this->isAbandoned()) { return; }

            QString urlString(nwReply->url().toString());
            CDatabaseReader::JsonDatastoreResponse res = this->setStatusAndTransformReplyIntoDatastoreResponse(nwReply.data());
            if (res.hasErrorMessage())
            {
                CLogMessage::preformatted(res.lastWarningOrAbove());
                emit dataRead(CEntityFlags::InfoObjectEntity, CEntityFlags::ReadFailed, 0);
                return;
            }

            // get all or incremental set
            CDbInfoList infoObjects = CDbInfoList::fromDatabaseJson(res.getJsonArray());

            // this part needs to be synchronized
            int n = infoObjects.size();
            {
                QWriteLocker wl(&this->m_lockInfoObjects);
                this->m_infoObjects = infoObjects;
            }

            // never emit when lock is held -> deadlock
            emit dataRead(CEntityFlags::InfoObjectEntity,
                          res.isRestricted() ? CEntityFlags::ReadFinishedRestricted : CEntityFlags::ReadFinished, n);
            CLogMessage(this).info("Read %1 %2 from %3") << n << CEntityFlags::flagToString(CEntityFlags::InfoObjectEntity) << urlString;
        }

        const CUrl &CInfoDataReader::getBaseUrl()
        {
            static const CUrl baseUrl(sApp->getGlobalSetup().getDbInfoReaderUrl());
            return baseUrl;
        }

        CUrl CInfoDataReader::getInfoObjectsUrl() const
        {
            return getBaseUrl().withAppendedPath("service/jsondbinfo.php");
        }
    } // namespace
} // namespace
