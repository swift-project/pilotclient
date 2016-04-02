/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORE_DATABASE_READER_H
#define BLACKCORE_DATABASE_READER_H

//! \file

#include "blackcore/blackcoreexport.h"
#include "blackmisc/threadedreader.h"
#include "blackmisc/network/entityflags.h"
#include <QNetworkReply>
#include <QJsonArray>
#include <QDateTime>
#include <QTimer>

namespace BlackCore
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

        //! Log categories
        static const BlackMisc::CLogCategoryList &getLogCategories();

        //! Start reading in own thread
        void readInBackgroundThread(BlackMisc::Network::CEntityFlags::Entity entities, const QDateTime &newerThan);

        //! Can connect to DB
        //! \threadsafe
        bool canConnect() const;

        //! Can connect to server?
        //! \return message why connect failed
        //! \threadsafe
        bool canConnect(QString &message) const;

    protected:
        BlackMisc::Network::CUrl m_watchdogUrl;              //!< URL for checking if alive
        QTimer                   m_watchdogTimer { this };   //!< Timer for watchdog (DB available?)
        QString                  m_watchdogMessage;          //!< Returned status message from watchdog
        bool                     m_canConnect = false;       //!< Successful connection?
        mutable QReadWriteLock   m_watchdogLock;             //!< Lock

        //! Constructor
        CDatabaseReader(QObject *owner, const QString &name);

        //! Watchdog URL, empty means no checking
        //! \threadsafe
        void setWatchdogUrl(const BlackMisc::Network::CUrl &url);

        //! Check if terminated or error, otherwise split into array of objects
        CDatabaseReader::JsonDatastoreResponse setStatusAndTransformReplyIntoDatastoreResponse(QNetworkReply *nwReply);

    private slots:
        //! Watchdog checking if DB is available
        void ps_watchdog();

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
} // namespace

#endif // guard
