/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DB_DATABASEREADER_H
#define BLACKCORE_DB_DATABASEREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/db/databasereaderconfig.h"
#include "blackmisc/db/dbinfolist.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/network/url.h"
#include "blackmisc/statusmessage.h"
#include "blackcore/threadedreader.h"
#include "blackmisc/sequence.h"
#include "blackmisc/valueobject.h"

#include <QDateTime>
#include <QJsonArray>
#include <QMap>
#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QTimer>
#include <QtGlobal>
#include <QNetworkReply>

class QNetworkReply;
namespace BlackMisc { class CLogCategoryList; }
namespace BlackCore
{
    namespace Db
    {
        //! Specialized version of threaded reader for DB data
        class BLACKCORE_EXPORT CDatabaseReader : public BlackCore::CThreadedReader
        {
            Q_OBJECT

        public:
            //! Header response part
            struct HeaderResponse
            {
            private:
                QDateTime  m_lastModified;             //!< when was the latest update?
                qulonglong m_contentLengthHeader = 0;  //!< content length
                qint64     m_loadTimeMs = -1;          //!< how long did it take to load
                BlackMisc::CStatusMessage m_message;   //!< last error or warning
                BlackMisc::Network::CUrl  m_url;       //!< loaded url

            public:
                //! Any timestamp?
                bool hasTimestamp() const { return m_lastModified.isValid(); }

                //! Is response newer?
                bool isNewer(const QDateTime &ts) const { return m_lastModified.toMSecsSinceEpoch() > ts.toMSecsSinceEpoch(); }

                //! Is response newer?
                bool isNewer(qint64 mSecsSinceEpoch) const { return m_lastModified.toMSecsSinceEpoch() > mSecsSinceEpoch; }

                //! Get the "last-modified" timestamp
                const QDateTime &getLastModifiedTimestamp() const { return m_lastModified; }

                //! Set update timestamp, default normally "last-modified"
                void setLastModifiedTimestamp(const QDateTime &updated) { m_lastModified = updated; }

                //! Header content length
                qulonglong getContentLengthHeader() const { return m_contentLengthHeader; }

                //! Set the content length
                void setContentLengthHeader(qulonglong size) { m_contentLengthHeader = size; }

                //! Error message?
                bool hasErrorMessage() const { return m_message.getSeverity() == BlackMisc::CStatusMessage::SeverityError; }

                //! Warning or error message?
                bool hasWarningOrAboveMessage() const { return m_message.isWarningOrAbove(); }

                //! Last error or warning
                const BlackMisc::CStatusMessage &lastWarningOrAbove() const { return m_message; }

                //! Set the error/warning message
                void setMessage(const BlackMisc::CStatusMessage &lastErrorOrWarning) { m_message = lastErrorOrWarning; }

                //! URL loaded
                const BlackMisc::Network::CUrl &getUrl() const { return m_url; }

                //! URL loaded as string
                QString getUrlString() const { return m_url.toQString(); }

                //! Set the loaded URL
                void setUrl(const BlackMisc::Network::CUrl &url) { m_url = url; }

                //! Is a shared file?
                bool isSharedFile() const;

                //! Load time in ms (from request to response)
                qint64 getLoadTimeMs() const { return m_loadTimeMs; }

                //! Load time as string
                QString getLoadTimeString() const { return QString("%1ms").arg(getLoadTimeMs()); }

                //! Set the load time (delta start -> response received)
                void setLoadTimeMs(qint64 deltaTime) { m_loadTimeMs = deltaTime; }
            };

            //!  Response from our database (depending on JSON DB backend generates)
            struct JsonDatastoreResponse : public HeaderResponse
            {
            private:
                QJsonArray m_jsonArray;          //!< JSON array data
                int        m_arraySize  = -1;    //!< size of array, if applicable (copied to member for debugging purposes)
                bool       m_restricted = false; //!< restricted reponse, only changed data

            public:
                //! Any data?
                bool isEmpty() const { return m_jsonArray.isEmpty(); }

                //! Number of elements
                int size() const { return m_jsonArray.size(); }

                //! Incremental data, restricted by query?
                bool isRestricted() const { return m_restricted; }

                //! Mark as restricted
                void setRestricted(bool restricted) { m_restricted = restricted; }

                //! Get the JSON array
                QJsonArray getJsonArray() const { return m_jsonArray; }

                //! Set the JSON array
                void setJsonArray(const QJsonArray &value);

                //! Implicit conversion
                operator QJsonArray() const { return m_jsonArray; }
            };

            //! Start reading in own thread
            //! \remark uses caches, info objects
            void readInBackgroundThread(BlackMisc::Network::CEntityFlags::Entity entities, const QDateTime &newerThan);

