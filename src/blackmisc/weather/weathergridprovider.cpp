// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
