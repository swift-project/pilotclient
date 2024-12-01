// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_SETTINGS_MODELMATCHERSETTINGS_H
#define SWIFT_MISC_SIMULATION_SETTINGS_MODELMATCHERSETTINGS_H

#include "misc/settingscache.h"
#include "misc/simulation/aircraftmatchersetup.h"

namespace swift::misc::simulation::settings
{
    //! Trait for simulator settings
    struct TModelMatching : public TSettingTrait<CAircraftMatcherSetup>
    {
        //! Key in data cache
        static const char *key() { return "matching"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Model matching");
            return name;
        }
    };
} // namespace swift::misc::simulation::settings

#endif // SWIFT_MISC_SIMULATION_SETTINGS_MODELMATCHERSETTINGS_H
