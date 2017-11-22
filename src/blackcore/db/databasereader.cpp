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
#include "blackcore/db/databaseutils.h"
#include "blackcore/db/networkwatchdog.h"
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
using namespace BlackCore::Db;

namespace BlackCore
{
    namespace Db
    {
        CDatabaseReader::CDatabaseReader(QObject *owner, const CDatabaseReaderConfigList &config, const QString &name) :
            BlackCore::CThreadedReader(owner, name), m_config(config)
        { }

        void CDatabaseReader::readInBackgroundThread(CEntityFlags::Entity entities, const QDateTime &newerThan)
        {
            if (!this->doWorkCheck()) { return; }

            // we accept cached data
            Q_ASSERT_X(!entities.testFlag(CEntityFlags::DbInfoObjectEntity), Q_FUNC_INFO, "Read info objects directly");
            const bool hasInfoObjects = this->hasDbInfoObjects(); // no info objects is not necessarily an error, but indicates a) either data not available (DB down) or b) only caches are used
            CEntityFlags::Entity allEntities    = entities;
            CEntityFlags::Entity cachedEntities = CEntityFlags::NoEntity;
            CEntityFlags::Entity dbEntities     = CEntityFlags::NoEntity;
            CEntityFlags::Entity sharedEntities = CEntityFlags::NoEntity;
            CEntityFlags::Entity currentEntity  = CEntityFlags::iterateDbEntities(allEntities); // CEntityFlags::InfoObjectEntity will be ignored
            while (currentEntity)
            {
                const CDatabaseReaderConfig config(this->getConfigForEntity(currentEntity));
                const QString currentEntityName = CEntityFlags::flagToString(currentEntity);

                // retrieval mode
                const CDbFlags::DataRetrievalMode rm = config.getRetrievalMode(); // DB reading, cache, shared
                Q_ASSERT_X(!rm.testFlag(CDbFlags::Unspecified), Q_FUNC_INFO, "Missing retrieval mode");
                const QString rmString = CDbFlags::flagToString(rm);
                const CDbFlags::DataRetrievalModeFlag rmDbOrSharedFlag = CDbFlags::modeToModeFlag(rm & CDbFlags::DbReadingOrShared);
                const QString rmDbOrSharedFlagString = CDbFlags::flagToString(rmDbOrSharedFlag);
                const bool rmDbReadingOrShared = (rmDbOrSharedFlag == CDbFlags::DbReading || rmDbOrSharedFlag == CDbFlags::Shared);
                const int currentEntityCount = this->getCacheCount(currentEntity);

                if (rm.testFlag(CDbFlags::Ignore) || rm.testFlag(CDbFlags::Canceled))
                {
                    // do not load
                }
                else if (rm.testFlag(CDbFlags::Cached))
                {
                    // info object comparisons only for: cache + shared or cache + DB data
                    if (hasInfoObjects && rmDbReadingOrShared)
                    {
                        // check mode here for consistency
                        Q_ASSERT_X(!getBaseUrl(rmDbOrSharedFlag).isEmpty(), Q_FUNC_INFO, "Wrong retrieval mode");

                        CUrl oldUrlInfo;
                        CUrl newUrlInfo;
                        const bool changedUrl = this->hasChangedUrl(currentEntity, oldUrlInfo, newUrlInfo);
                        const QDateTime cacheTs(this->getCacheTimestamp(currentEntity));
                        const QDateTime latestEntityTs(this->getLatestEntityTimestampFromDbInfoObjects(currentEntity));
                        const qint64 cacheTimestamp = cacheTs.isValid() ? cacheTs.toMSecsSinceEpoch() : -1;
                        const qint64 latestEntityTimestamp = latestEntityTs.isValid() ? latestEntityTs.toMSecsSinceEpoch() : -1;
                        Q_ASSERT_X(latestEntityTimestamp >= 0, Q_FUNC_INFO, "Missing timestamp");
                        if (!changedUrl && cacheTimestamp >= latestEntityTimestamp && cacheTimestamp >= 0 && latestEntityTimestamp >= 0)
                        {
                            this->admitCaches(currentEntity);
                            cachedEntities |= currentEntity; // read from cache
                            CLogMessage(this).info("Using cache for '%1' (%2, %3)") << currentEntityName << cacheTs.toString() << cacheTimestamp;
                        }
                        else
                        {
                            Q_ASSERT_X(rmDbReadingOrShared, Q_FUNC_INFO, "Wrong retrieval mode");
                            if (rmDbOrSharedFlag == CDbFlags::DbReading) { dbEntities |= currentEntity; }
                            else if (rmDbOrSharedFlag == CDbFlags::Shared) { sharedEntities |= currentEntity; }

                            if (changedUrl)
                            {
                                CLogMessage(this).info("Data location for '%1' changed ('%2'->'%3'), will override cache for reading '%4'")
                                        << currentEntityName << oldUrlInfo.toQString()
                                        << newUrlInfo.toQString() << rmDbOrSharedFlagString;
                            }
                            else
                            {
                                CLogMessage(this).info("Cache for '%1' outdated, latest entity (%2, %3), reading '%4'") << currentEntityName << latestEntityTs.toString() << latestEntityTimestamp << rmDbOrSharedFlagString;
                            }
                        }
                    }
                    else
                    {
                        if (!rmDbReadingOrShared) { CLogMessage(this).info("No DB or shared reading for '%1'") << currentEntityName; }
                        if (!hasInfoObjects) { CLogMessage(this).info("No info objects for '%1'") << currentEntityName; }
                        if (currentEntityCount > 0)
                        {
                            CLogMessage(this).info("Cache for '%1' already read, %2 entries") << currentEntityName << currentEntityCount;
                        }
                        else
                        {
                            // no info objects, server down or no shared/db read mode
                            this->admitCaches(currentEntity);
                            if (!rmDbReadingOrShared)
                            {
                                // intentionally we do not want to read from DB/shared
                                CLogMessage(this).info("Triggered reading cache for '%1', read mode: %2") << currentEntityName << rmString;
                            }
                            else
                            {
                                // we want to read from DB/shared, but have no info object
                                CLogMessage(this).info("No info object for '%1', triggered reading cache, read mode: %2") << currentEntityName << rmString;
                            }
                        }
                        cachedEntities |= currentEntity; // read from cache
                    }
                }
                else
                {
                    // cache ignored
                    Q_ASSERT_X(rmDbReadingOrShared, Q_FUNC_INFO, "Wrong retrieval mode");
                    if (rmDbOrSharedFlag == CDbFlags::DbReading) { dbEntities |= currentEntity; }
                    else if (rmDbOrSharedFlag == CDbFlags::Shared) { sharedEntities |= currentEntity; }
                }
                currentEntity = CEntityFlags::iterateDbEntities(allEntities);
            }

            // signals for the cached entities
            if (cachedEntities != CEntityFlags::NoEntity)
            {
                this->emitReadSignalPerSingleCachedEntity(cachedEntities, true);
            }

            // Real read from DB
            if (dbEntities != CEntityFlags::NoEntity)
            {
                CLogMessage(this).info("Start reading DB entities: %1") << CEntityFlags::flagToString(dbEntities);
                this->startReadFromBackendInBackgroundThread(dbEntities, CDbFlags::DbReading, newerThan);
            }

            // Real read from shared
            if (sharedEntities != CEntityFlags::NoEntity)
            {
                CLogMessage(this).info("Start reading shared entities: %1") << CEntityFlags::flagToString(sharedEntities);
                this->startReadFromBackendInBackgroundThread(dbEntities, CDbFlags::Shared, newerThan);
            }
        }

