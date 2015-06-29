/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "vatsim_metar_reader.h"
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
        this->connect(this->m_networkManager, &QNetworkAccessManager::finished, this, &CVatsimMetarReader::ps_decodeMetar);
        this->connect(this->m_updateTimer, &QTimer::timeout, this, &CVatsimMetarReader::ps_readMetar);
    }

    void CVatsimMetarReader::readInBackgroundThread()
    {
        bool s = QMetaObject::invokeMethod(this, "ps_readMetar");
        Q_ASSERT(s);
        Q_UNUSED(s);
    }

    void CVatsimMetarReader::ps_readMetar()
    {
        this->threadAssertCheck();
        QUrl url(this->m_metarUrl);
        if (url.isEmpty()) return;
        Q_ASSERT(this->m_networkManager);
        QNetworkRequest request(url);
        this->m_networkManager->get(request);
    }

    void CVatsimMetarReader::ps_decodeMetar(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);

        this->threadAssertCheck();

        // Worker thread, make sure to write no members here!
        if (this->isFinished())
        {
            CLogMessage(this).debug() << Q_FUNC_INFO;
            CLogMessage(this).info("terminated metar decoding process"); // for users
            return; // stop, terminate straight away, ending thread
        }

        if (nwReply->error() == QNetworkReply::NoError)
        {
            QString metarData = nwReply->readAll();
            nwReply->close(); // close asap

            CMetarSet metars;
            QFile outFile ("invalid_metar.txt");
            if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }
            QTextStream out(&outFile);

            QTextStream lineReader(&metarData);
            while (!lineReader.atEnd())
            {
                QString line = lineReader.readLine();
                if (line.startsWith("MSV 291130Z"))
                {
                    qDebug() << "now!";
                }

                CMetar metar = m_metarDecoder.decode(line);
                if (metar != CMetar())
                {
                    metars.push_back(metar);
                }
                else
                {
                    out << line << endl;
                }
            }
            outFile.flush();
            outFile.close();

            emit metarUpdated(metars);
        }
        else
        {
            // network error
            CLogMessage(this).warning("Reading METARS failed %1 %2") << nwReply->errorString() << nwReply->url().toString();
            nwReply->abort();
        }
    } // method

} // namespace
