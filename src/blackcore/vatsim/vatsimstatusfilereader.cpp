/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/vatsim/vatsimstatusfilereader.h"
#include "blackcore/application.h"
#include "blackcore/data/globalsetup.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/url.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/statusmessage.h"

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

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackCore::Data;

namespace BlackCore::Vatsim
{
    CVatsimStatusFileReader::CVatsimStatusFileReader(QObject *owner) :
        CThreadedReader(owner, "CVatsimStatusFileReader")
    {
        // do not connect with time, will be read once at startup
    }

    void CVatsimStatusFileReader::readInBackgroundThread()
    {
        QPointer<CVatsimStatusFileReader> myself(this);
        QTimer::singleShot(0, this, [ = ]
        {
            if (!myself) { return; }
            myself->read();
        });
    }

    CUrlList CVatsimStatusFileReader::getMetarFileUrls() const
    {
        return m_lastGoodSetup.get().getMetarFileUrls();
    }

    CUrlList CVatsimStatusFileReader::getDataFileUrls() const
    {
        return m_lastGoodSetup.get().getDataFileUrls();
    }

    void CVatsimStatusFileReader::read()
    {
        this->threadAssertCheck();
        if (!this->doWorkCheck()) { return; }
        if (!this->isInternetAccessible("No network/internet access, cannot read VATSIM status file")) { return; }

        Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing application");
        const CUrlList urls(sApp->getGlobalSetup().getVatsimStatusFileUrls());
        const CUrl url = urls.getRandomUrl();
        if (url.isEmpty()) { return; }
        CLogMessage(this).info(u"Trigger read of VATSIM status file from '%1'") << url.toQString(true);
        this->getFromNetworkAndLog(url, { this, &CVatsimStatusFileReader::parseVatsimFile});

        if (urls.size() < 2) { return; }
        const CUrl secondary = urls.getRandomWithout(url);
        if (secondary.isEmpty()) { return; }

        constexpr int DelayMs = 5000;
        const QPointer<CVatsimStatusFileReader> myself(this);
        QTimer::singleShot(DelayMs, this, [ = ]
        {
            if (!myself) { return; }
            const CVatsimSetup vs(m_lastGoodSetup.get());
            if (vs.getTimeDifferenceToNowMs() > 2 * DelayMs)
            {
                // not yet read
                this->getFromNetworkAndLog(url, { this, &CVatsimStatusFileReader::parseVatsimFile});
            }
        });
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
            const QString dataFileData = nwReply->readAll();
            nwReply->close(); // close asap

            if (dataFileData.isEmpty()) return;
            const QList<QStringRef> lines = splitLinesRefs(dataFileData);
            if (lines.isEmpty()) { return; }

            CUrlList dataFileUrls;
            CUrlList serverFileUrls;
            CUrlList metarFileUrls;

            QString currentLine; // declared outside of the for loop, to amortize the cost of allocation
            for (const QStringRef &clRef : lines)
            {
                if (!this->doWorkCheck())
                {
                    CLogMessage(this).debug() << Q_FUNC_INFO;
                    CLogMessage(this).info(u"Terminated status parsing process"); // for users
                    return; // stop, terminate straight away, ending thread
                }

                // parse lines
                currentLine = clRef.toString().trimmed();
                if (currentLine.isEmpty()) { continue; }
                if (currentLine.startsWith(";")) { continue; }
                if (!currentLine.contains("="))  { continue; }

                const QStringList parts(currentLine.split('='));
                if (parts.length() != 2) { continue; }
                const QString key(parts[0].trimmed().toLower());
                const QString value(parts[1].trimmed());
                const CUrl url(value);
                if (key.startsWith("json3"))
                {
                    dataFileUrls.push_back(url);
                }
                else if (key.startsWith("url1"))
                {
                    serverFileUrls.push_back(url);
                }
                else if (key.startsWith("metar"))
                {
                    metarFileUrls.push_back(url);
                }
                else if (key.startsWith("atis"))
                {
                    // not yet used
                }
            } // for each line

            // cache itself is thread safe, avoid writing with unchanged data
            CVatsimSetup vs(m_lastGoodSetup.get());
            const bool changed = vs.setUrls(dataFileUrls, serverFileUrls, metarFileUrls);
            vs.setUtcTimestamp(QDateTime::currentDateTime());
            const CStatusMessage cacheMsg = m_lastGoodSetup.set(vs);
            if (cacheMsg.isFailure()) { CLogMessage::preformatted(cacheMsg); }
            else
            {
                CLogMessage(this).info(u"Read VATSIM status file from '%1', %2 data file URLs, %3 server file URLs, %4 METAR file URLs")
                        << urlString << dataFileUrls.size() << serverFileUrls.size() << metarFileUrls.size();
            }
            Q_UNUSED(changed);

            // data read finished
            emit this->dataFileRead(lines.count());
            emit this->dataRead(CEntityFlags::VatsimStatusFile, CEntityFlags::ReadFinished, lines.count());
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