        CEntityFlags::Entity CDatabaseReader::triggerLoadingDirectlyFromDb(CEntityFlags::Entity entities, const QDateTime &newerThan)
        {
            this->startReadFromBackendInBackgroundThread(entities, CDbFlags::DbReading, newerThan);
            return entities;
        }

        CEntityFlags::Entity CDatabaseReader::triggerLoadingDirectlyFromSharedFiles(CEntityFlags::Entity entities, bool checkCacheTsUpfront)
        {
            if (entities == CEntityFlags::NoEntity) { return CEntityFlags::NoEntity; }
            if (checkCacheTsUpfront)
            {
                CEntityFlags::Entity newerHeaderEntities = this->getEntitesWithNewerSharedInfoObject(entities);
                if (newerHeaderEntities != entities)
                {
                    const CEntityFlags::Entity validInCacheEntities = (entities ^ newerHeaderEntities) & entities;
                    CLogMessage(this).info("Reduced '%1' to '%2' before triggering load of shared files (still in cache)") << CEntityFlags::flagToString(entities) << CEntityFlags::flagToString(newerHeaderEntities);

                    // In case we have difference entities we treat them as they were loaded, they result from still being in the cache
                    // Using timer to first finish this function, then the resulting signal
                    if (validInCacheEntities != CEntityFlags::NoEntity)
                    {
                        QTimer::singleShot(0, this, [ = ]
                        {
                            emit this->dataRead(validInCacheEntities, CEntityFlags::ReadFinished, 0);
                        });
                    }
                    if (newerHeaderEntities == CEntityFlags::NoEntity) { return CEntityFlags::NoEntity; }
                    entities = newerHeaderEntities;
                }
            }
            this->startReadFromBackendInBackgroundThread(entities, CDbFlags::Shared);
            return entities;
        }

