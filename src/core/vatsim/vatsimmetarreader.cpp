// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/vatsim/vatsimmetarreader.h"

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

#include "core/application.h"
#include "misc/logmessage.h"
#include "misc/mixin/mixincompare.h"
#include "misc/network/entityflags.h"
#include "misc/network/url.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::weather;
using namespace swift::core::data;

namespace swift::core::vatsim
{
    CVatsimMetarReader::CVatsimMetarReader(QObject *owner) : CThreadedReaderPeriodic(owner, "CVatsimMetarReader")
    {
        this->reloadSettings();
    }

    CMetarList CVatsimMetarReader::getMetars() const
    {
        QReadLocker l(&m_lock);
        return m_metars;
    }

    CMetar CVatsimMetarReader::getMetarForAirport(const CAirportIcaoCode &icao) const
    {
        QReadLocker l(&m_lock);
        return m_metars.getMetarForAirport(icao);
    }

    void CVatsimMetarReader::doWorkImpl() { this->read(); }

    void CVatsimMetarReader::read()
    {
        this->threadAssertCheck();
        if (!this->doWorkCheck()) { return; }

        const CUrl url(sApp->getVatsimMetarUrl());
        if (url.isEmpty()) { return; }
        Q_ASSERT_X(sApp, Q_FUNC_INFO, "No Application");
        this->getFromNetworkAndLog(url.withAppendedQuery("id=all"), { this, &CVatsimMetarReader::decodeMetars });
    }

    void CVatsimMetarReader::decodeMetars(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);

        // Worker thread, make sure to write thread safe!
        this->threadAssertCheck();

        if (!this->doWorkCheck())
        {
            CLogMessage(this).info(u"Terminated METAR decoding process"); // for users
            return; // stop, terminate straight away, ending thread
        }

        this->logNetworkReplyReceived(nwReplyPtr);
        const QUrl url = nwReply->url();
        const QString metarUrl = url.toString();

        if (nwReply->error() == QNetworkReply::NoError)
        {
            QString metarData = nwReply->readAll();
            nwReply->close(); // close asap

            if (metarData.isEmpty()) // Quick check by hash
            {
                CLogMessage(this).warning(u"No METAR data from '%1', skipped") << metarUrl;
                return;
            }
            if (!this->didContentChange(metarData)) // Quick check by hash
            {
                CLogMessage(this).info(u"METAR file from '%1' has same content, skipped") << metarUrl;
                return;
            }

            CMetarList metars;
            int invalidLines = 0;
            QTextStream lineReader(&metarData);
            while (!lineReader.atEnd())
            {
                if (!this->doWorkCheck()) { return; }
                const QString line = lineReader.readLine();
                // some check for obvious errors
                if (line.contains("<html")) { continue; }
                const CMetar metar = m_metarDecoder.decode(line);
                if (metar != CMetar()) { metars.push_back(metar); }
                else { invalidLines++; }
            }

            CLogMessage(this).info(u"METARs: %1 Metars (invalid %2) from '%3'")
                << metars.size() << invalidLines << metarUrl;
            {
                QWriteLocker l(&m_lock);
                m_metars = metars;
            }

            emit metarsRead(metars);
            emit dataRead(CEntityFlags::MetarEntity, CEntityFlags::ReadFinished, metars.size(), url);
        }
        else
        {
            // network error
            CLogMessage(this).warning(u"Reading METARs failed '%1' for '%2'") << nwReply->errorString() << metarUrl;
            nwReply->abort();
            emit this->dataRead(CEntityFlags::MetarEntity, CEntityFlags::ReadFailed, 0, url);
        }
    } // method

    void CVatsimMetarReader::reloadSettings()
    {
        const CReaderSettings s = m_settings.get();
        this->setInitialAndPeriodicTime(std::chrono::milliseconds(s.getInitialTime().toMs()),
                                        std::chrono::milliseconds(s.getPeriodicTime().toMs()));
    }
} // namespace swift::core::vatsim
