/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
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

namespace BlackCore
{
    namespace Vatsim
    {
        CVatsimStatusFileReader::CVatsimStatusFileReader(QObject *owner) :
            CThreadedReader(owner, "CVatsimStatusFileReader")
        {
            // do not connect with time, will be read once at startup
        }

        void CVatsimStatusFileReader::readInBackgroundThread()
        {
            const bool s = QMetaObject::invokeMethod(this, "ps_read");
            Q_ASSERT_X(s, Q_FUNC_INFO, "Invoke failed");
            Q_UNUSED(s);
        }

        CUrlList CVatsimStatusFileReader::getMetarFileUrls() const
        {
            return this->m_lastGoodSetup.get().getMetarFileUrls();
        }

        CUrlList CVatsimStatusFileReader::getDataFileUrls() const
        {
            return this->m_lastGoodSetup.get().getDataFileUrls();
        }

        void CVatsimStatusFileReader::ps_read()
        {
            this->threadAssertCheck();
            if (!this->doWorkCheck()) { return; }
            if (!this->isInternetAccessible("No network/internet access, cannot read VATSIM status file")) { return; }

            Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing application");
            CFailoverUrlList urls(sApp->getGlobalSetup().getVatsimStatusFileUrls());
            const CUrl url(urls.obtainNextWorkingUrl(true)); // random working URL
            if (url.isEmpty()) { return; }
            this->getFromNetworkAndLog(url, { this, &CVatsimStatusFileReader::parseVatsimFile});
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
                CLogMessage(this).info("Terminated VATSIM status file parsing process"); // for users
                return; // stop, terminate straight away, ending thread
            }

            this->logNetworkReplyReceived(nwReplyPtr);
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
                        CLogMessage(this).info("Terminated status parsing process"); // for users
                        return; // stop, terminate straight away, ending thread
                    }

                    // parse lines
                    currentLine = clRef.toString().trimmed();
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
                CVatsimSetup vs(this->m_lastGoodSetup.get());
                const bool changed = vs.setUrls(dataFileUrls, serverFileUrls, metarFileUrls);
                if (changed)
                {
                    vs.setUtcTimestamp(QDateTime::currentDateTime());
                    const CStatusMessage cacheMsg = this->m_lastGoodSetup.set(vs);
                    if (cacheMsg.isFailure()) { CLogMessage::preformatted(cacheMsg); }
                }

                // data read finished
                emit this->dataFileRead(lines.count());
                emit this->dataRead(CEntityFlags::VatsimStatusFile, CEntityFlags::ReadFinished, lines.count());
            }
            else
            {
                // network error
                CLogMessage(this).warning("Reading VATSIM status file failed '%1' '%2'") << nwReply->errorString() << nwReply->url().toString();
                nwReply->abort();
                emit this->dataRead(CEntityFlags::VatsimStatusFile, CEntityFlags::ReadFailed, 0);
            }
        }
    } // ns
} // ns
