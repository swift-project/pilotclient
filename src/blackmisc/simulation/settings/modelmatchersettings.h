// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_SETTINGS_MODELMATCHERSETTINGS_H
#define BLACKMISC_SIMULATION_SETTINGS_MODELMATCHERSETTINGS_H

#include "blackmisc/simulation/aircraftmatchersetup.h"
#include "blackmisc/settingscache.h"

namespace BlackMisc::Simulation::Settings
{
    //! Trait for simulator settings
    struct TModelMatching : public TSettingTrait<CAircraftMatcherSetup>
    {
        //! Key in data cache
        static const char *key() { return "matching"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Model matching");
            return name;
        }
    };
} // ns

#endif // guard
