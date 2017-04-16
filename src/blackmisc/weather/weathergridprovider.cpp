/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/weather/weathergridprovider.h"

using namespace BlackMisc;

namespace BlackMisc
{
    namespace Weather
    {

        void CWeatherGridAware::requestWeatherGrid(const BlackMisc::Weather::CWeatherGrid &weatherGrid,
                                                   const BlackMisc::CSlot<void(const BlackMisc::Weather::CWeatherGrid &)> &callback)
        {
            Q_ASSERT_X(this->m_weatherGridProvider, Q_FUNC_INFO, "No object available");
            this->m_weatherGridProvider->requestWeatherGrid(weatherGrid, callback);
        }

    } // namespace
} // namespace