        void CDatabaseReader::startReadFromBackendInBackgroundThread(CEntityFlags::Entity entities, CDbFlags::DataRetrievalModeFlag mode, const QDateTime &newerThan)
        {
            Q_ASSERT_X(mode == CDbFlags::DbReading || mode == CDbFlags::Shared, Q_FUNC_INFO, "Wrong mode");

            // ps_read is implemented in the derived classes
            if (entities == CEntityFlags::NoEntity) { return; }
            if (!this->isInternetAccessible(QString("No network/internet access, will not read %1").arg(CEntityFlags::flagToString(entities)))) { return; }

            const bool s = QMetaObject::invokeMethod(this, "ps_read",
                           Q_ARG(BlackMisc::Network::CEntityFlags::Entity, entities),
                           Q_ARG(BlackMisc::Db::CDbFlags::DataRetrievalModeFlag, mode),
                           Q_ARG(QDateTime, newerThan));
            Q_ASSERT_X(s, Q_FUNC_INFO, "Invoke failed");
            Q_UNUSED(s);
        }

        CDatabaseReader::JsonDatastoreResponse CDatabaseReader::transformReplyIntoDatastoreResponse(QNetworkReply *nwReply) const
        {
            Q_ASSERT_X(nwReply, Q_FUNC_INFO, "missing reply");
            JsonDatastoreResponse datastoreResponse;
            const bool ok = this->setHeaderInfoPart(datastoreResponse, nwReply);
            if (ok)
            {
                const QString dataFileData = nwReply->readAll();
                nwReply->close(); // close asap
                if (dataFileData.isEmpty())
                {
                    datastoreResponse.setMessage(CStatusMessage(this, CStatusMessage::SeverityError, "Empty response, no data"));
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
            return headerResponse;
        }

        bool CDatabaseReader::setHeaderInfoPart(CDatabaseReader::HeaderResponse &headerResponse, QNetworkReply *nwReply) const
        {
            Q_ASSERT_X(nwReply, Q_FUNC_INFO, "Missing reply");
            this->threadAssertCheck();
            if (!this->doWorkCheck())
            {
                nwReply->abort();
                headerResponse.setMessage(CStatusMessage(this, CStatusMessage::SeverityError, "Terminated data parsing process"));
                return false; // stop, terminate straight away, ending thread
            }

            headerResponse.setValues(nwReply);
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
                                          QString("Reading data failed: '" + error + "' " + url)));
                return false;
            }
        }

