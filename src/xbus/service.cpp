/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "service.h"
#include <XPLM/XPLMPlanes.h>
#include <XPLM/XPLMUtilities.h>
#include <QDebug>
#include <QTimer>

namespace XBus
{

    CService::CService(QObject *parent) : QObject(parent)
    {
        m_airportUpdater = new QTimer(this);
        m_airportUpdater->start(60000);
        connect(m_airportUpdater, &QTimer::timeout, this, &CService::updateAirportsInRange);
        updateAirportsInRange();
    }

    void CService::onAircraftModelChanged()
    {
        char filename[256];
        char path[512];
        XPLMGetNthAircraftModel(XPLM_USER_AIRCRAFT, filename, path);
        emit aircraftModelChanged(path, filename, getAircraftLivery(), getAircraftIcaoCode());
    }

    QString CService::getAircraftModelPath() const
    {
        char filename[256];
        char path[512];
        XPLMGetNthAircraftModel(XPLM_USER_AIRCRAFT, filename, path);
        return path;
    }

    QString CService::getAircraftModelFilename() const
    {
        char filename[256];
        char path[512];
        XPLMGetNthAircraftModel(XPLM_USER_AIRCRAFT, filename, path);
        return filename;
    }

    int CService::getXPlaneVersionMajor() const
    {
        int version;
        XPLMGetVersions(&version, nullptr, nullptr);
        if (version > 5000) { version /= 10; }
        return version / 100;
    }

    int CService::getXPlaneVersionMinor() const
    {
        int version;
        XPLMGetVersions(&version, nullptr, nullptr);
        if (version > 5000) { version /= 10; }
        return version % 100;
    }

    QString CService::getXPlaneInstallationPath() const
    {
        char path[512];
        XPLMGetSystemPath(path);
        return path;
    }

    QString CService::getXPlanePreferencesPath() const
    {
        char path[512];
        XPLMGetPrefsPath(path);
        return path;
    }

    void CService::readAirportsDatabase()
    {
        auto first = XPLMFindFirstNavAidOfType(xplm_Nav_Airport);
        auto last = XPLMFindLastNavAidOfType(xplm_Nav_Airport);
        if (first != XPLM_NAV_NOT_FOUND)
        {
            for (auto i = first; i <= last; ++i)
            {
                float lat, lon;
                char icao[32];
                XPLMGetNavAidInfo(i, nullptr, &lat, &lon, nullptr, nullptr, nullptr, icao, nullptr, nullptr);
                if (icao[0] != 0)
                {
                    using namespace BlackMisc::Math;
                    m_airports.insert(CMathUtils::deg2rad(lat), CMathUtils::deg2rad(lon), i);
                }
            }
        }

        int total = 0, count = 0, max = 0;
        for (auto key : m_airports.keys())
        {
            qDebug() << "<><><><>" << QString("%1").arg(key, 6, 16, QChar('0')) << m_airports.count(key);
            total += m_airports.count(key);
            count++;
            if (m_airports.count(key) > max) { max = m_airports.count(key); }
        }
        qDebug() << "<><><><> total" << total;
        qDebug() << "<><><><> max" << max;
        qDebug() << "<><><><> mean" << (total / count);
    }

    void CService::updateAirportsInRange()
    {
        if (m_airports.isEmpty())
        {
            readAirportsDatabase();
        }
        using namespace BlackMisc::Math;
        QStringList icaos, names;
        QDoubleList lats, lons, alts;
        for (auto navref : m_airports.inAdjacentTiles(CMathUtils::deg2rad(getLatitude()), CMathUtils::deg2rad(getLongitude())))
        {
            float lat, lon, alt;
            char icao[32], name[256];
            XPLMGetNavAidInfo(navref, nullptr, &lat, &lon, &alt, nullptr, nullptr, icao, name, nullptr);
            icaos.push_back(icao);
            names.push_back(name);
            lats.push_back(lat);
            lons.push_back(lon);
            alts.push_back(alt);
        }
        qDebug() << alts;
        emit airportsInRangeUpdated(icaos, names, lats, lons, alts);
    }

}
