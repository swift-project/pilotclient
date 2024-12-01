// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_DB_DATABASEREADER_H
#define SWIFT_CORE_DB_DATABASEREADER_H

#include <QDateTime>
#include <QJsonArray>
#include <QMap>
#include <QNetworkReply>
#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QtGlobal>

#include "core/db/databasereaderconfig.h"
#include "core/swiftcoreexport.h"
#include "core/threadedreader.h"
#include "misc/db/dbinfolist.h"
#include "misc/network/url.h"
#include "misc/pq/time.h"
#include "misc/statusmessage.h"
#include "misc/valueobject.h"

class QNetworkReply;
class QFileInfo;

namespace swift::misc
{
    class CLogCategoryList;
}
namespace swift::core::db
{
    //! Specialized version of threaded reader for DB data
    class SWIFT_CORE_EXPORT CDatabaseReader : public CThreadedReader
    {
        Q_OBJECT

    public:
        //! Header response part
        struct HeaderResponse
        {
        private:
            QDateTime m_lastModified; //!< when was the latest update?
            int m_httpStatusCode = -1; //!< HTTP status code
            qulonglong m_contentLengthHeader = 0; //!< content length
            qint64 m_requestStarted = -1; //!< when was request started
            qint64 m_responseReceived = -1; //!< response received
            qint64 m_loadTimeMs = -1; //!< how long did it take to load
            swift::misc::CStatusMessage m_message; //!< last error or warning
            swift::misc::network::CUrl m_url; //!< loaded URL

        public:
            //! Any timestamp?
            bool hasTimestamp() const { return m_lastModified.isValid(); }

            //! Is response newer?
            bool isNewer(const QDateTime &ts) const
            {
                return m_lastModified.toMSecsSinceEpoch() > ts.toMSecsSinceEpoch();
            }

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
            bool hasErrorMessage() const
            {
                return m_message.getSeverity() == swift::misc::CStatusMessage::SeverityError;
            }

            //! Warning or error message?
            bool hasWarningOrAboveMessage() const { return m_message.isWarningOrAbove(); }

            //! Last error or warning
            const swift::misc::CStatusMessage &lastWarningOrAbove() const { return m_message; }

            //! Set the error/warning message
            void setMessage(const swift::misc::CStatusMessage &lastErrorOrWarning) { m_message = lastErrorOrWarning; }

            //! URL loaded
            const swift::misc::network::CUrl &getUrl() const { return m_url; }

            //! URL loaded as string
            QString getUrlString() const { return m_url.toQString(); }

            //! Set the loaded URL
            void setUrl(const swift::misc::network::CUrl &url) { m_url = url; }

            //! Is a shared file?
            bool isSharedFile() const;

            //! Has HTTP status code?
            bool hasHttpStatusCode() const { return m_httpStatusCode >= 0; }

            //! HTTP status code
            int getHttpStatusCode() const { return m_httpStatusCode; }

            //! Load time in ms (from request to response)
            qint64 getLoadTimeMs() const { return m_loadTimeMs; }

            //! Load time as string
            QString getLoadTimeString() const;

            //! Load time as string
            QString getLoadTimeStringWithStartedHint() const;

            //! Set the load time (delta start -> response received)
            void setLoadTimeMs(qint64 deltaTime) { m_loadTimeMs = deltaTime; }

            //! Set reply values
            void setValues(const QNetworkReply *nwReply);
        };

        //!  Response from our database (depending on JSON DB backend generates)
        struct JsonDatastoreResponse : public HeaderResponse
        {
        private:
            QJsonArray m_jsonArray; //!< JSON array data
            int m_arraySize = -1; //!< size of array, if applicable (copied to member for debugging purposes)
            int m_stringSize = 0; //!< string size of JSON data
            bool m_restricted = false; //!< restricted reponse, only changed data

        public:
            //! Any data?
            bool isEmpty() const { return m_jsonArray.isEmpty(); }

            //! Is loaded from database
            bool isLoadedFromDb() const;

            //! Incremental data, restricted by query?
            bool isRestricted() const { return m_restricted; }

            //! Mark as restricted
            void setRestricted(bool restricted) { m_restricted = restricted; }

            //! Get the JSON array
            QJsonArray getJsonArray() const { return m_jsonArray; }