        CDatabaseReader::JsonDatastoreResponse CDatabaseReader::setStatusAndTransformReplyIntoDatastoreResponse(QNetworkReply *nwReply)
        {
            this->setReplyStatus(nwReply);
            const CDatabaseReader::JsonDatastoreResponse dsr = this->transformReplyIntoDatastoreResponse(nwReply);
            if (dsr.isSharedFile())
            {
                this->receivedSharedFileHeaderNonClosing(nwReply);
            }
            else
            {
                if (dsr.isLoadedFromDb())
                {
                    const bool s = !dsr.hasErrorMessage();
                    emit this->swiftDbDataRead(s);
                }
            }
            return dsr;
        }

        CDbInfoList CDatabaseReader::getDbInfoObjects() const
        {
            static const CDbInfoList e;
            if (!sApp->hasWebDataServices()) { return e; }
            if (!sApp->getWebDataServices()->getDbInfoDataReader()) { return e; }
            return sApp->getWebDataServices()->getDbInfoDataReader()->getInfoObjects();
        }

        CDbInfoList CDatabaseReader::getSharedInfoObjects() const
        {
            static const CDbInfoList e;
            if (!sApp->hasWebDataServices()) { return e; }
            if (!sApp->getWebDataServices()->getSharedInfoDataReader()) { return e; }
            return sApp->getWebDataServices()->getSharedInfoDataReader()->getInfoObjects();
        }

        bool CDatabaseReader::hasDbInfoObjects() const
        {
            return getDbInfoObjects().size() > 0;
        }

        bool CDatabaseReader::hasSharedInfoObjects() const
        {
            return getSharedInfoObjects().size() > 0;
        }