            //! Start loading from DB in own thread
            //! \remark bypass caches/config
            BlackMisc::Network::CEntityFlags::Entity triggerLoadingDirectlyFromDb(BlackMisc::Network::CEntityFlags::Entity entities, const QDateTime &newerThan);

            //! Start loading from shared files in own thread
            //! \remark bypass caches/config
            BlackMisc::Network::CEntityFlags::Entity triggerLoadingDirectlyFromSharedFiles(BlackMisc::Network::CEntityFlags::Entity entities, bool checkCacheTsUpfront);

            //! Has received Ok response from server at least once?
            //! \threadsafe
            bool hasReceivedOkReply() const;

            //! Has received Ok response from server?
            //! A message why connect failed can be obtained.
            //! \threadsafe
            bool hasReceivedOkReply(QString &message) const;

            //! Has received 1st reply?
            //! \threadsafe
            bool hasReceivedFirstReply() const;

            //! Supported entities by this reader
            virtual BlackMisc::Network::CEntityFlags::Entity getSupportedEntities() const = 0;

            //! Mask by supported entities
            BlackMisc::Network::CEntityFlags::Entity maskBySupportedEntities(BlackMisc::Network::CEntityFlags::Entity entities) const;

            //! Is any of the given entities supported here by this reader
            bool supportsAnyOfEntities(BlackMisc::Network::CEntityFlags::Entity entities) const;

            //! Get cache timestamp
            virtual QDateTime getCacheTimestamp(BlackMisc::Network::CEntityFlags::Entity entity) const = 0;

            //! Cache`s number of entities
            virtual int getCacheCount(BlackMisc::Network::CEntityFlags::Entity entity) const = 0;

            //! Info objects available?
            bool hasInfoObjects() const;

            //! Header of shared file read (for single entity)?
            bool hasSharedFileHeader(const BlackMisc::Network::CEntityFlags::Entity entity) const;

            //! Headers of shared file read (for single entity)?
            bool hasSharedFileHeaders(const BlackMisc::Network::CEntityFlags::Entity entities) const;

            //! Obtain latest object timestamp from info objects
            //! \sa BlackCore::Db::CInfoDataReader
            QDateTime getLatestEntityTimestampFromInfoObjects(BlackMisc::Network::CEntityFlags::Entity entity) const;

            //! Header timestamp (last-modified) for shared file
            QDateTime getLatestSharedFileHeaderTimestamp(BlackMisc::Network::CEntityFlags::Entity entity) const;

            //! Is the file timestamp neer than cache timestamp?
            bool isSharedHeaderNewerThanCacheTimestamp(BlackMisc::Network::CEntityFlags::Entity entity) const;

            //! Those entities where the timestamp of header is newer than the cache timestamp
            BlackMisc::Network::CEntityFlags::Entity getEntitesWithNewerHeaderTimestamp(BlackMisc::Network::CEntityFlags::Entity entities) const;

            //! Request header of shared file
            bool requestHeadersOfSharedFiles(const BlackMisc::Network::CEntityFlags::Entity &entities);

            //! Count from info objects
            //! \sa BlackCore::Db::CInfoDataReader
            int getCountFromInfoObjects(BlackMisc::Network::CEntityFlags::Entity entity) const;

            //! Status message (error message)
            const QString &getStatusMessage() const;

            //! Log categories
            static const BlackMisc::CLogCategoryList &getLogCategories();

            //! Name of latest timestamp
            static const QString &parameterLatestTimestamp();

            //! Name of parameter for latest id
            static const QString &parameterLatestId();

            //! swift DB server reachable?
            static bool canPingSwiftServer();

            //! Init the working URLs
            static bool initWorkingUrls(bool force = false);

            //! Currently used URL for shared DB data
            static BlackMisc::Network::CUrl getCurrentSharedDbDataUrl();

            //! Transform JSON data to response struct data
            //! \private used also for samples, that`s why it is declared public
            static void stringToDatastoreResponse(const QString &jsonContent, CDatabaseReader::JsonDatastoreResponse &datastoreResponse);

        signals:
            //! Combined read signal
            void dataRead(BlackMisc::Network::CEntityFlags::Entity entities, BlackMisc::Network::CEntityFlags::ReadState state, int number);

            //! Header of shared file read
            void sharedFileHeaderRead(BlackMisc::Network::CEntityFlags::Entity entity, const QString &fileName, bool success);