            //! Number of elements
            int getArraySize() const { return m_jsonArray.size(); }

            //! Set the JSON array
            void setJsonArray(const QJsonArray &value);

            //! Set string size
            void setStringSize(int size) { m_stringSize = size; }

            //! String info
            QString toQString() const;

            //! Implicit conversion
            operator QJsonArray() const { return m_jsonArray; }
        };

        //! Start reading in own thread
        //! \remark uses caches, info objects
        void readInBackgroundThread(swift::misc::network::CEntityFlags::Entity entities, const QDateTime &newerThan);

        //! Start loading from DB in own thread
        //! \remark bypass caches/config
        swift::misc::network::CEntityFlags::Entity
        triggerLoadingDirectlyFromDb(swift::misc::network::CEntityFlags::Entity entities, const QDateTime &newerThan);

        //! Start loading from shared files in own thread
        //! \remark bypass caches/config
        swift::misc::network::CEntityFlags::Entity
        triggerLoadingDirectlyFromSharedFiles(swift::misc::network::CEntityFlags::Entity entities,
                                              bool checkCacheTsUpfront);

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
        virtual swift::misc::network::CEntityFlags::Entity getSupportedEntities() const = 0;

        //! Supported entities as string
        QString getSupportedEntitiesAsString() const;

        //! Mask by supported entities
        swift::misc::network::CEntityFlags::Entity
        maskBySupportedEntities(swift::misc::network::CEntityFlags::Entity entities) const;

        //! Is any of the given entities supported here by this reader
        bool supportsAnyOfEntities(swift::misc::network::CEntityFlags::Entity entities) const;

        //! Get cache timestamp
        virtual QDateTime getCacheTimestamp(swift::misc::network::CEntityFlags::Entity entity) const = 0;

        //! Has entity a valid and newer timestamp
        bool hasCacheTimestampNewerThan(swift::misc::network::CEntityFlags::Entity entity,
                                        const QDateTime &threshold) const;

        //! Cache`s number of entities
        //! \remark this only works if the cache is admitted, DB caches are read deferred
        virtual int getCacheCount(swift::misc::network::CEntityFlags::Entity entity) const = 0;

        //! Entities already having data in cache
        //! \remark this only works if the cache is admitted, DB caches are read deferred
        virtual swift::misc::network::CEntityFlags::Entity getEntitiesWithCacheCount() const = 0;

        //! Entities already having data in cache (based on timestamp assumption)
        //! \remark unlike getEntitiesWithCacheCount() this even works when the cache is not yet admitted
        virtual swift::misc::network::CEntityFlags::Entity
        getEntitiesWithCacheTimestampNewerThan(const QDateTime &threshold) const = 0;

        //! DB info objects available?
        bool hasDbInfoObjects() const;

        //! Shared info objects available?
        bool hasSharedInfoObjects() const;

        //! Header of shared file read (for single entity)?
        bool hasSharedFileHeader(const swift::misc::network::CEntityFlags::Entity entity) const;

        //! Headers of shared file read (for single entity)?
        bool hasSharedFileHeaders(const swift::misc::network::CEntityFlags::Entity entities) const;

        //! Obtain latest object timestamp from DB info objects
        //! \sa swift::core::db::CInfoDataReader
        QDateTime getLatestEntityTimestampFromDbInfoObjects(swift::misc::network::CEntityFlags::Entity entity) const;

        //! Obtain latest object timestamp from shared info objects
        //! \sa swift::core::db::CInfoDataReader
        QDateTime
        getLatestEntityTimestampFromSharedInfoObjects(swift::misc::network::CEntityFlags::Entity entity) const;

        //! Header timestamp (last-modified) for shared file
        //! \deprecated use getLatestEntityTimestampFromSharedInfoObjects
        QDateTime getLatestSharedFileHeaderTimestamp(swift::misc::network::CEntityFlags::Entity entity) const;

        //! Is the file timestamp newer than cache timestamp?
        //! \deprecated use isSharedInfoNewerThanCacheTimestamp
        bool isSharedHeaderNewerThanCacheTimestamp(swift::misc::network::CEntityFlags::Entity entity) const;

        //! Is the shared info timestamp newer than cache timestamp?
        bool isSharedInfoObjectNewerThanCacheTimestamp(swift::misc::network::CEntityFlags::Entity entity) const;

