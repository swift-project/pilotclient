/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/temperature.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/range.h"
#include "blackmisc/weather/cloudlayer.h"
#include "blackmisc/weather/cloudlayerlist.h"
#include "blackmisc/weather/temperaturelayer.h"
#include "blackmisc/weather/temperaturelayerlist.h"
#include "blackmisc/weather/visibilitylayer.h"
#include "blackmisc/weather/visibilitylayerlist.h"
#include "blackmisc/weather/weathergrid.h"
#include "blackmisc/weather/windlayer.h"
#include "blackmisc/weather/windlayerlist.h"

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

        const QVector<CWeatherScenario> &CWeatherGrid::getAllScenarios()
        {
            static const QVector<CWeatherScenario> scenarios =
            {
                { CWeatherScenario::ClearSky, "Clear Sky", "Clear sky, no clouds" },
                { CWeatherScenario::Thunderstorm, "Thunderstorm", "Raining, lightning, several cloud layers" },
                { CWeatherScenario::RealWeather, "Realtime Weather", "As real as it gets..." },
            };
            return scenarios;
        }

        const CWeatherGrid &CWeatherGrid::getByScenario(const CWeatherScenario &scenario)
        {
            static const CWeatherGrid emptyGrid {};
            switch(scenario.getIndex())
            {
            case CWeatherScenario::ClearSky: return getClearWeatherGrid();
            case CWeatherScenario::Thunderstorm: return getThunderStormGrid();
            default: Q_ASSERT_X(false, Q_FUNC_INFO, "Unknown fixed scenario index requested."); return emptyGrid;
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
                CTemperature(20, CTemperatureUnit::C()),
                CTemperature(18, CTemperatureUnit::C()),
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
                {},
                CCloudLayerList { cloudLayer },
                CTemperatureLayerList { temperatureLayer },
                CVisibilityLayerList { visibilityLayer } ,
                CWindLayerList { windLayer },
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
                CTemperature(18, CTemperatureUnit::C()),
                0);

            static const CCloudLayer cloudLayer1(
                CAltitude(630, CAltitude::MeanSeaLevel, CLengthUnit::m()),
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
                {},
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
