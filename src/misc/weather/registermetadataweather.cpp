// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/weather/registermetadataweather.h"

#include "misc/valueobject.h"
#include "misc/variant.h"

// Weather headers
#include "misc/weather/cloudlayer.h"
#include "misc/weather/cloudlayerlist.h"
#include "misc/weather/metar.h"
#include "misc/weather/metarlist.h"
#include "misc/weather/presentweather.h"
#include "misc/weather/presentweatherlist.h"
#include "misc/weather/windlayer.h"
#include "misc/weather/windlayerlist.h"

namespace swift::misc::weather
{
    void registerMetadata()
    {
        CCloudLayer::registerMetadata();
        CCloudLayerList::registerMetadata();
        CMetar::registerMetadata();
        CMetarList::registerMetadata();
        CPresentWeather::registerMetadata();
        CPresentWeatherList::registerMetadata();
        CWindLayer::registerMetadata();
        CWindLayerList::registerMetadata();
    }

} // namespace swift::misc::weather
