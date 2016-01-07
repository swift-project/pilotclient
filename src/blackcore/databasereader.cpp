/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "databasereader.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/datastoreutility.h"
#include <QJsonDocument>

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackCore
{
    CDatabaseReader::CDatabaseReader(QObject *owner, const QString &name) :
        BlackMisc::CThreadedReader(owner, name)
    {
        connect(&m_watchdogTimer, &QTimer::timeout, this, &CDatabaseReader::ps_watchdog);
    }

    void CDatabaseReader::readInBackgroundThread(CEntityFlags::Entity entities, const QDateTime &newerThan)
    {
        if (isAbandoned()) { return; }
        this->m_watchdogTimer.stop();
        // ps_read is implemented in the derived classes
        bool s = QMetaObject::invokeMethod(this, "ps_read",
                                           Q_ARG(BlackMisc::Network::CEntityFlags::Entity, entities),
                                           Q_ARG(QDateTime, newerThan));
        Q_ASSERT_X(s, Q_FUNC_INFO, "Invoke failed");
        Q_UNUSED(s);
    }

    CDatabaseReader::JsonDatastoreResponse CDatabaseReader::transformReplyIntoDatastoreResponse(QNetworkReply *nwReply) const
    {
        this->threadAssertCheck();
        JsonDatastoreResponse datastoreResponse;
        if (this->isAbandoned())
        {
            CLogMessage(this).info("Terminated data parsing process"); // for users
            nwReply->abort();
            return datastoreResponse; // stop, terminate straight away, ending thread
        }

        if (nwReply->error() == QNetworkReply::NoError)
        {
            const QString dataFileData = nwReply->readAll().trimmed();
            nwReply->close(); // close asap
            if (dataFileData.isEmpty()) { datastoreResponse.updated = QDateTime::currentDateTimeUtc(); return datastoreResponse; }

            QJsonDocument jsonResponse = QJsonDocument::fromJson(dataFileData.toUtf8());
            if (jsonResponse.isArray())
            {
                // directly an array, no further info
                datastoreResponse.jsonArray = jsonResponse.array();
                datastoreResponse.updated = QDateTime::currentDateTimeUtc();
            }
            else
            {
                QJsonObject responseObject(jsonResponse.object());
                datastoreResponse.jsonArray = responseObject["data"].toArray();
                QString ts(responseObject["latest"].toString());
                datastoreResponse.updated = ts.isEmpty() ? QDateTime::currentDateTimeUtc() : CDatastoreUtility::parseTimestamp(ts);
                datastoreResponse.restricted = responseObject["restricted"].toBool();
            }
            return datastoreResponse;
        }

        // no valid response
        QString error(nwReply->errorString());
        QString url(nwReply->url().toString());
        CLogMessage(this).warning("Reading data failed %1 %2") << error << url;
        nwReply->abort();
        return datastoreResponse;
    }

    CDatabaseReader::JsonDatastoreResponse CDatabaseReader::setStatusAndTransformReplyIntoDatastoreResponse(QNetworkReply *nwReply)
    {
        setConnectionStatus(nwReply);
        return transformReplyIntoDatastoreResponse(nwReply);
    }

    void CDatabaseReader::setWatchdogUrl(const CUrl &url)
    {
        bool start;
        {
            QWriteLocker wl(&this->m_watchdogLock);
            m_watchdogUrl = url;
            start = url.isEmpty();
        }
        if (start)
        {
            m_watchdogTimer.start(30 * 1000);
        }
        else
        {
            m_watchdogTimer.stop();
        }
    }

    bool CDatabaseReader::canConnect() const
    {
        QReadLocker rl(&this->m_watchdogLock);
        return m_canConnect;
    }

    bool CDatabaseReader::canConnect(QString &message) const
    {
        QReadLocker rl(&this->m_watchdogLock);
        message = m_watchdogMessage;
        return m_canConnect;
    }

    void CDatabaseReader::setConnectionStatus(bool ok, const QString &message)
    {
        {
            QWriteLocker wl(&this->m_watchdogLock);
            this->m_watchdogMessage = message;
            this->m_canConnect = ok;
            if (this->m_watchdogUrl.isEmpty()) { return; }
        }
        this->m_updateTimer->start(); // restart
    }

    void CDatabaseReader::setConnectionStatus(QNetworkReply *nwReply)
    {
        Q_ASSERT_X(nwReply, Q_FUNC_INFO, "Missing network reply");
        if (nwReply->isFinished())
        {
            if (nwReply->error() == QNetworkReply::NoError)
            {
                setConnectionStatus(true);
            }
            else
            {
                setConnectionStatus(false, nwReply->errorString());
            }
        }
    }

    void CDatabaseReader::ps_watchdog()
    {
        CUrl url;
        {
            QReadLocker rl(&this->m_watchdogLock);
            url = this->m_watchdogUrl;
        }
        if (url.isEmpty())
        {
            this->m_watchdogTimer.stop();
            return;
        }
        QString m;
        bool ok = CNetworkUtils::canConnect(url, m);
        this->setConnectionStatus(ok, m);
    }

} // namespace
