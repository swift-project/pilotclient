/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/weather/weather.h"

/*
 * Metadata for aviation
 *
 * In a separate file to workaround a limitation of MinGW:
 * http://stackoverflow.com/q/16596876/1639256
 */
void BlackMisc::Weather::registerMetadata()
{
    CMetar::registerMetadata();
    CMetarSet::registerMetadata();
    CPresentWeather::registerMetadata();
    CPresentWeatherList::registerMetadata();
    CCloudLayer::registerMetadata();
    CCloudLayerList::registerMetadata();
    CWindLayer::registerMetadata();
}
