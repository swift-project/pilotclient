/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/weather/weathergridprovider.h"

namespace BlackMisc::Weather
{
    void CWeatherGridAware::requestWeatherGrid(const Geo::ICoordinateGeodetic &position, const CIdentifier &identifier)
    {
        Q_ASSERT_X(this->hasProvider(), Q_FUNC_INFO, "No object available");
        this->provider()->requestWeatherGrid(position, identifier);
    }

    void CWeatherGridAware::requestWeatherGrid(
        const CWeatherGrid &weatherGrid,
        const CSlot<void(const CWeatherGrid &)> &callback)
    {
        Q_ASSERT_X(this->hasProvider(), Q_FUNC_INFO, "No object available");
        this->provider()->requestWeatherGrid(weatherGrid, callback);
    }
} // namespace
