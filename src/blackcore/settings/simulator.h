/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_SETTINGS_SIMULATOR_H
#define BLACKCORE_SETTINGS_SIMULATOR_H

#include "blackmisc/settingscache.h"
#include "blackmisc/weather/weatherscenario.h"

namespace BlackCore
{
    namespace Settings
    {
        namespace Simulator
        {
            //! Selected weather scenario
            struct SelectedWeatherScenario : public BlackMisc::CSettingTrait<BlackMisc::Weather::CWeatherScenario>
            {
                //! \copydoc BlackCore::CSettingTrait::key
                static const char *key() { return "simulator/selectedweatherscenario"; }

                //! \copydoc BlackCore::CSettingTrait::defaultValue
                static const BlackMisc::Weather::CWeatherScenario &defaultValue()
                {
                    static const BlackMisc::Weather::CWeatherScenario scenario {};
                    return scenario;
                }
            };
        } // ns
    } // ns
} // ns

#endif