        //! Those entities where the timestamp of the header is newer than the cache timestamp
        swift::misc::network::CEntityFlags::Entity
        getEntitesWithNewerHeaderTimestamp(swift::misc::network::CEntityFlags::Entity entities) const;

        //! Those entities where the timestamp of a shared info object is newer than the cache timestamp
        swift::misc::network::CEntityFlags::Entity
        getEntitesWithNewerSharedInfoObject(swift::misc::network::CEntityFlags::Entity entities) const;

        //! Status message (error message)
        const QString &getStatusMessage() const;

        //! Severity used for log messages in case of no URLs
        void setSeverityNoWorkingUrl(swift::misc::CStatusMessage::StatusSeverity s) { m_severityNoWorkingUrl = s; }

        //! Init from local resource file
        //! \remark normally used after installation for a 1st time init
        swift::misc::CStatusMessageList initFromLocalResourceFiles(bool inBackground);

        //! Init from local resource file
        //! \remark normally used after installation for a 1st time init
        swift::misc::CStatusMessageList initFromLocalResourceFiles(swift::misc::network::CEntityFlags::Entity entities,
                                                                   bool inBackground);

        //! Data read from local data
        virtual swift::misc::CStatusMessageList readFromJsonFiles(const QString &dir,
                                                                  swift::misc::network::CEntityFlags::Entity whatToRead,
                                                                  bool overrideNewer) = 0;

        //! Data read from local data
        virtual bool readFromJsonFilesInBackground(const QString &dir,
                                                   swift::misc::network::CEntityFlags::Entity whatToRead,
                                                   bool overrideNewer) = 0;

        //! Log categories
        static const QStringList &getLogCategories();

        //! Transform JSON data to response struct data
        //! \private used also for samples, that`s why it is declared public
        static void stringToDatastoreResponse(const QString &jsonContent,
                                              CDatabaseReader::JsonDatastoreResponse &datastoreResponse);

    signals:
        //! DB have been read
        void swiftDbDataRead(bool success);

        //! Combined read signal
        //! \remark normally in success case state for a single case, skipped cases can be reported for 1..n enities
        void dataRead(swift::misc::network::CEntityFlags::Entity entities,
                      swift::misc::network::CEntityFlags::ReadState state, int number, const QUrl &url);

        //! Header of shared file read
        void sharedFileHeaderRead(swift::misc::network::CEntityFlags::Entity entity, const QString &fileName,
                                  bool success);

        //! Database reader messages
        //! \remark used with splash screen
        void databaseReaderMessages(const swift::misc::CStatusMessageList &messages);

        //! Download progress for an entity
        void entityDownloadProgress(swift::misc::network::CEntityFlags::Entity entity, int logId, int progress,
                                    qint64 current, qint64 max, const QUrl &url);

    protected:
        CDatabaseReaderConfigList m_config; //!< DB reder configuration
        QString m_statusMessage; //!< Returned status message from watchdog
        bool m_1stReplyReceived = false; //!< Successful connection? Does not mean data / authorizations are correct
        mutable QReadWriteLock m_statusLock; //!< Lock
        QNetworkReply::NetworkError m_1stReplyStatus = QNetworkReply::UnknownServerError; //!< Successful connection?
        QMap<swift::misc::network::CEntityFlags::Entity, HeaderResponse>
            m_sharedFileResponses; //!< file responses of the shared files
        swift::misc::CStatusMessage::StatusSeverity m_severityNoWorkingUrl =
            swift::misc::CStatusMessage::SeverityWarning; //!< severity of message if there is no working URL

        //! Constructor
        CDatabaseReader(QObject *owner, const CDatabaseReaderConfigList &config, const QString &name);

        //! Check if terminated or error, otherwise split into array of objects
        CDatabaseReader::JsonDatastoreResponse setStatusAndTransformReplyIntoDatastoreResponse(QNetworkReply *nwReply);

        //! DB Info list (latest data timestamps from DB web service)
        //! \sa swift::core::db::CInfoDataReader
        swift::misc::db::CDbInfoList getDbInfoObjects() const;

        //! Shared info list (latest data timestamps from DB web service)
        //! \sa swift::core::db::CInfoDataReader
        swift::misc::db::CDbInfoList getSharedInfoObjects() const;

