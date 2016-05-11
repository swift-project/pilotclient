/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/vatsimstatusfilereader.h"
#include "blackcore/application.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/logmessage.h"

#include <QNetworkReply>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackCore::Data;

namespace BlackCore
{
    CVatsimStatusFileReader::CVatsimStatusFileReader(QObject *owner) :
        CThreadedReader(owner, "CVatsimStatusFileReader")
    {
        this->connect(this->m_updateTimer, &QTimer::timeout, this, &CVatsimStatusFileReader::ps_read);
    }

    void CVatsimStatusFileReader::readInBackgroundThread()
    {
        bool s = QMetaObject::invokeMethod(this, "ps_read");
        Q_ASSERT_X(s, Q_FUNC_INFO, "Invoke failed");
        Q_UNUSED(s);
    }

    CUrlList CVatsimStatusFileReader::getMetarFileUrls() const
    {
        return this->m_lastGoodSetup.getCopy().getMetarFileUrls();
    }

    CUrlList CVatsimStatusFileReader::getDataFileUrls() const
    {
        return this->m_lastGoodSetup.getCopy().getDataFileUrls();
    }

    void CVatsimStatusFileReader::cleanup()
    {
        // void
    }

    void CVatsimStatusFileReader::ps_read()
    {
        this->threadAssertCheck();

        Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing application");
        CFailoverUrlList urls(sApp->getGlobalSetup().getVatsimStatusFileUrls());
        const CUrl url(urls.obtainNextWorkingUrl(true)); // random working URL
        if (url.isEmpty()) { return; }
        sApp->getFromNetwork(url, { this, &CVatsimStatusFileReader::ps_parseVatsimFile});
    }

    void CVatsimStatusFileReader::ps_parseVatsimFile(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);

        this->threadAssertCheck();

        // Worker thread, make sure to write only synced here!
        if (this->isAbandoned())
        {
            CLogMessage(this).debug() << Q_FUNC_INFO;
            CLogMessage(this).info("Terminated VATSIM status file parsing process"); // for users
            return; // stop, terminate straight away, ending thread
        }

        QStringList illegalIcaoCodes;
        if (nwReply->error() == QNetworkReply::NoError)
        {
            const QString dataFileData = nwReply->readAll();
            nwReply->close(); // close asap

            if (dataFileData.isEmpty()) return;
            const QStringList lines = dataFileData.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
            if (lines.isEmpty()) { return; }

            CUrlList dataFiles;
            CUrlList serverFiles;
            CUrlList metarFiles;

            for (const QString &cl : lines)
            {
                if (this->isAbandoned())
                {
                    CLogMessage(this).debug() << Q_FUNC_INFO;
                    CLogMessage(this).info("Terminated status parsing process"); // for users
                    return; // stop, terminate straight away, ending thread
                }

                // parse lines
                const QString currentLine(cl.trimmed());
                if (currentLine.isEmpty()) { continue; }
                if (currentLine.startsWith(";")) { continue; }
                if (!currentLine.contains("=")) { continue; }

                const QStringList parts(currentLine.split('='));
                if (parts.length() != 2) { continue; }
                const QString key(parts[0].trimmed().toLower());
                const QString value(parts[1].trimmed());
                const CUrl url(value);
                if (key.startsWith("url0"))
                {
                    dataFiles.push_back(url);
                }
                else if (key.startsWith("url1"))
                {
                    serverFiles.push_back(url);
                }
                else if (key.startsWith("metar"))
                {
                    metarFiles.push_back(url);
                }
                else if (key.startsWith("atis"))
                {
                    // not yet used
                }
            } // for each line

            // this part needs to be synchronized
            {
                // cache itself is thread safe
                CVatsimSetup vs(this->m_lastGoodSetup.getCopy());
                vs.setDataFileUrls(dataFiles);
                vs.setMetarFileUrls(metarFiles);
                vs.setServerFileUrls(serverFiles);
                vs.setUtcTimestamp(QDateTime::currentDateTime());
                this->m_lastGoodSetup.set(vs);
            }

            // warnings, if required
            if (!illegalIcaoCodes.isEmpty())
            {
                CLogMessage(this).info("Illegal / ignored ICAO code(s) in VATSIM data file: %1") << illegalIcaoCodes.join(", ");
            }

            // data read finished
            emit this->dataFileRead(lines.count());
            emit this->dataRead(CEntityFlags::VatsimStatusFile, CEntityFlags::ReadFinished, lines.count());
        }
        else
        {
            // network error
            CLogMessage(this).warning("Reading VATSIM status file failed %1 %2") << nwReply->errorString() << nwReply->url().toString();
            nwReply->abort();
            emit this->dataRead(CEntityFlags::VatsimStatusFile, CEntityFlags::ReadFailed, 0);
        }
    }

} // namespace
