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

using namespace BlackMisc;

namespace BlackCore
{
    CDatabaseReader::CDatabaseReader(QObject *owner, const QString &name) :
        BlackMisc::CThreadedReader(owner, name)
    { }

    void CDatabaseReader::readInBackgroundThread()
    {
        bool s = QMetaObject::invokeMethod(this, "ps_read");
        Q_ASSERT_X(s, Q_FUNC_INFO, "Invoke failed");
        Q_UNUSED(s);
    }

    QJsonArray CDatabaseReader::transformReplyIntoJsonArray(QNetworkReply *nwReply) const
    {
        this->threadAssertCheck();
        QJsonArray array;
        if (this->isFinished())
        {
            CLogMessage(this).debug() << Q_FUNC_INFO;
            CLogMessage(this).info("Terminated data parsing process"); // for users
            return array; // stop, terminate straight away, ending thread
        }

        if (nwReply->error() == QNetworkReply::NoError)
        {
            const QString dataFileData = nwReply->readAll();
            nwReply->close(); // close asap
            if (dataFileData.isEmpty()) { return array; }

            QJsonDocument jsonResponse = QJsonDocument::fromJson(dataFileData.toUtf8());
            QJsonObject jsonObject = jsonResponse.object();
            QJsonArray jsonArray = jsonObject["rows"].toArray();
            return jsonArray;
        }
        CLogMessage(this).warning("Reading data failed %1 %2") << nwReply->errorString() << nwReply->url().toString();
        nwReply->abort();
        return array;
    }

    QString CDatabaseReader::buildUrl(const QString &protocol, const QString &server, const QString &baseUrl, const QString &serviceUrl)
    {
        Q_ASSERT_X(protocol.length() > 3, Q_FUNC_INFO, "worng protocol");
        Q_ASSERT_X(!server.isEmpty(), Q_FUNC_INFO, "missing server");
        Q_ASSERT_X(!serviceUrl.isEmpty(), Q_FUNC_INFO, "missing service URL");

        QString url(server);
        if (!baseUrl.isEmpty())
        {
            url.append("/").append(baseUrl);
        }
        url.append("/").append(serviceUrl);
        url.replace("//", "/");
        return protocol + "://" + url;
    }

} // namespace