        //! Config for given entity
        CDatabaseReaderConfig getConfigForEntity(swift::misc::network::CEntityFlags::Entity entity) const;

        //! Split into single entity and send dataRead signal
        swift::misc::network::CEntityFlags::Entity
        emitReadSignalPerSingleCachedEntity(swift::misc::network::CEntityFlags::Entity cachedEntities,
                                            bool onlyIfHasData);

        //! Emit signal and log when data have been read
        void emitAndLogDataRead(swift::misc::network::CEntityFlags::Entity entity, int number,
                                const JsonDatastoreResponse &res);

        //! Get the service URL, individual for each reader
        virtual swift::misc::network::CUrl getDbServiceBaseUrl() const = 0;

        //! Log if no working URL exists, using m_noWorkingUrlSeverity
        void logNoWorkingUrl(swift::misc::network::CEntityFlags::Entity entity);

        //! Base URL for mode (either a shared or DB URL)
        swift::misc::network::CUrl getBaseUrl(swift::misc::db::CDbFlags::DataRetrievalModeFlag mode) const;

        //! DB base URL
        static const swift::misc::network::CUrl &getDbUrl();

        //! File name for given mode, either php service or shared file name
        static QString fileNameForMode(swift::misc::network::CEntityFlags::Entity entity,
                                       swift::misc::db::CDbFlags::DataRetrievalModeFlag mode);

        //! Name of latest timestamp
        static const QString &parameterLatestTimestamp();

        //! A newer than value understood by swift DB
        //! \sa CDatabaseReader::parameterLatestTimestamp
        static QString dateTimeToDbLatestTs(const QDateTime &ts);

        //! Latest timestamp query for DB
        static QString queryLatestTimestamp(const QDateTime &ts);

        //! \name Cache access
        //! @{
        //! Synchronize caches for given entities
        virtual void synchronizeCaches(swift::misc::network::CEntityFlags::Entity entities) = 0;

        //! Admit caches for given entities
        virtual void admitCaches(swift::misc::network::CEntityFlags::Entity entities) = 0;

        //! Invalidate the caches for given entities
        virtual void invalidateCaches(swift::misc::network::CEntityFlags::Entity entities) = 0;

        //! Changed URL, means the cache values have been read from elsewhere
        //! \remark testing based on swift::misc::db::CDbFlags::DbReading
        virtual bool hasChangedUrl(swift::misc::network::CEntityFlags::Entity entity,
                                   swift::misc::network::CUrl &oldUrlInfo,
                                   swift::misc::network::CUrl &newUrlInfo) const = 0;

        //! Cache for given entity has changed
        virtual void cacheHasChanged(swift::misc::network::CEntityFlags::Entity entities);

        //! Has URL been changed? Means we load from a different server
        static bool isChangedUrl(const swift::misc::network::CUrl &oldUrl,
                                 const swift::misc::network::CUrl &currentUrl);
        //! @}

        //! Start reading in own thread (without config/caching)
        //! \remarks can handle DB or shared file reads
        void startReadFromBackendInBackgroundThread(swift::misc::network::CEntityFlags::Entity entities,
                                                    swift::misc::db::CDbFlags::DataRetrievalModeFlag mode,
                                                    const QDateTime &newerThan = QDateTime());

        //! Received a reply of a header for a shared file
        void receivedSharedFileHeader(QNetworkReply *nwReplyPtr);

        //! Received a reply of a header for a shared file
        void receivedSharedFileHeaderNonClosing(QNetworkReply *nwReplyPtr);

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

        //! Override cache from file
        //! \threadsafe
        bool overrideCacheFromFile(bool overrideNewerOnly, const QFileInfo &fileInfo,
                                   swift::misc::network::CEntityFlags::Entity entity,
                                   swift::misc::CStatusMessageList &msgs) const;

        //! Parsing info message
        void logParseMessage(const QString &entity, int size, int msElapsed,
                             const JsonDatastoreResponse &response) const;

        //! Network request progress
        virtual void networkReplyProgress(int logId, qint64 current, qint64 max, const QUrl &url) override;

    private:
        //! Read / re-read data file
        virtual void read(swift::misc::network::CEntityFlags::Entity entities,
                          swift::misc::db::CDbFlags::DataRetrievalModeFlag mode, const QDateTime &newerThan) = 0;
    };
} // namespace swift::core::db

#endif // guard
