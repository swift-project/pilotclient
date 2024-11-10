// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/vatsim/vatsimstatusfilereader.h"
#include "core/application.h"
#include "core/data/globalsetup.h"
#include "misc/logmessage.h"
#include "misc/network/url.h"
#include "misc/statusmessage.h"

#include <QByteArray>
#include <QDateTime>
#include <QList>
#include <QMetaObject>
#include <QNetworkReply>
#include <QScopedPointer>
#include <QScopedPointerDeleteLater>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QUrl>
#include <QtGlobal>

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::misc::physical_quantities;
using namespace swift::core::data;

namespace swift::core::vatsim
{
    CVatsimStatusFileReader::CVatsimStatusFileReader(QObject *owner) : CThreadedReader(owner, "CVatsimStatusFileReader")
    {
        // do not connect with time, will be read once at startup
    }

    void CVatsimStatusFileReader::readInBackgroundThread()
    {
        QPointer<CVatsimStatusFileReader> myself(this);
        QTimer::singleShot(0, this, [=] {
            if (!myself) { return; }
            myself->read();
        });
    }

    CUrl CVatsimStatusFileReader::getMetarFileUrl() const
    {
        return m_lastGoodSetup.get().getMetarFileUrl();
    }

    CUrl CVatsimStatusFileReader::getDataFileUrl() const
    {
        return m_lastGoodSetup.get().getDataFileUrl();
    }

    void CVatsimStatusFileReader::read()
    {
        this->threadAssertCheck();
        if (!this->doWorkCheck()) { return; }

        Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing application");
        const CUrl url(sApp->getGlobalSetup().getVatsimStatusFileUrl());
        if (url.isEmpty()) { return; }
        CLogMessage(this).info(u"Trigger read of VATSIM status file from '%1'") << url.toQString(true);
        this->getFromNetworkAndLog(url, { this, &CVatsimStatusFileReader::parseVatsimFile });
    }

    void CVatsimStatusFileReader::parseVatsimFile(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);

        // Worker thread, make sure to write only synced here!
        this->threadAssertCheck();
        if (!this->doWorkCheck())
        {
            CLogMessage(this).debug() << Q_FUNC_INFO;
            CLogMessage(this).info(u"Terminated VATSIM status file parsing process"); // for users
            return; // stop, terminate straight away, ending thread
        }

        this->logNetworkReplyReceived(nwReplyPtr);
        const QString urlString = nwReply->url().toString();

        if (nwReply->error() == QNetworkReply::NoError)
        {
            const QString statusFileData = nwReply->readAll();
            nwReply->close(); // close asap

            if (statusFileData.isEmpty()) return;
            auto jsonDoc = QJsonDocument::fromJson(statusFileData.toUtf8());
            if (jsonDoc.isEmpty()) { return; }

            CUrl dataFileUrl;
            CUrl serverFileUrl;
            CUrl metarFileUrl;

            // Always taking the first URL from the file
            // (at the time of writing, also only one URL per service is available anyway)
            if (const QJsonArray dataUrls = jsonDoc["data"]["v3"].toArray(); !dataUrls.empty())
            {
                dataFileUrl = QUrl(dataUrls.at(0).toString());
            }

            if (const QJsonArray serverFileUrls = jsonDoc["data"]["servers"].toArray(); !serverFileUrls.empty())
            {
                serverFileUrl = QUrl(serverFileUrls.at(0).toString());
            }

            if (const QJsonArray metarUrls = jsonDoc["metar"].toArray(); !metarUrls.empty())
            {
                metarFileUrl = QUrl(metarUrls.at(0).toString());
            }

            // cache itself is thread safe, avoid writing with unchanged data
            CVatsimSetup vs(m_lastGoodSetup.get());
            const bool changed = vs.setUrls(dataFileUrl, serverFileUrl, metarFileUrl);
            vs.setUtcTimestamp(QDateTime::currentDateTime());
            const CStatusMessage cacheMsg = m_lastGoodSetup.set(vs);
            if (cacheMsg.isFailure()) { CLogMessage::preformatted(cacheMsg); }
            else
            {
                CLogMessage(this).info(u"Read VATSIM status file from '%1'")
                    << urlString;
            }
            Q_UNUSED(changed);

            // data read finished
            emit this->statusFileRead(statusFileData.count());
            emit this->dataRead(CEntityFlags::VatsimStatusFile, CEntityFlags::ReadFinished, statusFileData.count());
        }
        else
        {
            // network error
            CLogMessage(this).warning(u"Reading VATSIM status file failed '%1' '%2'") << nwReply->errorString() << urlString;
            nwReply->abort();
            emit this->dataRead(CEntityFlags::VatsimStatusFile, CEntityFlags::ReadFailed, 0);
        }
    }
} // ns
