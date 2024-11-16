// SPDX-FileCopyrightText: Copyright (C) 2023 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/vatsim/vatsimserverfilereader.h"

#include <QMetaObject>
#include <QNetworkReply>
#include <QPointer>
#include <QScopedPointerDeleteLater>
#include <QStringBuilder>
#include <QTimer>
#include <QUrl>
#include <QtGlobal>

#include "core/application.h"
#include "misc/logmessage.h"
#include "misc/network/entityflags.h"
#include "misc/network/server.h"
#include "misc/network/user.h"
#include "misc/pq/units.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::geo;
using namespace swift::misc::simulation;
using namespace swift::misc::physical_quantities;
using namespace swift::core::data;

namespace swift::core::vatsim
{
    CVatsimServerFileReader::CVatsimServerFileReader(QObject *owner) : CThreadedReader(owner, "CVatsimServerFileReader")
    {}

    CServerList CVatsimServerFileReader::getFsdServers() const { return m_lastGoodSetup.get().getFsdServers(); }

    void CVatsimServerFileReader::readInBackgroundThread()
    {
        QPointer<CVatsimServerFileReader> myself(this);
        QTimer::singleShot(0, this, [=] {
            if (!myself) { return; }
            myself->read();
        });
    }

    void CVatsimServerFileReader::doWorkImpl() { this->read(); }

    void CVatsimServerFileReader::read()
    {
        this->threadAssertCheck();
        if (!this->doWorkCheck()) { return; }

        Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing application");
        const QUrl url = sApp->getVatsimServerFileUrl();
        if (url.isEmpty()) { return; }
        this->getFromNetworkAndLog(url, { this, &CVatsimServerFileReader::parseVatsimFile });
    }

    void CVatsimServerFileReader::parseVatsimFile(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        this->threadAssertCheck();

        // Worker thread, make sure to write only synced here!
        if (!this->doWorkCheck())
        {
            CLogMessage(this).info(u"Terminated VATSIM file parsing process");
            return; // stop, terminate straight away, ending thread
        }

        this->logNetworkReplyReceived(nwReplyPtr);

        const QUrl url = nwReply->url();
        const QString urlString = url.toString();

        if (nwReply->error() == QNetworkReply::NoError)
        {
            const QString dataFileData = nwReply->readAll();
            nwReply->close(); // close asap

            if (dataFileData.isEmpty()) { return; }
            if (!this->didContentChange(dataFileData)) // Quick check by hash
            {
                CLogMessage(this).info(u"VATSIM file '%1' has same content, skipped") << urlString;
                return;
            }
            auto jsonDoc = QJsonDocument::fromJson(dataFileData.toUtf8());
            if (jsonDoc.isEmpty()) { return; }

            // build on local vars for thread safety
            CServerList fsdServers;

            for (QJsonValueRef server : jsonDoc.array())
            {
                if (!this->doWorkCheck())
                {
                    CLogMessage(this).info(u"Terminated VATSIM file parsing process");
                    return;
                }
                fsdServers.push_back(parseServer(server.toObject()));
                if (!fsdServers.back().hasName()) { fsdServers.pop_back(); }
            }

            // Setup for VATSIM servers and sorting for comparison
            fsdServers.sortBy(&CServer::getName, &CServer::getDescription);

            // update cache itself is thread safe
            CVatsimSetup vs(m_lastGoodSetup.get());
            const bool changedSetup = vs.setServers(fsdServers, {});
            if (changedSetup)
            {
                vs.setCurrentUtcTime();
                m_lastGoodSetup.set(vs);
            }

            // data read finished
            emit this->dataFileRead(dataFileData.size());
            emit this->dataRead(CEntityFlags::VatsimDataFile, CEntityFlags::ReadFinished, dataFileData.size() / 1000,
                                url);
        }
        else
        {
            // network error
            CLogMessage(this).warning(u"Reading VATSIM data file failed '%1' '%2'")
                << nwReply->errorString() << urlString;
            nwReply->abort();
            emit this->dataRead(CEntityFlags::VatsimDataFile, CEntityFlags::ReadFailed, 0, url);
        }
    }

    CServer CVatsimServerFileReader::parseServer(const QJsonObject &server) const
    {
        return CServer(server["name"].toString(), server["location"].toString(), server["hostname_or_ip"].toString(),
                       6809, CUser("id", "real name", "email", "password"), CFsdSetup::vatsimStandard(),
                       CEcosystem::VATSIM, CServer::FSDServerVatsim, server["clients_connection_allowed"].toInt());
    }

} // namespace swift::core::vatsim
