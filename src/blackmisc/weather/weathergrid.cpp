/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "weathergrid.h"

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Weather
    {
        CWeatherGrid::CWeatherGrid(const CSequence<CGridPoint> &other) :
            CSequence<CGridPoint>(other)
        { }

        const CWeatherGrid &CWeatherGrid::getCavokGrid()
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
                5, CCloudLayer::NoPrecipitation, CCloudLayer::NoClouds,
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
                CWindLayerList { windLayer }
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
                CWindLayerList { windLayer1, windLayer2 }
            };

            static const CWeatherGrid weatherGrid({ gridPointGLOB });
            return weatherGrid;
        }

    } // namespace
} // namespace
