/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/weather/registermetadataweather.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/weather/weather.h"
#include "blackmisc/variant.h"

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
            CMetarList::registerMetadata();
            CPresentWeather::registerMetadata();
            CPresentWeatherList::registerMetadata();
            CTemperatureLayer::registerMetadata();
            CTemperatureLayerList::registerMetadata();
            CVisibilityLayer::registerMetadata();
            CVisibilityLayerList::registerMetadata();
            CWeatherDataPluginInfo::registerMetadata();
            CWeatherDataPluginInfoList::registerMetadata();
            CWeatherGrid::registerMetadata();
            CWeatherScenario::registerMetadata();
            CWindLayer::registerMetadata();
            CWindLayerList::registerMetadata();
        }
    }

} // ns
