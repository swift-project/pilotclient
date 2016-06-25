/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/vatsim/vatsimmetarreader.h"
#include "blackcore/application.h"
#include "blackmisc/compare.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/url.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/statusmessage.h"

#include <QByteArray>
#include <QMetaObject>
#include <QNetworkReply>
#include <QReadLocker>
#include <QScopedPointer>
#include <QScopedPointerDeleteLater>
#include <QString>
#include <QTextStream>
#include <QTimer>
#include <QUrl>
#include <QWriteLocker>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Weather;
using namespace BlackCore::Data;

namespace BlackCore
{
    namespace Vatsim
    {
        CVatsimMetarReader::CVatsimMetarReader(QObject *owner) :
            CThreadedReader(owner, "CVatsimMetarReader")
        {
            this->connect(this->m_updateTimer, &QTimer::timeout, this, &CVatsimMetarReader::ps_readMetars);
        }

        void CVatsimMetarReader::readInBackgroundThread()
        {
            bool s = QMetaObject::invokeMethod(this, "ps_readMetars");
            Q_ASSERT_X(s, Q_FUNC_INFO, "Cannot invoke");
            Q_UNUSED(s);
        }

        CMetarSet CVatsimMetarReader::getMetars() const
        {
            QReadLocker l(&m_lock);
            return m_metars;
        }

        CMetar CVatsimMetarReader::getMetarForAirport(const Aviation::CAirportIcaoCode &icao) const
        {
            QReadLocker l(&m_lock);
            return m_metars.getMetarForAirport(icao);
        }

        int CVatsimMetarReader::getMetarsCount() const
        {
            QReadLocker l(&m_lock);
            return m_metars.size();
        }

        void CVatsimMetarReader::cleanup()
        {
            // void
        }

        Settings::CSettingsReader CVatsimMetarReader::getSettings() const
        {
            return m_settings.get();
        }

        void CVatsimMetarReader::ps_readMetars()
        {
            this->threadAssertCheck();
            CFailoverUrlList urls(sApp->getVatsimMetarUrls());
            const CUrl url(urls.obtainNextWorkingUrl(true));
            if (url.isEmpty()) { return; }
            Q_ASSERT_X(sApp, Q_FUNC_INFO, "No Application");
            sApp->getFromNetwork(url.withAppendedQuery("id=all"), { this, &CVatsimMetarReader::ps_decodeMetars});
        }

        void CVatsimMetarReader::ps_decodeMetars(QNetworkReply *nwReplyPtr)
        {
            // wrap pointer, make sure any exit cleans up reply
            // required to use delete later as object is created in a different thread
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);

            this->threadAssertCheck();

            // Worker thread, make sure to write thread safe!
            if (this->isAbandoned())
            {
                CLogMessage(this).debug() << Q_FUNC_INFO;
                CLogMessage(this).info("terminated METAR decoding process"); // for users
                return; // stop, terminate straight away, ending thread
            }

            if (nwReply->error() == QNetworkReply::NoError)
            {
                QString metarData = nwReply->readAll();
                nwReply->close(); // close asap

                // Quick check by hash
                if (!this->didContentChange(metarData))
                {
                    CLogMessage(this).info("METAR file has same content, skipped");
                    return;
                }

                CMetarSet metars;
                QString invalidMetars;
                int invalidLineCount = 0;
                QTextStream lineReader(&metarData);
                while (!lineReader.atEnd())
                {
                    if (this->isAbandoned()) { return; }
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

                {
                    QWriteLocker l(&m_lock);
                    m_metars = metars;
                }

                // I could use those for logging, etc.
                Q_UNUSED(invalidMetars);
                if (invalidLineCount > 0)
                {
                    // Regular issue, log it, but do not show to user
                    CLogMessage(this).debug() << "Reading METARs failed for entries" << invalidLineCount;
                }
                emit metarsRead(metars);
                emit dataRead(CEntityFlags::MetarEntity, CEntityFlags::ReadFinished, metars.size());
            }
            else
            {
                // network error
                CLogMessage(this).warning("Reading METARs failed %1 %2") << nwReply->errorString() << nwReply->url().toString();
                nwReply->abort();
                emit dataRead(CEntityFlags::MetarEntity, CEntityFlags::ReadFailed, 0);
            }
        } // method
    } // ns
} // ns
