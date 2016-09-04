/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "airportdatareader.h"
#include "blackcore/application.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/networkutils.h"
#include <QNetworkReply>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackCore
{
    CAirportDataReader::CAirportDataReader(QObject* parent) :
        CThreadedReader(parent, QStringLiteral("CAirportDataReader"))
    {
        // void
    }

    void CAirportDataReader::readInBackgroundThread()
    {
        bool s = QMetaObject::invokeMethod(this, "ps_readAirports");
        Q_ASSERT_X(s, Q_FUNC_INFO, "Cannot invoke ");
        Q_UNUSED(s);
    }

    BlackMisc::Aviation::CAirportList CAirportDataReader::getAirports() const
    {
        return m_airportCache.get();
    }

    CUrl CAirportDataReader::getAirportsUrl() const
    {
        const CUrl url(sApp->getGlobalSetup().getSwiftAirportUrls().getRandomWorkingUrl());
        return url;
    }

    void CAirportDataReader::ps_parseAirportData(QNetworkReply *nwReply)
    {
        QJsonParseError error;
        QByteArray data = nwReply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(data, &error);
        if (error.error != QJsonParseError::NoError)
        {
            CLogMessage(this).error("Error parsing airport list from JSON (%1)") << error.errorString();
            return;
        }

        QJsonArray array = document.array();
        if (array.isEmpty())
        {
            CLogMessage(this).error("Error parsing airport list from JSON (document is not an array)");
            return;
        }

        CAirportList airports;
        airports.convertFromDatabaseJson(array);
        quint64 timestamp = lastModifiedMsSinceEpoch(nwReply);

        {
            QWriteLocker wl(&this->m_lock);
            m_airportCache.set(airports, timestamp);
        }

        emit dataRead(CEntityFlags::AirportEntity, CEntityFlags::ReadFinished, airports.size());
    }

    void CAirportDataReader::ps_parseAirportHeader(QNetworkReply *nwReply)
    {
        this->threadAssertCheck();
        m_lastModified = lastModifiedMsSinceEpoch(nwReply);
        ps_readAirports();
    }

    void CAirportDataReader::ps_readAirports()
    {
        this->threadAssertCheck();
        Q_ASSERT_X(sApp, Q_FUNC_INFO, "No Application");

        CFailoverUrlList urls(sApp->getGlobalSetup().getSwiftAirportUrls());
        const CUrl url(urls.obtainNextWorkingUrl(true));
        if (url.isEmpty()) { return; }

        if (0 == m_lastModified) {
            sApp->headerFromNetwork(url, { this, &CAirportDataReader::ps_parseAirportHeader });
            return;
        }

        m_airportCache.synchronize();

        int size = m_airportCache.get().size();
        if (size > 0 &&
                m_airportCache.getAvailableTimestamp().toMSecsSinceEpoch() >= static_cast<qint64>(m_lastModified)) // cache is up-to-date
        {
            CLogMessage(this).info("Loaded %1 airports from cache") << m_airportCache.get().size();
            emit dataRead(CEntityFlags::AirportEntity, CEntityFlags::ReadFinished, size);
        }
        else
        {
            sApp->getFromNetwork(url, { this, &CAirportDataReader::ps_parseAirportData });
        }
    }

    void CAirportDataReader::ps_airportCacheChanged()
    {
        // void
    }

} // ns
