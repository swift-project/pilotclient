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
#include "blackmisc/threadedreader.h"
#include "blackmisc/sequence.h"
#include "blackmisc/valueobject.h"

#include <QDateTime>
#include <QJsonArray>
#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QTimer>
#include <QtGlobal>

class QNetworkReply;
namespace BlackMisc { class CLogCategoryList; }

namespace BlackCore
{
    namespace Db
    {
        //! Specialized version of threaded reader for DB data
        class BLACKCORE_EXPORT CDatabaseReader : public BlackMisc::CThreadedReader
        {
            Q_OBJECT

        public:
            //!  Response from our database
            struct JsonDatastoreResponse
            {
                QJsonArray m_jsonArray;              //!< JSON array data
                QDateTime  m_updated;                //!< when was the latest updated?
                bool       m_restricted = false;     //!< restricted reponse, only data changed
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
                void setJsonArray(const QJsonArray &value) { m_jsonArray = value; }

                //! Implicit conversion
                operator QJsonArray() const { return m_jsonArray; }
            };

            //! Start reading in own thread
            void readInBackgroundThread(BlackMisc::Network::CEntityFlags::Entity entities, const QDateTime &newerThan);

            //! Can connect to DB
            //! \threadsafe
            bool canConnect() const;

            //! Can connect to server?
            //! \return message why connect failed
            //! \threadsafe
            bool canConnect(QString &message) const;

            //! Obtain a working shared URL
            BlackMisc::Network::CUrl getWorkingSharedUrl() const;

            //! Status message (error message)
            const QString &getStatusMessage() const;

            //! Log categories
            static const BlackMisc::CLogCategoryList &getLogCategories();

            //! Name of latest timestamp
            static const QString &parameterLatestTimestamp();

            //! Name of parameter for latest id
            static const QString &parameterLatestId();

        protected:
            CDatabaseReaderConfigList m_config;                 //!< DB reder configuration
            BlackMisc::Network::CUrl  m_sharedUrl;              //!< URL for checking if alive
            QString                   m_statusMessage;          //!< Returned status message from watchdog
            bool                      m_canConnect = false;     //!< Successful connection?
            mutable QReadWriteLock    m_statusLock;             //!< Lock

            //! Constructor
            CDatabaseReader(QObject *owner, const CDatabaseReaderConfigList &config, const QString &name);

            //! Check if terminated or error, otherwise split into array of objects
            CDatabaseReader::JsonDatastoreResponse setStatusAndTransformReplyIntoDatastoreResponse(QNetworkReply *nwReply);

            //! Info list (latest data timestamp)
            //! \sa BlackCore::Db::CInfoDataReader
            BlackMisc::Db::CDbInfoList infoList() const;

            //! Info objects available?
            bool hasInfoObjects() const;

            //! Obtain latest object timestamp
            //! \sa BlackCore::Db::CInfoDataReader
            QDateTime getLatestEntityTimestamp(BlackMisc::Network::CEntityFlags::Entity entity) const;

            //! Config for given entity
            CDatabaseReaderConfig getConfigForEntity(BlackMisc::Network::CEntityFlags::Entity entity) const;

            //! Syncronize caches for given entities
            virtual void syncronizeCaches(BlackMisc::Network::CEntityFlags::Entity entities) = 0;

            //! Cache`s timestamp for given entity
            virtual QDateTime getCacheTimestamp(BlackMisc::Network::CEntityFlags::Entity entities) const = 0;

            //! Cache`s number of entities
            virtual int getCacheCount(BlackMisc::Network::CEntityFlags::Entity entity) const = 0;

            //! Invalidate the caches for given entities
            virtual void invalidateCaches(BlackMisc::Network::CEntityFlags::Entity entities) = 0;

            //! Changed URL, means the cache values have been read from elsewhere
            virtual bool hasChangedUrl(BlackMisc::Network::CEntityFlags::Entity entity) const = 0;

            //! Has URL been changed? Means we load from a differrent server
            static bool isChangedUrl(const BlackMisc::Network::CUrl &oldUrl, const BlackMisc::Network::CUrl &currentUrl);

        private:
            //! Check if terminated or error, otherwise split into array of objects
            JsonDatastoreResponse transformReplyIntoDatastoreResponse(QNetworkReply *nwReply) const;

            //! Feedback about connection status
            //! \threadsafe
            void setConnectionStatus(bool ok, const QString &message = "");

            //! Feedback about connection status
            //! \threadsafe
            void setConnectionStatus(QNetworkReply *nwReply);
        };
    } // ns
} // ns

#endif // guard
