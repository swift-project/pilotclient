/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "registermetadataweather.h"
#include "weather.h"

namespace BlackMisc
{
    namespace Weather
    {
        void registerMetadata()
        {
            CCloudLayer::registerMetadata();
            CCloudLayerList::registerMetadata();
            CGridPoint::registerMetadata();
            CMetar::registerMetadata();
            CMetarSet::registerMetadata();
            CPresentWeather::registerMetadata();
            CPresentWeatherList::registerMetadata();
            CTemperatureLayer::registerMetadata();
            CTemperatureLayerList::registerMetadata();
            CWeatherDataPluginInfo::registerMetadata();
            CWeatherDataPluginInfoList::registerMetadata();
            CWeatherGrid::registerMetadata();
            CWindLayer::registerMetadata();
            CWindLayerList::registerMetadata();
        }
    } // ns
} // ns
