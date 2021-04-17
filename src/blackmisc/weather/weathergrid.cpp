/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/weather/cloudlayer.h"
#include "blackmisc/weather/cloudlayerlist.h"
#include "blackmisc/weather/temperaturelayer.h"
#include "blackmisc/weather/temperaturelayerlist.h"
#include "blackmisc/weather/visibilitylayer.h"
#include "blackmisc/weather/visibilitylayerlist.h"
#include "blackmisc/weather/weathergrid.h"
#include "blackmisc/weather/windlayer.h"
#include "blackmisc/weather/windlayerlist.h"

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/temperature.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/range.h"
#include "blackmisc/verify.h"

#include "blackconfig/buildconfig.h"

using namespace BlackConfig;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;

namespace BlackMisc
{
    namespace Weather
    {
        CWeatherGrid::CWeatherGrid(const CSequence<CGridPoint> &other) :
            CSequence<CGridPoint>(other)
        { }

        CWeatherGrid::CWeatherGrid(const ICoordinateGeodetic &coordinate)
        {
            const CGridPoint p("GLOB", coordinate);
            this->push_back(p);
        }

        CWeatherGrid CWeatherGrid::findWithinRange(const ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range) const
        {
            return findBy([&](const CGridPoint & gridPoint)
            {
                return calculateGreatCircleDistance(gridPoint.getPosition(), coordinate) <= range;
            });
        }

        CWeatherGrid CWeatherGrid::findClosest(int number, const ICoordinateGeodetic &coordinate) const
        {
            CWeatherGrid closest = partiallySorted(number, [ & ](const CGridPoint & a, const CGridPoint & b)
            {
                return calculateEuclideanDistanceSquared(a.getPosition(), coordinate) < calculateEuclideanDistanceSquared(b.getPosition(), coordinate);
            });
            closest.truncate(number);
            return closest;
        }

        QString CWeatherGrid::getDescription(const QString sep) const
        {
            QString s;
            QTextStream qtout(&s);
            for (const CGridPoint &gridPoint : *this)
            {
                qtout << "Latitude: "  << gridPoint.getPosition().latitude().toQString()    << sep;
                qtout << "Longitude: " << gridPoint.getPosition().longitude().toQString()  << sep;
                qtout << "    MSL Pressure: " << gridPoint.getPressureAtMsl().toQString() << sep;

                CTemperatureLayerList temperatureLayers = gridPoint.getTemperatureLayers();
                temperatureLayers.sort([](const CTemperatureLayer & a, const CTemperatureLayer & b) { return a.getLevel() < b.getLevel(); });
                qtout << "    Temperature Layers: " << sep;
                for (const auto &temperatureLayer : std::as_const(temperatureLayers))
                {
                    qtout << "        Level: " << temperatureLayer.getLevel().toQString() << sep;
                    qtout << "            Temperature: " << temperatureLayer.getTemperature().toQString() << sep;
                    qtout << "            Relative Humidity: " << temperatureLayer.getRelativeHumidity() << " %" << sep;
                }
                qtout << sep;

                CWindLayerList windLayers = gridPoint.getWindLayers();
                windLayers.sort([](const CWindLayer & a, const CWindLayer & b) { return a.getLevel() < b.getLevel(); });
                qtout << "    Wind Layers: " << sep;
                for (const auto &windLayer : std::as_const(windLayers))
                {
                    qtout << "        Level: " << windLayer.getLevel().toQString() << sep;
                    qtout << "            Wind: " << windLayer.getDirection().toQString() << " at " << windLayer.getSpeed().toQString() << sep;
                }
                qtout << sep;

                qtout << "    Cloud Layers: " << sep;
                CCloudLayerList cloudLayers = gridPoint.getCloudLayers();
                cloudLayers.sort([](const CCloudLayer & a, const CCloudLayer & b) { return a.getBase() < b.getBase(); });
                for (int i = 0; i < cloudLayers.size(); i++)
                {
                    const CCloudLayer &cloudLayer = cloudLayers[i];
                    qtout << "        Top: " << cloudLayer.getTop().toQString() << sep;
                    qtout << "            Coverage: " << cloudLayer.getCoveragePercent() << " %"     << sep;
                    qtout << "            Precipitation type: " << cloudLayer.getPrecipitation()     << sep;
                    qtout << "            Precipitation rate: " << cloudLayer.getPrecipitationRate() << sep;
                    qtout << "        Base: " << cloudLayer.getBase().toQString() << sep;
                }
                qtout << sep << sep;
            }

            qtout.flush();
            return s;
        }

