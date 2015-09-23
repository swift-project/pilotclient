/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "databasereader.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/datastoreutility.h"
#include <QJsonDocument>

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackCore
{
    CDatabaseReader::CDatabaseReader(QObject *owner, const QString &name) :
        BlackMisc::CThreadedReader(owner, name)
    { }

    void CDatabaseReader::readInBackgroundThread(CDbFlags::Entity entities)
    {
        if (m_shutdown) { return; }
        bool s = QMetaObject::invokeMethod(this, "ps_read", Q_ARG(BlackMisc::Network::CDbFlags::Entity, entities));
        Q_ASSERT_X(s, Q_FUNC_INFO, "Invoke failed");
        Q_UNUSED(s);
    }

    CDatabaseReader::JsonDatastoreResponse CDatabaseReader::transformReplyIntoDatastoreResponse(QNetworkReply *nwReply) const
    {
        this->threadAssertCheck();
        JsonDatastoreResponse datastoreResponse;
        if (m_shutdown || this->isFinished())
        {
            CLogMessage(this).debug() << Q_FUNC_INFO;
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

    bool CDatabaseReader::canConnect() const
    {
        QString m;
        return canConnect(m);
    }

} // namespace
