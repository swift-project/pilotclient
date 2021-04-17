/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/weatherdata.h"
#include "blackcore/weathermanager.h"
#include "blackcore/application.h"

#include "blackmisc/weather/gridpoint.h"
#include "blackmisc/weather/weatherdataplugininfo.h"
#include "blackmisc/weather/weatherdataplugininfolist.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/range.h"
#include "blackmisc/verify.h"
#include "blackmisc/statusmessage.h"
#include "blackconfig/buildconfig.h"

#include <QtGlobal>
#include <QDateTime>

using namespace BlackConfig;
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

    void CWeatherManager::requestWeatherGrid(const ICoordinateGeodetic &position, const CIdentifier &identifier)
    {
        const CWeatherGrid weatherGrid = CWeatherGrid { { "GLOB", position } };
        this->requestWeatherGrid(weatherGrid, identifier);
    }

    void CWeatherManager::appendRequest(const WeatherRequest &request)
    {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        if (m_pendingRequests.size() > 0 && m_lastPendingRequestTs > 0)
        {
            const qint64 pendingMs = now - m_lastPendingRequestTs;
            if (pendingMs > 30 * 1000)
            {
                BLACK_VERIFY_X(!CBuildConfig::isLocalDeveloperDebugBuild(), Q_FUNC_INFO, "Time out of pending weather request");
                CLogMessage(this).warning(u"Time out of pending weather request, cleaning all requests!");
                m_pendingRequests.clear();
            }
        }

        m_pendingRequests.append(request);
        m_lastPendingRequestTs = now;
    }


    void CWeatherManager::requestWeatherGrid(const CWeatherGrid &initialWeatherGrid, const CIdentifier &identifier)
    {
        if (identifier.isNull() || initialWeatherGrid.isEmpty() || initialWeatherGrid.frontOrDefault().getPosition().isNull())
        {
            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                BLACK_VERIFY_X(!identifier.isNull(), Q_FUNC_INFO, "Missing callback");
                BLACK_VERIFY_X(!initialWeatherGrid.isEmpty(), Q_FUNC_INFO, "Empty grid, need position");
                BLACK_VERIFY_X(!initialWeatherGrid.frontOrDefault().getPosition().isNull(), Q_FUNC_INFO, "NULL position");
            }
            return;
        }

        const WeatherRequest weatherRequest { {}, identifier, initialWeatherGrid, {} };
        this->appendRequest(weatherRequest);

        // Serialize the requests, since plugins can handle only one at a time
        if (m_pendingRequests.size() == 1) { fetchNextWeatherDataDeferred(); }
    }

    void CWeatherManager::requestWeatherGrid(const CWeatherGrid &initialWeatherGrid,
            const CSlot<void(const CWeatherGrid &)> &callback)
    {
        if (!callback || initialWeatherGrid.isEmpty() || initialWeatherGrid.frontOrDefault().getPosition().isNull())
        {
            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                BLACK_VERIFY_X(callback, Q_FUNC_INFO, "Missing callback");
                BLACK_VERIFY_X(!initialWeatherGrid.isEmpty(), Q_FUNC_INFO, "Empty grid, need position");
                BLACK_VERIFY_X(!initialWeatherGrid.frontOrDefault().getPosition().isNull(), Q_FUNC_INFO, "NULL position");
            }
            return;
        }

        if (m_isWeatherClear)
        {
            callback.singleShot(CWeatherGrid::getClearWeatherGrid());
            return;
        }

        const WeatherRequest weatherRequest { {}, CIdentifier::null(), initialWeatherGrid, callback };
        this->appendRequest(weatherRequest);

        // Serialize the requests, since plugins can handle only one at a time
        if (m_pendingRequests.size() == 1) { fetchNextWeatherDataDeferred(); }
    }

    void CWeatherManager::requestWeatherGridFromFile(const QString &filePath, const CWeatherGrid &initialWeatherGrid,
            const CSlot<void(const CWeatherGrid &)> &callback)
    {
        if (m_isWeatherClear && callback)
        {
            callback.singleShot(CWeatherGrid::getClearWeatherGrid());
            return;
        }

        WeatherRequest weatherRequest { filePath, CIdentifier::null(), initialWeatherGrid, callback };
        this->appendRequest(weatherRequest);

        // Serialize the requests, since plugins can handle only one at a time
        if (m_pendingRequests.size() == 1) { fetchNextWeatherDataDeferred(); }
    }

    bool CWeatherManager::loadWeatherDataPlugins()
    {
        const CWeatherDataPluginInfoList weatherDataPluginInfos = m_pluginManagerWeatherData.getAvailableWeatherDataPlugins();
        if (weatherDataPluginInfos.isEmpty())
        {
            CLogMessage(this).warning(u"No weather data plugin found!");
            return false;
        }

        for (const auto &pluginInfo : weatherDataPluginInfos)
        {
            IWeatherDataFactory *factory = m_pluginManagerWeatherData.getPluginById<IWeatherDataFactory>(pluginInfo.getIdentifier());
            if (!factory)
            {
                CLogMessage(this).error(u"Failed to create IWeatherDataFactory for %1") << pluginInfo.getIdentifier();
                return false;
            }

            IWeatherData *weatherData = factory->create(this);
            if (!weatherData)
            {
                CLogMessage(this).error(u"Failed to create IWeatherData instance for %1") << pluginInfo.getIdentifier();
                return false;
            }

            connect(weatherData, &IWeatherData::fetchingFinished, this, &CWeatherManager::handleNextRequest, Qt::QueuedConnection);
            m_weatherDataPlugins.append(weatherData);
            delete factory;
        }
        return true;
    }

    void CWeatherManager::fetchNextWeatherData()
    {
        const WeatherRequest weatherRequest = m_pendingRequests.constFirst();
        const CLength maxDistance(100.0, CLengthUnit::km());

        for (IWeatherData *plugin : std::as_const(m_weatherDataPlugins))
        {
            if (!plugin) { continue; }
            if (weatherRequest.filePath.isEmpty())
            {
                plugin->fetchWeatherData(weatherRequest.weatherGrid, maxDistance);
            }
            else
            {
                plugin->fetchWeatherDataFromFile(weatherRequest.filePath, weatherRequest.weatherGrid, maxDistance);
            }
        }
    }

    void CWeatherManager::fetchNextWeatherDataDeferred()
    {
        QPointer<CWeatherManager> myself(this);
        QTimer::singleShot(0, this, [ = ]
        {
            if (!myself || !sApp || sApp->isShuttingDown()) { return; }
            myself->fetchNextWeatherData();
        });
    }

    void CWeatherManager::handleNextRequest()
    {
        if (m_pendingRequests.isEmpty())
        {
            // we should normally never get here, only if a request timedout
            CLogMessage(this).warning(u"No pending weather request, ignoring ....");
            BLACK_VERIFY_X(!CBuildConfig::isLocalDeveloperDebugBuild(), Q_FUNC_INFO, "No pending weather request, ignoring ....");
            return;
        }

        IWeatherData *weatherDataPlugin = qobject_cast<IWeatherData *>(sender());
        Q_ASSERT(weatherDataPlugin);
        const auto fetchedWeatherGrid = weatherDataPlugin->getWeatherData();

        const WeatherRequest weatherRequest = m_pendingRequests.front();
        CWeatherGrid requestedWeatherGrid   = weatherRequest.weatherGrid;

        // Interpolation. So far it just picks the closest point without interpolation.
        for (CGridPoint &gridPoint : requestedWeatherGrid)
        {
            const auto nearestGridPoint = fetchedWeatherGrid.findClosest(1, gridPoint.getPosition()).frontOrDefault();
            gridPoint.copyWeatherDataFrom(nearestGridPoint);
            gridPoint.setPosition(nearestGridPoint.getPosition());
        }

        if (weatherRequest.callback)
        {
            weatherRequest.callback.singleShot(requestedWeatherGrid);
        }

        if (!weatherRequest.identifier.isAnonymous()) { emit weatherGridReceived(requestedWeatherGrid, weatherRequest.identifier); }
        m_pendingRequests.pop_front();

        // In case there are pending requests, start over again
        if (m_pendingRequests.size() > 0)
        {
            fetchNextWeatherData();
        }
        else
        {
            m_lastPendingRequestTs = -1;
        }
    }
}
