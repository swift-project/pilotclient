// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/weather/registermetadataweather.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

// Weather headers
#include "blackmisc/weather/cloudlayer.h"
#include "blackmisc/weather/cloudlayerlist.h"
#include "blackmisc/weather/gridpoint.h"
#include "blackmisc/weather/metar.h"
#include "blackmisc/weather/metarlist.h"
#include "blackmisc/weather/presentweather.h"
#include "blackmisc/weather/presentweatherlist.h"
#include "blackmisc/weather/temperaturelayer.h"
#include "blackmisc/weather/temperaturelayerlist.h"
#include "blackmisc/weather/visibilitylayer.h"
#include "blackmisc/weather/visibilitylayerlist.h"
#include "blackmisc/weather/weatherdataplugininfo.h"
#include "blackmisc/weather/weatherdataplugininfolist.h"
#include "blackmisc/weather/weathergrid.h"
#include "blackmisc/weather/weatherscenario.h"
#include "blackmisc/weather/windlayer.h"
#include "blackmisc/weather/windlayerlist.h"

namespace BlackMisc::Weather
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

} // ns
