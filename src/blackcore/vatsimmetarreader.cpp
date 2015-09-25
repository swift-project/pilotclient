/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "vatsimmetarreader.h"
#include "blackmisc/sequence.h"
#include "blackmisc/logmessage.h"
#include <QTextStream>
#include <QFile>

using namespace BlackMisc;
using namespace BlackMisc::Weather;

namespace BlackCore
{
    CVatsimMetarReader::CVatsimMetarReader(QObject *owner, const QString &url) :
        CThreadedReader(owner, "CVatsimMetarReader"),
        m_metarUrl(url)
    {
        this->m_networkManager = new QNetworkAccessManager(this);
        this->connect(this->m_networkManager, &QNetworkAccessManager::finished, this, &CVatsimMetarReader::ps_decodeMetars);
        this->connect(this->m_updateTimer, &QTimer::timeout, this, &CVatsimMetarReader::ps_readMetars);
    }

    void CVatsimMetarReader::readInBackgroundThread()
    {
        bool s = QMetaObject::invokeMethod(this, "ps_readMetar");
        Q_ASSERT_X(s, Q_FUNC_INFO, "Cannot invoke");
        Q_UNUSED(s);
    }

    void CVatsimMetarReader::ps_readMetars()
    {
        this->threadAssertCheck();
        QUrl url(this->m_metarUrl);
        if (url.isEmpty()) return;
        Q_ASSERT(this->m_networkManager);
        QNetworkRequest request(url);
        this->m_networkManager->get(request);
    }

    void CVatsimMetarReader::ps_decodeMetars(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);

        this->threadAssertCheck();

        // Worker thread, make sure to write no members here!
        if (this->isFinished())
        {
            CLogMessage(this).debug() << Q_FUNC_INFO;
            CLogMessage(this).info("terminated METAR decoding process"); // for users
            return; // stop, terminate straight away, ending thread
        }

        if (nwReply->error() == QNetworkReply::NoError)
        {
            QString metarData = nwReply->readAll();
            nwReply->close(); // close asap
            CMetarSet metars;

            QString invalidMetars;
            int invalidLineCount = 0;
            QTextStream lineReader(&metarData);
            while (!lineReader.atEnd())
            {
                QString line = lineReader.readLine();
                CMetar metar = m_metarDecoder.decode(line);
                if (metar != CMetar())
                {
                    metars.push_back(metar);
                }
                else
                {
                    invalidMetars += line;
                    invalidMetars += "\n";
                    invalidLineCount++;
                }
            }

            // I could use those for logging, etc.
            Q_UNUSED(invalidMetars);
            if (invalidLineCount > 0)
            {
                CLogMessage(this).warning("Reading METARs failed for %1 entries") << invalidLineCount;
            }
            emit dataRead(metars);
        }
        else
        {
            // network error
            CLogMessage(this).warning("Reading METARs failed %1 %2") << nwReply->errorString() << nwReply->url().toString();
            nwReply->abort();
        }
    } // method

} // namespace
