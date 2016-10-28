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
            //!  Response from our database
            struct JsonDatastoreResponse
            {
                QJsonArray m_jsonArray;              //!< JSON array data
                QDateTime  m_updated;                //!< when was the latest updated?
                int        m_arraySize  = -1;        //!< size of array, if applicable (copied to member for debugging purposes)
                bool       m_restricted = false;     //!< restricted reponse, only changed data
                BlackMisc::CStatusMessage m_message; //!< last error or warning

                //! Any data?
                bool isEmpty() const { return m_jsonArray.isEmpty(); }

                //! Number of elements
                int size() const { return m_jsonArray.size(); }

                //! Any timestamp?
                bool hasTimestamp() const { return m_updated.isValid(); }

                //! Is response newer?
                bool isNewer(const QDateTime &ts) const { return m_updated.toMSecsSinceEpoch() > ts.toMSecsSinceEpoch(); }

                //! Is response newer?
                bool isNewer(qint64 mSecsSinceEpoch) const { return m_updated.toMSecsSinceEpoch() > mSecsSinceEpoch; }

                //! Incremental data
                bool isRestricted() const { return m_restricted; }

                //! Error message?
                bool hasErrorMessage() const { return m_message.getSeverity() == BlackMisc::CStatusMessage::SeverityError; }

                //! Warning or error message?
                bool hasWarningOrAboveMessage() const { return m_message.isWarningOrAbove(); }

                //! Last error or warning
                const BlackMisc::CStatusMessage &lastWarningOrAbove() const { return m_message; }

                //! Set the error/warning message
                void setMessage(const BlackMisc::CStatusMessage &lastErrorOrWarning) { m_message = lastErrorOrWarning; }

                //! Get the JSON array
                QJsonArray getJsonArray() const { return m_jsonArray; }

                //! Set the JSON array
                void setJsonArray(const QJsonArray &value);

                //! Implicit conversion
                operator QJsonArray() const { return m_jsonArray; }
            };

            //! Start reading in own thread
            void readInBackgroundThread(BlackMisc::Network::CEntityFlags::Entity entities, const QDateTime &newerThan);

            //! Start reading in own thread (without config/caching)
            void startReadFromDbInBackgroundThread(BlackMisc::Network::CEntityFlags::Entity entities, const QDateTime &newerThan);

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

            //! Get cache timestamp
            virtual QDateTime getCacheTimestamp(BlackMisc::Network::CEntityFlags::Entity entities) const = 0;

            //! Cache`s number of entities
            virtual int getCacheCount(BlackMisc::Network::CEntityFlags::Entity entity) const = 0;

            //! Info objects available?
            bool hasInfoObjects() const;

            //! Obtain latest object timestamp from info objects
            //! \sa BlackCore::Db::CInfoDataReader
            QDateTime getLatestEntityTimestampFromInfoObjects(BlackMisc::Network::CEntityFlags::Entity entity) const;

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

            //! sift DB server reachable?
            static bool canPingSwiftServer();

            //! Transform JSON data to response struct
            static JsonDatastoreResponse stringToDatastoreResponse(const QString &jsonContent);

        signals:
            //! Combined read signal
            void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

        protected:
            CDatabaseReaderConfigList   m_config;                      //!< DB reder configuration
            QString                     m_statusMessage;               //!< Returned status message from watchdog
            QNetworkReply::NetworkError m_1stReplyStatus = QNetworkReply::UnknownServerError; //!< Successful connection?
            bool                        m_1stReplyReceived = false;    //!< Successful connection? Does not mean data / authorizations are correct
            mutable QReadWriteLock      m_statusLock;                  //!< Lock

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

            //! DB base URL
            static const BlackMisc::Network::CUrl &getDbUrl();

            //! Obtain a working shared URL
            static BlackMisc::Network::CUrl getWorkingSharedUrl();

            //! \name  Cache access
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
            //! Check if terminated or error, otherwise split into array of objects
            JsonDatastoreResponse transformReplyIntoDatastoreResponse(QNetworkReply *nwReply) const;

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
