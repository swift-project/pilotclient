/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/db/databasereader.h"
#include "blackcore/db/infodatareader.h"
#include "blackcore/webdataservices.h"
#include "blackcore/application.h"
#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/logmessage.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValueRef>
#include <QMetaObject>
#include <QNetworkReply>
#include <QReadLocker>
#include <QUrl>
#include <QWriteLocker>

using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;
using namespace BlackCore;

namespace BlackCore
{
    namespace Db
    {
        CDatabaseReader::CDatabaseReader(QObject *owner, const CDatabaseReaderConfigList &config, const QString &name) :
            BlackCore::CThreadedReader(owner, name), m_config(config)
        {
            getDbUrl(); // init
        }

        void CDatabaseReader::readInBackgroundThread(CEntityFlags::Entity entities, const QDateTime &newerThan)
        {
            if (isAbandoned()) { return; }

            // we accept cached cached data
            Q_ASSERT_X(!entities.testFlag(CEntityFlags::InfoObjectEntity), Q_FUNC_INFO, "Read info objects directly");
            const bool hasInfoObjects = this->hasInfoObjects(); // no info objects is no necessarily error, but indicates a) either data not available or b) only caches is used
            CEntityFlags::Entity allEntities    = entities;
            CEntityFlags::Entity cachedEntities = CEntityFlags::NoEntity;
            CEntityFlags::Entity currentEntity  = CEntityFlags::iterateDbEntities(allEntities); // CEntityFlags::InfoObjectEntity will be ignored
            while (currentEntity)
            {
                const CDatabaseReaderConfig config(this->getConfigForEntity(currentEntity));
                const QString currentEntityName = CEntityFlags::flagToString(currentEntity);
                if (config.getRetrievalMode().testFlag(CDbFlags::Cached))
                {
                    if (hasInfoObjects)
                    {
                        const bool changedUrl = this->hasChangedUrl(currentEntity);
                        const QDateTime cacheTs(this->getCacheTimestamp(currentEntity));
                        const QDateTime latestEntityTs(this->getLatestEntityTimestampFromInfoObjects(currentEntity));
                        const qint64 cacheTimestamp = cacheTs.isValid() ? cacheTs.toMSecsSinceEpoch() : -1;
                        const qint64 latestEntityTimestamp = latestEntityTs.isValid() ? latestEntityTs.toMSecsSinceEpoch() : -1;
                        Q_ASSERT_X(latestEntityTimestamp >= 0, Q_FUNC_INFO, "Missing timestamp");
                        if (!changedUrl && cacheTimestamp >= latestEntityTimestamp && cacheTimestamp >= 0 && latestEntityTimestamp >= 0)
                        {
                            this->syncronizeCaches(currentEntity);
                            entities &= ~currentEntity; // do not load from web
                            cachedEntities |= currentEntity; // read from cache
                            CLogMessage(this).info("Using cache for %1 (%2, %3)") << currentEntityName << cacheTs.toString() << cacheTimestamp;
                        }
                        else
                        {
                            if (changedUrl)
                            {
                                CLogMessage(this).info("Data location changed, will override cache for %1") << currentEntityName;
                            }
                            else
                            {
                                CLogMessage(this).info("Cache for %1 outdated, latest entity (%2, %3)") << currentEntityName << latestEntityTs.toString() << latestEntityTimestamp;
                            }
                        }
                    }
                    else
                    {
                        // no info objects, server down
                        this->syncronizeCaches(currentEntity);
                        const int c = this->getCacheCount(currentEntity);
                        CLogMessage(this).info("No info object for %1, using cache with %2 objects") << currentEntityName << c;
                        entities &= ~currentEntity; // do not load from web
                        cachedEntities |= currentEntity; // read from cache
                    }
                }
                currentEntity = CEntityFlags::iterateDbEntities(allEntities);
            }

            // signals for the cached entities
            this->emitReadSignalPerSingleCachedEntity(cachedEntities);

            // Real read from DB
            this->startReadFromDbInBackgroundThread(entities, newerThan);
        }

