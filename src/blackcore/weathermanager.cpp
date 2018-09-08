/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/weatherdata.h"
#include "blackcore/weathermanager.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/range.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/weather/gridpoint.h"
#include "blackmisc/weather/weatherdataplugininfo.h"
#include "blackmisc/weather/weatherdataplugininfolist.h"

#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Weather;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackCore
{
    CWeatherManager::CWeatherManager(QObject *parent) : QObject(parent)
    {
        m_pluginManagerWeatherData.collectPlugins();
        loadWeatherDataPlugins();
    }

    void CWeatherManager::setWeatherToClear(bool value)
    {
        m_isWeatherClear = value;
        // todo: send weather grid to drivers from here
    }

    void CWeatherManager::requestWeatherGrid(const CWeatherGrid &weatherGrid, const CIdentifier &identifier)
    {
        WeatherRequest request { {}, identifier, weatherGrid, {} };
        m_pendingRequests.append(request);
        // Serialize the requests, since plugins can handle only one at a time
        if (m_pendingRequests.size() == 1) { fetchNextWeatherData(); }
    }

    void CWeatherManager::requestWeatherGrid(const CWeatherGrid &weatherGrid,
            const CSlot<void(const CWeatherGrid &)> &callback)
    {
        if (m_isWeatherClear)
        {
            callback(CWeatherGrid::getClearWeatherGrid());
            return;
        }

        WeatherRequest weatherRequest { {}, CIdentifier::null(), weatherGrid, callback };
        m_pendingRequests.append(weatherRequest);

        // Serialize the requests, since plugins can handle only one at a time
        if (m_pendingRequests.size() == 1) { fetchNextWeatherData(); }
    }

    void CWeatherManager::requestWeatherGridFromFile(const QString &filePath, const CWeatherGrid &weatherGrid,
            const CSlot<void(const CWeatherGrid &)> &callback)
    {
        if (m_isWeatherClear)
        {
            callback(CWeatherGrid::getClearWeatherGrid());
            return;
        }

        WeatherRequest weatherRequest { filePath, CIdentifier::null(), weatherGrid, callback };
        m_pendingRequests.append(weatherRequest);

        // Serialize the requests, since plugins can handle only one at a time
        if (m_pendingRequests.size() == 1) { fetchNextWeatherData(); }
    }

    bool CWeatherManager::loadWeatherDataPlugins()
    {
        const CWeatherDataPluginInfoList weatherDataPluginInfos = m_pluginManagerWeatherData.getAvailableWeatherDataPlugins();
        if (weatherDataPluginInfos.isEmpty())
        {
            CLogMessage(this).warning("No weather data plugin found!");
            return false;
        }

        for (const auto &pluginInfo : weatherDataPluginInfos)
        {
            IWeatherDataFactory *factory = m_pluginManagerWeatherData.getPluginById<IWeatherDataFactory>(pluginInfo.getIdentifier());
            if (!factory)
            {
                CLogMessage(this).error("Failed to create IWeatherDataFactory for %1") << pluginInfo.getIdentifier();
                return false;
            }

            IWeatherData *weatherData = factory->create(this);
            if (!weatherData)
            {
                CLogMessage(this).error("Failed to create IWeatherData instance for %1") << pluginInfo.getIdentifier();
                return false;
            }

            connect(weatherData, &IWeatherData::fetchingFinished, this, &CWeatherManager::handleNextRequest);
            m_weatherDataPlugins.append(weatherData);
            delete factory;
        }
        return true;
    }

    void CWeatherManager::fetchNextWeatherData()
    {
        const WeatherRequest weatherRequest = m_pendingRequests.constFirst();
        PhysicalQuantities::CLength maxDistance(100.0, CLengthUnit::km());

        for (IWeatherData *plugin : as_const(m_weatherDataPlugins))
        {
            if (weatherRequest.filePath.isEmpty()) { plugin->fetchWeatherData(weatherRequest.weatherGrid, maxDistance); }
            else { plugin->fetchWeatherDataFromFile(weatherRequest.filePath, weatherRequest.weatherGrid, maxDistance); }
        }
    }

    void CWeatherManager::handleNextRequest()
    {
        Q_ASSERT(!m_pendingRequests.isEmpty());

        IWeatherData *weatherDataPlugin = qobject_cast<IWeatherData *>(sender());
        Q_ASSERT(weatherDataPlugin);
        auto fetchedWeatherGrid = weatherDataPlugin->getWeatherData();

        const WeatherRequest weatherRequest = m_pendingRequests.constFirst();
        CWeatherGrid requestedWeatherGrid = weatherRequest.weatherGrid;

        // Interpolation. So far it just picks the closest point without interpolation.
        for (CGridPoint &gridPoint : requestedWeatherGrid)
        {
            const auto nearestGridPoint = fetchedWeatherGrid.findClosest(1, gridPoint.getPosition()).frontOrDefault();
            gridPoint.copyWeatherDataFrom(nearestGridPoint);
        }

        if (weatherRequest.callback) { weatherRequest.callback(requestedWeatherGrid); }
        if (!weatherRequest.identifier.isAnonymous()) { emit weatherGridReceived(requestedWeatherGrid, weatherRequest.identifier); }
        m_pendingRequests.pop_front();

        // In case there are pending requests, start over again
        if (m_pendingRequests.size() > 0) { fetchNextWeatherData(); }
    }
}
