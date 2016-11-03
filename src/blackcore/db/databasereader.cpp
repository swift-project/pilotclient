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
using namespace BlackCore::Data;

namespace BlackCore
{
    namespace Db
    {
        CDatabaseReader::CDatabaseReader(QObject *owner, const CDatabaseReaderConfigList &config, const QString &name) :
            BlackCore::CThreadedReader(owner, name), m_config(config)
        {
            getDbUrl(); // init the cache
        }

        void CDatabaseReader::readInBackgroundThread(CEntityFlags::Entity entities, const QDateTime &newerThan)
        {
            if (isAbandoned()) { return; }

            // we accept cached cached data
            Q_ASSERT_X(!entities.testFlag(CEntityFlags::InfoObjectEntity), Q_FUNC_INFO, "Read info objects directly");
            const bool hasInfoObjects = this->hasInfoObjects(); // no info objects is no necessarily error, but indicates a) either data not available (DB down) or b) only caches are used
            CEntityFlags::Entity allEntities    = entities;
            CEntityFlags::Entity cachedEntities = CEntityFlags::NoEntity;
            CEntityFlags::Entity currentEntity  = CEntityFlags::iterateDbEntities(allEntities); // CEntityFlags::InfoObjectEntity will be ignored
            while (currentEntity)
            {
                const CDatabaseReaderConfig config(this->getConfigForEntity(currentEntity));
                const QString currentEntityName = CEntityFlags::flagToString(currentEntity);
                const CDbFlags::DataRetrievalMode rm = config.getRetrievalMode();
                Q_ASSERT_X(!rm.testFlag(CDbFlags::Unspecified), Q_FUNC_INFO, "Missing retrieval mode");
                if (rm.testFlag(CDbFlags::Ignore) || rm.testFlag(CDbFlags::Canceled))
                {
                    entities &= ~currentEntity; // do not load from web database
                }
                else if (rm.testFlag(CDbFlags::Cached))
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
                            this->admitCaches(currentEntity);
                            entities &= ~currentEntity; // do not load from web database
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
                        this->admitCaches(currentEntity);
                        const int c = this->getCacheCount(currentEntity);
                        CLogMessage(this).info("No info object for %1, using cache with %2 objects") << currentEntityName << c;
                        entities &= ~currentEntity; // do not load from web database
                        cachedEntities |= currentEntity; // read from cache
                    }
                }
                currentEntity = CEntityFlags::iterateDbEntities(allEntities);
            }

