/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_SIMULATOR_SETTINGS_H
#define BLACKCORE_SIMULATOR_SETTINGS_H

#include "blackmisc/settingscache.h"
#include "blackmisc/weather/weatherscenario.h"
#include "blackmisc/simulation/distributorlistpreferences.h"

namespace BlackCore
{
    namespace Simulator
    {
        //! Selected weather scenario
        struct TSelectedWeatherScenario : public BlackMisc::TSettingTrait<BlackMisc::Weather::CWeatherScenario>
        {
            //! \copydoc BlackCore::TSettingTrait::key
            static const char *key() { return "simulator/selectedweatherscenario"; }

            //! \copydoc BlackCore::TSettingTrait::defaultValue
            static const BlackMisc::Weather::CWeatherScenario &defaultValue()
            {
                static const BlackMisc::Weather::CWeatherScenario scenario {};
                return scenario;
            }
        };

        //! Mapping preferences for model distributor list
        struct TDistributorListPreferences : public BlackMisc::TSettingTrait<BlackMisc::Simulation::CDistributorListPreferences>
        {
            //! \copydoc BlackMisc::TSettingTrait::key
            static const char *key() { return "mapping/distributorpreferences"; }
        };
    } // ns
} // ns

#endif