        bool CDatabaseReader::hasSharedFileHeader(const CEntityFlags::Entity entity) const
        {
            Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "need single entity");
            return m_sharedFileResponses.contains(entity);
        }

        bool CDatabaseReader::hasSharedFileHeaders(const CEntityFlags::Entity entities) const
        {
            CEntityFlags::Entity myEntities = maskBySupportedEntities(entities);
            CEntityFlags::Entity currentEntity = CEntityFlags::iterateDbEntities(myEntities);
            while (currentEntity != CEntityFlags::NoEntity)
            {
                if (!hasSharedFileHeader(currentEntity)) { return false; }
                currentEntity = CEntityFlags::iterateDbEntities(myEntities);
            }
            return true;
        }

        QDateTime CDatabaseReader::getLatestEntityTimestampFromDbInfoObjects(CEntityFlags::Entity entity) const
        {
            Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "need single entity");
            static const QDateTime e;
            const CDbInfoList il(getDbInfoObjects());
            if (il.isEmpty() || entity == CEntityFlags::NoEntity) { return e; }

            // for some entities there can be more than one entry because of the
            // raw tables (see DB view last updates)
            const CDbInfo info = il.findFirstByEntityOrDefault(entity);
            if (!info.isValid()) { return e; }
            return info.getUtcTimestamp();
        }

        QDateTime CDatabaseReader::getLatestEntityTimestampFromSharedInfoObjects(CEntityFlags::Entity entity) const
        {
            Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "need single entity");
            static const QDateTime e;
            const CDbInfoList il(getSharedInfoObjects());
            if (il.isEmpty() || entity == CEntityFlags::NoEntity) { return e; }

            const CDbInfo info = il.findFirstByEntityOrDefault(entity);
            if (!info.isValid()) { return e; }
            return info.getUtcTimestamp();
        }

        QDateTime CDatabaseReader::getLatestSharedFileHeaderTimestamp(CEntityFlags::Entity entity) const
        {
            Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "need single entity");
            static const QDateTime e;
            if (!this->hasSharedFileHeader(entity)) { return e; }
            return m_sharedFileResponses[entity].getLastModifiedTimestamp();
        }

        int CDatabaseReader::requestHeadersOfSharedFiles(CEntityFlags::Entity entities)
        {
            if (!this->isInternetAccessible(QString("No network/internet access, will not read shared file headers for %1").arg(CEntityFlags::flagToString(entities)))) { return false; }

            CEntityFlags::Entity allEntities(this->maskBySupportedEntities(entities));
            CEntityFlags::Entity currentEntity = CEntityFlags::iterateDbEntities(allEntities);
            const CUrl urlSharedDbdata = CDatabaseReader::getWorkingSharedDbdataDirectoryUrl();
            if (urlSharedDbdata.isEmpty())
            {
                CLogMessage(this).warning("No working shared URL, cannot request headers");
                return 0;
            }

            int c = 0;
            while (currentEntity != CEntityFlags::NoEntity)
            {
                const QString fileName = CDbInfo::entityToSharedName(currentEntity);
                Q_ASSERT_X(!fileName.isEmpty(), Q_FUNC_INFO, "No file name for entity");
                CUrl url = urlSharedDbdata;
                url.appendPath(fileName);

                const QString entityString = CEntityFlags::flagToString(currentEntity);
                CLogMessage(this).info("Triggered read of header for shared file of '%1'") << entityString;
                const QNetworkReply *reply = sApp->headerFromNetwork(url, { this, &CDatabaseReader::receivedSharedFileHeader });
                if (reply) { c++; }
                currentEntity = CEntityFlags::iterateDbEntities(allEntities);
            }
            return c > 0;
        }

        bool CDatabaseReader::isSharedHeaderNewerThanCacheTimestamp(CEntityFlags::Entity entity) const
        {
            Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "need single entity");
            const QDateTime cacheTs(this->getCacheTimestamp(entity));
            if (!cacheTs.isValid()) { return true; } // we have no cache ts

            const QDateTime headerTimestamp(this->getLatestSharedFileHeaderTimestamp(entity));
            if (!headerTimestamp.isValid()) { return false; }
            return headerTimestamp > cacheTs;
        }

        bool CDatabaseReader::isSharedInfoObjectNewerThanCacheTimestamp(CEntityFlags::Entity entity) const
        {
            Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "need single entity");
            const QDateTime cacheTs(this->getCacheTimestamp(entity));
            if (!cacheTs.isValid()) { return true; } // we have no cache ts

            const QDateTime sharedInfoTimestamp(this->getLatestEntityTimestampFromSharedInfoObjects(entity));
            if (!sharedInfoTimestamp.isValid()) { return false; }
            return sharedInfoTimestamp > cacheTs;
        }

        CEntityFlags::Entity CDatabaseReader::getEntitesWithNewerHeaderTimestamp(CEntityFlags::Entity entities) const
        {
            entities = this->maskBySupportedEntities(entities); // handled by this reader
            CEntityFlags::Entity currentEntity = CEntityFlags::iterateDbEntities(entities);
            CEntityFlags::Entity newerEntities = CEntityFlags::NoEntity;
            while (currentEntity != CEntityFlags::NoEntity)
            {
                if (this->isSharedHeaderNewerThanCacheTimestamp(currentEntity))
                {
                    newerEntities |= currentEntity;
                }
                currentEntity = CEntityFlags::iterateDbEntities(entities);
            }
            return newerEntities;
        }

        CEntityFlags::Entity CDatabaseReader::getEntitesWithNewerSharedInfoObject(CEntityFlags::Entity entities) const
        {
            entities = this->maskBySupportedEntities(entities); // handled by this reader
            CEntityFlags::Entity currentEntity = CEntityFlags::iterateDbEntities(entities);
            CEntityFlags::Entity newerEntities = CEntityFlags::NoEntity;
            while (currentEntity != CEntityFlags::NoEntity)
            {
                if (this->isSharedInfoObjectNewerThanCacheTimestamp(currentEntity))
                {
                    newerEntities |= currentEntity;
                }
                currentEntity = CEntityFlags::iterateDbEntities(entities);
            }
            return newerEntities;
        }

        CDatabaseReaderConfig CDatabaseReader::getConfigForEntity(CEntityFlags::Entity entity) const
        {
            return m_config.findFirstOrDefaultForEntity(entity);
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
                    emit this->dataRead(currentCachedEntity, CEntityFlags::ReadFinished, c);
                    emitted |= currentCachedEntity;
                }
                currentCachedEntity = CEntityFlags::iterateDbEntities(cachedEntitiesToEmit);
            }
            return emitted;
        }

        void CDatabaseReader::emitAndLogDataRead(CEntityFlags::Entity entity, int number, const JsonDatastoreResponse &res)
        {
            // never emit when lock is held, deadlock
            Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "Expect single entity");
            emit this->dataRead(entity, res.isRestricted() ? CEntityFlags::ReadFinishedRestricted : CEntityFlags::ReadFinished, number);
            CLogMessage(this).info("Read %1 entities of '%2' from '%3' (%4)") << number << CEntityFlags::flagToString(entity) << res.getUrlString() << res.getLoadTimeStringWithStartedHint();
        }

        void CDatabaseReader::logNoWorkingUrl(CEntityFlags::Entity entity)
        {
            const CStatusMessage msg = CStatusMessage(this, m_severityNoWorkingUrl, "No working URL for '%1'") << CEntityFlags::flagToString(entity);
            CLogMessage::preformatted(msg);
        }

        CUrl CDatabaseReader::getBaseUrl(CDbFlags::DataRetrievalModeFlag mode) const
        {
            Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing app object, URLs cannot be obtained");
            switch (mode)
            {
            case CDbFlags::DbReading:
                return this->getDbServiceBaseUrl().withAppendedPath("/service");
            case CDbFlags::SharedInfoOnly:
            case CDbFlags::Shared:
                return CDatabaseReader::getWorkingSharedDbdataDirectoryUrl();
            default:
                qFatal("Wrong mode");
                break;
            }
            return CUrl();
        }

        bool CDatabaseReader::isChangedUrl(const CUrl &oldUrl, const CUrl &currentUrl)
        {
            if (oldUrl.isEmpty()) { return true; }
            Q_ASSERT_X(!currentUrl.isEmpty(), Q_FUNC_INFO, "No base URL");

            const QString old(oldUrl.getFullUrl(false));
            const QString current(currentUrl.getFullUrl(false));
            return old != current;
        }

        void CDatabaseReader::receivedSharedFileHeader(QNetworkReply *nwReplyPtr)
        {
            // wrap pointer, make sure any exit cleans up reply
            // required to use delete later as object is created in a different thread
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
            if (!this->doWorkCheck()) { return; }
            this->receivedSharedFileHeaderNonClosing(nwReplyPtr);
            nwReply->close();
        }

        void CDatabaseReader::receivedSharedFileHeaderNonClosing(QNetworkReply *nwReplyPtr)
        {
            if (this->isAbandoned()) { return; }

            const HeaderResponse headerResponse = this->transformReplyIntoHeaderResponse(nwReplyPtr);
            const QString fileName = nwReplyPtr->url().fileName();
            const CEntityFlags::Entity entity = CEntityFlags::singleEntityByName(fileName);
            m_sharedFileResponses[entity] = headerResponse;

            CLogMessage(this).info("Received header for shared file of '%1' from '%2'") << fileName << headerResponse.getUrl().toQString();
            emit this->sharedFileHeaderRead(entity, fileName, !headerResponse.hasWarningOrAboveMessage());
        }

        bool CDatabaseReader::hasReceivedOkReply() const
        {
            QReadLocker rl(&m_statusLock);
            return m_1stReplyReceived && m_1stReplyStatus == QNetworkReply::NoError;
        }

        bool CDatabaseReader::hasReceivedOkReply(QString &message) const
        {
            QReadLocker rl(&m_statusLock);
            message = m_statusMessage;
            return m_1stReplyReceived && m_1stReplyStatus == QNetworkReply::NoError;
        }

        bool CDatabaseReader::hasReceivedFirstReply() const
        {
            QReadLocker rl(&m_statusLock);
            return m_1stReplyReceived;
        }

        CEntityFlags::Entity CDatabaseReader::maskBySupportedEntities(CEntityFlags::Entity entities) const
        {
            return entities & getSupportedEntities();
        }

        bool CDatabaseReader::supportsAnyOfEntities(CEntityFlags::Entity entities) const
        {
            return static_cast<int>(maskBySupportedEntities(entities)) > 0;
        }

        bool CDatabaseReader::hasCacheTimestampNewerThan(CEntityFlags::Entity entity, const QDateTime &threshold) const
        {
            const QDateTime ts = this->getCacheTimestamp(entity);
            if (!ts.isValid()) return false;
            return ts > threshold;
        }

        const QString &CDatabaseReader::getStatusMessage() const
        {
            return m_statusMessage;
        }

        void CDatabaseReader::setReplyStatus(QNetworkReply::NetworkError status, const QString &message)
        {
            QWriteLocker wl(&m_statusLock);
            m_statusMessage = message;
            m_1stReplyStatus = status;
            m_1stReplyReceived = true;
        }

        void CDatabaseReader::setReplyStatus(QNetworkReply *nwReply)
        {
            Q_ASSERT_X(nwReply, Q_FUNC_INFO, "Missing network reply");
            if (nwReply && nwReply->isFinished())
            {
                this->logNetworkReplyReceived(nwReply);
                this->setReplyStatus(nwReply->error(), nwReply->errorString());
            }
        }

        QString CDatabaseReader::fileNameForMode(CEntityFlags::Entity entity, CDbFlags::DataRetrievalModeFlag mode)
        {
            Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "needs single entity");
            switch (mode)
            {
            case CDbFlags::Shared: return CDbInfo::entityToSharedName(entity);
            case CDbFlags::SharedInfoOnly: return CDbInfo::sharedInfoFileName();
            default:
            case CDbFlags::DbReading:
                return CDbInfo::entityToServiceName(entity);
            }
        }

        QString CDatabaseReader::dateTimeToDbLatestTs(const QDateTime &ts)
        {
            if (!ts.isValid()) { return ""; }
            return ts.toUTC().toString(Qt::ISODate);
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

        QString CDatabaseReader::queryLatestTimestamp(const QDateTime &ts)
        {
            if (!ts.isValid()) return "";
            const QString q = parameterLatestTimestamp() + "=" + dateTimeToDbLatestTs(ts);
            return q;
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

        CUrl CDatabaseReader::getWorkingSharedDbdataDirectoryUrl()
        {
            const CUrl sharedUrl(sApp->getWorkingSharedUrl());
            return CGlobalSetup::buildDbDataDirectoryUrl(sharedUrl);
        }

        void CDatabaseReader::cacheHasChanged(CEntityFlags::Entity entities)
        {
            this->emitReadSignalPerSingleCachedEntity(entities, false);
        }

        void CDatabaseReader::stringToDatastoreResponse(const QString &jsonContent, JsonDatastoreResponse &datastoreResponse)
        {
            const int status = datastoreResponse.getHttpStatusCode();
            if (jsonContent.isEmpty())
            {
                static const QString errorMsg = "Empty JSON string, status: %1, URL: '%2', load time: %3";
                datastoreResponse.setMessage(CStatusMessage(getLogCategories(),
                                             CStatusMessage::SeverityError,
                                             errorMsg.arg(status).arg(datastoreResponse.getUrlString(), datastoreResponse.getLoadTimeStringWithStartedHint())));
                return;
            }

            const QJsonDocument jsonResponse = CDatabaseUtils::databaseJsonToQJsonDocument(jsonContent);
            if (jsonResponse.isEmpty())
            {
                if (CNetworkUtils::looksLikePhpErrorMessage(jsonContent))
                {
                    static const QString errorMsg = "Looks like PHP errror, status %1, URL: '%2', msg: %3";
                    const QString phpErrorMessage = CNetworkUtils::removeHtmlPartsFromPhpErrorMessage(jsonContent);
                    datastoreResponse.setMessage(CStatusMessage(getLogCategories(),
                                                 CStatusMessage::SeverityError,
                                                 errorMsg.arg(status).arg(datastoreResponse.getUrlString(), phpErrorMessage)));
                }
                else
                {
                    static const QString errorMsg = "Empty JSON document, URL: '%1', load time: %2";
                    datastoreResponse.setMessage(CStatusMessage(getLogCategories(),
                                                 CStatusMessage::SeverityError,
                                                 errorMsg.arg(datastoreResponse.getUrlString(), datastoreResponse.getLoadTimeStringWithStartedHint())));
                }
                return;
            }

            if (jsonResponse.isArray())
            {
                // directly an array, no further info
                datastoreResponse.setJsonArray(jsonResponse.array());
                datastoreResponse.setLastModifiedTimestamp(QDateTime::currentDateTimeUtc());
            }
            else
            {
                const QJsonObject responseObject(jsonResponse.object());
                datastoreResponse.setJsonArray(responseObject["data"].toArray());
                const QString ts(responseObject["latest"].toString());
                datastoreResponse.setLastModifiedTimestamp(ts.isEmpty() ? QDateTime::currentDateTimeUtc() : CDatastoreUtility::parseTimestamp(ts));
                datastoreResponse.setRestricted(responseObject["restricted"].toBool());
            }
        }

        bool CDatabaseReader::JsonDatastoreResponse::isLoadedFromDb() const
        {
            return CNetworkWatchdog::isDbUrl(this->getUrl());
        }

        void CDatabaseReader::JsonDatastoreResponse::setJsonArray(const QJsonArray &value)
        {
            m_jsonArray = value;
            m_arraySize = value.size();
        }

        bool CDatabaseReader::HeaderResponse::isSharedFile() const
        {
            const QString fn(getUrl().getFileName());
            return CDbInfo::sharedFileNames().contains(fn, Qt::CaseInsensitive);
        }

        QString CDatabaseReader::HeaderResponse::getLoadTimeString() const
        {
            return QString("%1ms").arg(getLoadTimeMs());
        }

        QString CDatabaseReader::HeaderResponse::getLoadTimeStringWithStartedHint() const
        {
            if (m_requestStarted < 0) { return this->getLoadTimeString(); }
            const qint64 diff = QDateTime::currentMSecsSinceEpoch() - m_requestStarted;
            static const QString s("%1 load time, started %2ms before now");
            return s.arg(this->getLoadTimeString()).arg(diff);
        }

        void CDatabaseReader::HeaderResponse::setValues(const QNetworkReply *nwReply)
        {
            Q_ASSERT_X(nwReply, Q_FUNC_INFO, "Need valid reply");
            this->setUrl(nwReply->url());
            const QVariant started = nwReply->property("started");
            if (started.isValid() && started.canConvert<qint64>())
            {
                const qint64 now = QDateTime::currentMSecsSinceEpoch();
                const qint64 start = started.value<qint64>();
                this->setLoadTimeMs(now - start);
                m_requestStarted = start;
                m_responseReceived = now;
            }

            const QVariant qvStatusCode = nwReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            if (qvStatusCode.isValid() && qvStatusCode.canConvert<int>()) { m_httpStatusCode = qvStatusCode.toInt(); }

            const QDateTime lastModified = nwReply->header(QNetworkRequest::LastModifiedHeader).toDateTime();
            const qulonglong size = nwReply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
            this->setLastModifiedTimestamp(lastModified);
            this->setContentLengthHeader(size);
        }
    } // ns
} // ns