        protected:
            CDatabaseReaderConfigList   m_config;                    //!< DB reder configuration
            QString                     m_statusMessage;             //!< Returned status message from watchdog
            bool                        m_1stReplyReceived = false;  //!< Successful connection? Does not mean data / authorizations are correct
            mutable QReadWriteLock      m_statusLock;                //!< Lock
            QNetworkReply::NetworkError m_1stReplyStatus = QNetworkReply::UnknownServerError;     //!< Successful connection?
            QMap<BlackMisc::Network::CEntityFlags::Entity, HeaderResponse> m_sharedFileResponses; //!< file responses of the shared files

            //! Constructor
            CDatabaseReader(QObject *owner, const CDatabaseReaderConfigList &config, const QString &name);

            //! Check if terminated or error, otherwise split into array of objects
            CDatabaseReader::JsonDatastoreResponse setStatusAndTransformReplyIntoDatastoreResponse(QNetworkReply *nwReply);

            //! Info list (latest data timestamp)
            //! \sa BlackCore::Db::CInfoDataReader
            BlackMisc::Db::CDbInfoList infoList() const;

            //! Config for given entity
            CDatabaseReaderConfig getConfigForEntity(BlackMisc::Network::CEntityFlags::Entity entity) const;

            //! Split into single entity and send dataRead signal
            BlackMisc::Network::CEntityFlags::Entity emitReadSignalPerSingleCachedEntity(BlackMisc::Network::CEntityFlags::Entity cachedEntities, bool onlyIfHasData);

            //! Emit signal and log when data have been read
            void emitAndLogDataRead(BlackMisc::Network::CEntityFlags::Entity entity, int number, const JsonDatastoreResponse &res);

            //! Get the service URL, individual for each reader
            virtual BlackMisc::Network::CUrl getDbServiceBaseUrl() const = 0;

            //! Base URL
            BlackMisc::Network::CUrl getBaseUrl(BlackMisc::Db::CDbFlags::DataRetrievalModeFlag mode) const;

            //! DB base URL
            static const BlackMisc::Network::CUrl &getDbUrl();

            //! Get the working shared URL, initialized by CDatabaseReader::initWorkingUrls
            //! \remark normally constant after startup phase
            static BlackMisc::Network::CUrl getWorkingDbDataFileLocationUrl();

            //! File name for given mode, either php service or shared file name
            static QString fileNameForMode(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Db::CDbFlags::DataRetrievalModeFlag mode);

            //! \name Cache access
            //! @{
            //! Synchronize caches for given entities
            virtual void synchronizeCaches(BlackMisc::Network::CEntityFlags::Entity entities) = 0;

            //! Admit caches for given entities
            virtual void admitCaches(BlackMisc::Network::CEntityFlags::Entity entities) = 0;

            //! Invalidate the caches for given entities
            virtual void invalidateCaches(BlackMisc::Network::CEntityFlags::Entity entities) = 0;

            //! Changed URL, means the cache values have been read from elsewhere
            virtual bool hasChangedUrl(BlackMisc::Network::CEntityFlags::Entity entity) const = 0;

            //! Cache for given entity has changed
            virtual void cacheHasChanged(BlackMisc::Network::CEntityFlags::Entity entities);

            //! Has URL been changed? Means we load from a differrent server
            static bool isChangedUrl(const BlackMisc::Network::CUrl &oldUrl, const BlackMisc::Network::CUrl &currentUrl);
            //! @}

        private:
            static BlackMisc::Network::CUrl s_workingSharedDbData; //!< one chhosen URL for all DB reader objects

            //! Start reading in own thread (without config/caching)
            //! \remarks can handle DB or shared file reads
            void startReadFromBackendInBackgroundThread(BlackMisc::Network::CEntityFlags::Entity entities, BlackMisc::Db::CDbFlags::DataRetrievalModeFlag mode, const QDateTime &newerThan = QDateTime());

            //! Received a reply of a header for a shared file
            void receivedSharedFileHeader(QNetworkReply *nwReplyPtr);

            //! Received a reply of a header for a shared file
            void receivedSharedFileHeaderNonClosing(QNetworkReply *nwReply);

            //! Check if terminated or error, otherwise split into array of objects
            JsonDatastoreResponse transformReplyIntoDatastoreResponse(QNetworkReply *nwReply) const;

            //! Check if terminated or error, otherwise set header information
            HeaderResponse transformReplyIntoHeaderResponse(QNetworkReply *nwReply) const;

            //! Set the header part
            bool setHeaderInfoPart(HeaderResponse &headerResponse, QNetworkReply *nwReply) const;

            //! Feedback about connection status
            //! \threadsafe
            void setReplyStatus(QNetworkReply::NetworkError status, const QString &message = "");

            //! Feedback about connection status
            //! \threadsafe
            void setReplyStatus(QNetworkReply *nwReply);
        };
    } // ns
} // ns

#endif // guard