        void CDatabaseReader::startReadFromDbInBackgroundThread(CEntityFlags::Entity entities, const QDateTime &newerThan)
        {
            // ps_read is implemented in the derived classes
            if (entities == CEntityFlags::NoEntity) { return; }
            if (!this->isNetworkAvailable())
            {
                CLogMessage(this).warning("No network, will not read %1") << CEntityFlags::flagToString(entities);
                return;
            }

            const bool s = QMetaObject::invokeMethod(this, "ps_read",
                           Q_ARG(BlackMisc::Network::CEntityFlags::Entity, entities),
                           Q_ARG(QDateTime, newerThan));
            Q_ASSERT_X(s, Q_FUNC_INFO, "Invoke failed");
            Q_UNUSED(s);
        }

        CDatabaseReader::JsonDatastoreResponse CDatabaseReader::transformReplyIntoDatastoreResponse(QNetworkReply *nwReply) const
        {
            this->threadAssertCheck();
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::webservice()}));

            JsonDatastoreResponse datastoreResponse;
            if (this->isAbandoned())
            {
                nwReply->abort();
                datastoreResponse.setMessage(CStatusMessage(cats, CStatusMessage::SeverityError, "Terminated data parsing process"));
                return datastoreResponse; // stop, terminate straight away, ending thread
            }

            if (nwReply->error() == QNetworkReply::NoError)
            {
                const QString dataFileData = nwReply->readAll().trimmed();
                nwReply->close(); // close asap
                if (dataFileData.isEmpty())
                {
                    datastoreResponse.setMessage(CStatusMessage(cats, CStatusMessage::SeverityError, "Empty response, no data"));
                    datastoreResponse.m_updated = QDateTime::currentDateTimeUtc();
                    return datastoreResponse;
                }

                QJsonDocument jsonResponse = QJsonDocument::fromJson(dataFileData.toUtf8());
                if (jsonResponse.isArray())
                {
                    // directly an array, no further info
                    datastoreResponse.setJsonArray(jsonResponse.array());
                    datastoreResponse.m_updated = QDateTime::currentDateTimeUtc();
                }
                else
                {
                    QJsonObject responseObject(jsonResponse.object());
                    datastoreResponse.setJsonArray(responseObject["data"].toArray());
                    QString ts(responseObject["latest"].toString());
                    datastoreResponse.m_updated = ts.isEmpty() ? QDateTime::currentDateTimeUtc() : CDatastoreUtility::parseTimestamp(ts);
                    datastoreResponse.m_restricted = responseObject["restricted"].toBool();
                }
                return datastoreResponse;
            }

            // no valid response
            QString error(nwReply->errorString());
            QString url(nwReply->url().toString());
            nwReply->abort();
            datastoreResponse.setMessage(CStatusMessage(cats, CStatusMessage::SeverityError,
                                         QString("Reading data failed: " + error + " " + url)));
            return datastoreResponse;
        }

        CDatabaseReader::JsonDatastoreResponse CDatabaseReader::setStatusAndTransformReplyIntoDatastoreResponse(QNetworkReply *nwReply)
        {
            this->setReplyStatus(nwReply);
            return this->transformReplyIntoDatastoreResponse(nwReply);
        }

        CDbInfoList CDatabaseReader::infoList() const
        {
            static const CDbInfoList e;
            if (!sApp->hasWebDataServices()) { return e; }
            if (!sApp->getWebDataServices()->getInfoDataReader()) { return e; }
            return sApp->getWebDataServices()->getInfoDataReader()->getDbInfoObjects();
        }

        bool CDatabaseReader::hasInfoObjects() const
        {
            return infoList().size() > 0;
        }

        QDateTime CDatabaseReader::getLatestEntityTimestampFromInfoObjects(CEntityFlags::Entity entity) const
        {
            static const QDateTime e;
            const CDbInfoList il(infoList());
            if (il.isEmpty() || entity == CEntityFlags::NoEntity) { return e; }
            CDbInfo info = il.findFirstByEntityOrDefault(entity);
            if (!info.isValid()) { return e; }
            return info.getUtcTimestamp();
        }

        int CDatabaseReader::getCountFromInfoObjects(CEntityFlags::Entity entity) const
        {
            static const QDateTime e;
            const CDbInfoList il(infoList());
            if (il.isEmpty() || entity == CEntityFlags::NoEntity) { return -1; }
            CDbInfo info = il.findFirstByEntityOrDefault(entity);
            if (!info.isValid()) { return -1; }
            return info.getEntries();
        }

        CDatabaseReaderConfig CDatabaseReader::getConfigForEntity(CEntityFlags::Entity entity) const
        {
            return this->m_config.findFirstOrDefaultForEntity(entity);
        }

        void CDatabaseReader::emitReadSignalPerSingleCachedEntity(CEntityFlags::Entity cachedEntities)
        {
            CEntityFlags::Entity cachedEntitiesToEmit = cachedEntities;
            CEntityFlags::Entity currentCachedEntity  = CEntityFlags::iterateDbEntities(cachedEntitiesToEmit);
            while (currentCachedEntity)
            {
                const int c = this->getCacheCount(currentCachedEntity);
                emit dataRead(currentCachedEntity, CEntityFlags::ReadFinished, c);
                currentCachedEntity  = CEntityFlags::iterateDbEntities(cachedEntitiesToEmit);
            }
        }

        bool CDatabaseReader::isChangedUrl(const CUrl &oldUrl, const CUrl &currentUrl)
        {
            if (oldUrl.isEmpty()) { return true; }
            Q_ASSERT_X(!currentUrl.isEmpty(), Q_FUNC_INFO, "No base URL");

            const QString oldS(oldUrl.getFullUrl(false));
            const QString currentS(currentUrl.getFullUrl(false));
            return oldS != currentS;
        }

        bool CDatabaseReader::hasReceivedOkReply() const
        {
            QReadLocker rl(&this->m_statusLock);
            return m_1stReplyReceived && m_1stReplyStatus == QNetworkReply::NoError;
        }

        bool CDatabaseReader::hasReceivedOkReply(QString &message) const
        {
            QReadLocker rl(&this->m_statusLock);
            message = m_statusMessage;
            return m_1stReplyReceived && m_1stReplyStatus == QNetworkReply::NoError;
        }

        bool CDatabaseReader::hasReceivedFirstReply() const
        {
            QReadLocker rl(&this->m_statusLock);
            return m_1stReplyReceived;
        }

        const QString &CDatabaseReader::getStatusMessage() const
        {
            return this->m_statusMessage;
        }

        void CDatabaseReader::setReplyStatus(QNetworkReply::NetworkError status, const QString &message)
        {
            QWriteLocker wl(&this->m_statusLock);
            this->m_statusMessage = message;
            this->m_1stReplyStatus = status;
            this->m_1stReplyReceived = true;
        }

        void CDatabaseReader::setReplyStatus(QNetworkReply *nwReply)
        {
            Q_ASSERT_X(nwReply, Q_FUNC_INFO, "Missing network reply");
            if (nwReply && nwReply->isFinished())
            {
                this->setReplyStatus(nwReply->error(), nwReply->errorString());
            }
        }

        const CLogCategoryList &CDatabaseReader::getLogCategories()
        {
            static const BlackMisc::CLogCategoryList cats
            (
                CThreadedReader::getLogCategories().join({ BlackMisc::CLogCategory::swiftDbWebservice(), BlackMisc::CLogCategory::mapping() })
            );
            return cats;
        }

        const QString &CDatabaseReader::parameterLatestTimestamp()
        {
            static const QString p("latestTimestamp");
            return p;
        }

        const QString &CDatabaseReader::parameterLatestId()
        {
            static const QString p("latestId");
            return p;
        }

        const CUrl &CDatabaseReader::getDbUrl()
        {
            static const CUrl dbUrl(sApp->getGlobalSetup().getDbRootDirectoryUrl());
            return dbUrl;
        }

        CUrl CDatabaseReader::getWorkingSharedUrl()
        {
            return sApp->getGlobalSetup().getSwiftSharedUrls().getRandomWorkingUrl();
        }

        bool CDatabaseReader::canPingSwiftServer()
        {
            return CNetworkUtils::canConnect(getDbUrl());
        }

        void CDatabaseReader::JsonDatastoreResponse::setJsonArray(const QJsonArray &value)
        {
            m_jsonArray = value;
            m_arraySize = value.size();
        }
    } // ns
} // ns