        const QVector<CWeatherScenario> &CWeatherGrid::getAllScenarios()
        {
            static const QVector<CWeatherScenario> scenarios =
            {
                { CWeatherScenario::ClearSky     },
                { CWeatherScenario::Thunderstorm },
                { CWeatherScenario::RealWeather  },
            };
            return scenarios;
        }

        const CWeatherGrid &CWeatherGrid::getByScenario(const CWeatherScenario &scenario)
        {
            static const CWeatherGrid emptyGrid {};
            switch (scenario.getIndex())
            {
            case CWeatherScenario::ClearSky:     return getClearWeatherGrid();
            case CWeatherScenario::Thunderstorm: return getThunderStormGrid();
            default:
                // in release versions just return, no need to ASSERT
                BLACK_VERIFY_X(!CBuildConfig::isLocalDeveloperDebugBuild(), Q_FUNC_INFO, "Unknown fixed scenario index requested.");
                return emptyGrid;
            }
        }

        const CWeatherGrid &CWeatherGrid::getClearWeatherGrid()
        {
            static const CVisibilityLayer visibilityLayer(
                CAltitude(0, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                CAltitude(2728, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                CLength(50, CLengthUnit::mi()));

            static const CTemperatureLayer temperatureLayer(
                CAltitude(0, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                CTemperature(22, CTemperatureUnit::C()),
                CTemperature(7, CTemperatureUnit::C()),
                0);

            static const CCloudLayer cloudLayer(
                CAltitude(0, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                CAltitude(5000, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                0, CCloudLayer::NoPrecipitation, CCloudLayer::NoClouds,
                CCloudLayer::None);

            static const CWindLayer windLayer(
                CAltitude(0, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                CAngle(0, CAngleUnit::deg()),
                CSpeed(0, CSpeedUnit::kts()),
                CSpeed(0, CSpeedUnit::kts()));

            static const CGridPoint gridPointGLOB =
            {
                "GLOB",
                CCoordinateGeodetic::null(),
                CCloudLayerList { cloudLayer },
                CTemperatureLayerList { temperatureLayer },
                CVisibilityLayerList  { visibilityLayer },
                CWindLayerList        { windLayer },
                { CAltitude::standardISASeaLevelPressure() }
            };

            static const CWeatherGrid weatherGrid = { gridPointGLOB };
            return weatherGrid;
        }

        const CWeatherGrid &CWeatherGrid::getThunderStormGrid()
        {
            static const CVisibilityLayer visibilityLayer(
                CAltitude(0, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                CAltitude(2728, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                CLength(50, CLengthUnit::mi()));

            static const CTemperatureLayer temperatureLayer(
                CAltitude(0, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                CTemperature(20, CTemperatureUnit::C()),
                CTemperature(18, CTemperatureUnit::C()), // min 55 Fahrenheit, 13 Celsius for thunderstorm
                83.1); // Dampness: a moist air layer at ground level with a larger extension and relative humidity above 80%;

            static const CCloudLayer cloudLayer1(
                CAltitude(630,  CAltitude::MeanSeaLevel, CLengthUnit::m()),
                CAltitude(4630, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                5, CCloudLayer::Rain, CCloudLayer::Thunderstorm,
                CCloudLayer::Overcast);

            static const CCloudLayer cloudLayer2(
                CAltitude(10130, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                CAltitude(11130, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                0, CCloudLayer::NoPrecipitation, CCloudLayer::Cirrus,
                CCloudLayer::Scattered);

            static const CWindLayer windLayer1(
                CAltitude(1110, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                CAngle(325.6, CAngleUnit::deg()),
                CSpeed(18, CSpeedUnit::kts()),
                CSpeed(0, CSpeedUnit::kts()));

            static const CWindLayer windLayer2(
                CAltitude(1130, CAltitude::MeanSeaLevel, CLengthUnit::m()),
                CAngle(334.8, CAngleUnit::deg()),
                CSpeed(21, CSpeedUnit::kts()),
                CSpeed(8, CSpeedUnit::kts()));

            static const CGridPoint gridPointGLOB
            {
                "GLOB",
                CCoordinateGeodetic::null(),
                CCloudLayerList { cloudLayer1, cloudLayer2 },
                CTemperatureLayerList { temperatureLayer },
                CVisibilityLayerList { visibilityLayer },
                CWindLayerList { windLayer1, windLayer2 },
                { CAltitude::standardISASeaLevelPressure() }
            };

            static const CWeatherGrid weatherGrid({ gridPointGLOB });
            return weatherGrid;
        }

    } // namespace
} // namespace