            // signals for the cached entities
            this->emitReadSignalPerSingleCachedEntity(cachedEntities, true);

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
            JsonDatastoreResponse datastoreResponse;
            const bool ok = this->setHeaderInfoPart(datastoreResponse, nwReply);
            if (ok)
            {
                const QString dataFileData = nwReply->readAll().trimmed();
                nwReply->close(); // close asap
                if (dataFileData.isEmpty())
                {
                    datastoreResponse.setMessage(CStatusMessage(this, CStatusMessage::SeverityError, "Empty response, no data"));
                    datastoreResponse.setUpdateTimestamp(QDateTime::currentDateTimeUtc());
                }
                else
                {
                    CDatabaseReader::stringToDatastoreResponse(dataFileData, datastoreResponse);
                }
            }
            return datastoreResponse;
        }

        CDatabaseReader::HeaderResponse CDatabaseReader::transformReplyIntoHeaderResponse(QNetworkReply *nwReply) const
        {
            HeaderResponse headerResponse;
            const bool success = this->setHeaderInfoPart(headerResponse, nwReply);
            Q_UNUSED(success);
            nwReply->close();
            return headerResponse;
        }

        bool CDatabaseReader::setHeaderInfoPart(CDatabaseReader::HeaderResponse &headerResponse, QNetworkReply *nwReply) const
        {
            Q_ASSERT_X(nwReply, Q_FUNC_INFO, "Missing reply");
            this->threadAssertCheck();
            if (this->isAbandoned())
            {
                nwReply->abort();
                headerResponse.setMessage(CStatusMessage(this, CStatusMessage::SeverityError, "Terminated data parsing process"));
                return false; // stop, terminate straight away, ending thread
            }

            headerResponse.setUrl(nwReply->url());
            const QVariant started = nwReply->property("started");
            if (started.isValid() && started.canConvert<qint64>())
            {
                const qint64 now = QDateTime::currentMSecsSinceEpoch();
                const qint64 start = started.value<qint64>();
                headerResponse.setLoadTimeMs(now - start);
            }

            const QDateTime lastModified = nwReply->header(QNetworkRequest::LastModifiedHeader).toDateTime();
            const qulonglong size = static_cast<qlonglong>(nwReply->header(QNetworkRequest::ContentLengthHeader).toULongLong());
            headerResponse.setUpdateTimestamp(lastModified);
            headerResponse.setContentLengthHeader(size);

            if (nwReply->error() == QNetworkReply::NoError)
            {
                // do not close because of obtaining data
                return true;
            }
            else
            {
                // no valid response
                const QString error(nwReply->errorString());
                const QString url(nwReply->url().toString());
                nwReply->abort();
                headerResponse.setMessage(CStatusMessage(this, CStatusMessage::SeverityError,
                                          QString("Reading data failed: " + error + " " + url)));
                return false;
            }
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

            // for some entities there can be more than one entry because of the
            // raw tables (see DB view last updates)
            const CDbInfo info = il.findFirstByEntityOrDefault(entity);
            if (!info.isValid()) { return e; }
            return info.getUtcTimestamp();
        }

        QDateTime CDatabaseReader::getSharedFileTimestamp(CEntityFlags::Entity entity) const
        {
            const int key = static_cast<int>(entity);
            if (m_sharedFileResponses.contains(key))
            {
                return this->m_sharedFileResponses[key].getUpdateTimestamp();
            }
            else
            {
                return QDateTime();
            }
        }

        bool CDatabaseReader::requestHeadersOfSharedFiles(const CEntityFlags::Entity &entities)
        {
            CEntityFlags::Entity allEntities(entities);
            CEntityFlags::Entity currentEntity = CEntityFlags::iterateDbEntities(allEntities);
            CUrl urlDbData = CGlobalSetup::buildDbDataDirectory(getWorkingSharedUrl());

            int c = 0;
            while (currentEntity != CEntityFlags::NoEntity)
            {
                const QString fileName = CDbInfo::entityToSharedFileName(currentEntity);
                Q_ASSERT_X(!fileName.isEmpty(), Q_FUNC_INFO, "No file name for entity");
                CUrl url = urlDbData;
                url.appendPath(fileName);

                const QString entityString = CEntityFlags::flagToString(currentEntity);
                CLogMessage(this).info("Triggered read of header for shared file of %1") << entityString;
                const QNetworkReply *reply = sApp->headerFromNetwork(url, { this, &CDatabaseReader::receivedSharedFileHeader });
                if (reply) { c++; }
                currentEntity = CEntityFlags::iterateDbEntities(allEntities);
            }
            return c > 0;
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

        CEntityFlags::Entity CDatabaseReader::emitReadSignalPerSingleCachedEntity(CEntityFlags::Entity cachedEntities, bool onlyIfHasData)
        {
            if (cachedEntities == CEntityFlags::NoEntity) { return CEntityFlags::NoEntity; }
            CEntityFlags::Entity emitted = CEntityFlags::NoEntity;
            CEntityFlags::Entity cachedEntitiesToEmit = cachedEntities;
            CEntityFlags::Entity currentCachedEntity  = CEntityFlags::iterateDbEntities(cachedEntitiesToEmit);
            while (currentCachedEntity)
            {
                const int c = this->getCacheCount(currentCachedEntity);
                if (!onlyIfHasData || c > 0)
                {
                    emit dataRead(currentCachedEntity, CEntityFlags::ReadFinished, c);
                    emitted |= currentCachedEntity;
                }
                currentCachedEntity  = CEntityFlags::iterateDbEntities(cachedEntitiesToEmit);
            }
            return emitted;
        }

        bool CDatabaseReader::isChangedUrl(const CUrl &oldUrl, const CUrl &currentUrl)
        {
            if (oldUrl.isEmpty()) { return true; }
            Q_ASSERT_X(!currentUrl.isEmpty(), Q_FUNC_INFO, "No base URL");

            const QString oldS(oldUrl.getFullUrl(false));
            const QString currentS(currentUrl.getFullUrl(false));
            return oldS != currentS;
        }

        void CDatabaseReader::receivedSharedFileHeader(QNetworkReply *nwReplyPtr)
        {
            // wrap pointer, make sure any exit cleans up reply
            // required to use delete later as object is created in a different thread
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
            if (this->isAbandoned()) { return; }

            const HeaderResponse headerResponse = this->transformReplyIntoHeaderResponse(nwReply.data());
            const QString fileName = nwReply->url().fileName();
            const CEntityFlags::Entity entity = CEntityFlags::singleEntityByName(fileName);
            const int key = static_cast<int>(entity);
            this->m_sharedFileResponses[key] = headerResponse;

            emit this->sharedFileHeaderRead(entity, fileName, !headerResponse.hasWarningOrAboveMessage());
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
                CThreadedReader::getLogCategories().join({ BlackMisc::CLogCategory::swiftDbWebservice(), BlackMisc::CLogCategory::webservice() })
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

        void CDatabaseReader::cacheHasChanged(CEntityFlags::Entity entities)
        {
            this->emitReadSignalPerSingleCachedEntity(entities, false);
        }

        bool CDatabaseReader::canPingSwiftServer()
        {
            const CUrl url(getDbUrl());
            return CNetworkUtils::canConnect(url);
        }

        void CDatabaseReader::stringToDatastoreResponse(const QString &jsonContent, JsonDatastoreResponse &datastoreResponse)
        {
            if (jsonContent.isEmpty())
            {
                datastoreResponse.setMessage(CStatusMessage(getLogCategories(), CStatusMessage::SeverityError, "Empty string, no data"));
                datastoreResponse.setUpdateTimestamp(QDateTime::currentDateTimeUtc());
                return;
            }

            const QJsonDocument jsonResponse = QJsonDocument::fromJson(jsonContent.toUtf8());
            if (jsonResponse.isArray())
            {
                // directly an array, no further info
                datastoreResponse.setJsonArray(jsonResponse.array());
                datastoreResponse.setUpdateTimestamp(QDateTime::currentDateTimeUtc());
            }
            else
            {
                const QJsonObject responseObject(jsonResponse.object());
                datastoreResponse.setJsonArray(responseObject["data"].toArray());
                const QString ts(responseObject["latest"].toString());
                datastoreResponse.setUpdateTimestamp(ts.isEmpty() ? QDateTime::currentDateTimeUtc() : CDatastoreUtility::parseTimestamp(ts));
                datastoreResponse.setRestricted(responseObject["restricted"].toBool());
            }
        }

        void CDatabaseReader::JsonDatastoreResponse::setJsonArray(const QJsonArray &value)
        {
            m_jsonArray = value;
            m_arraySize = value.size();
        }
    } // ns
} // ns
