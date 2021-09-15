/* Copyright (C) 2018
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
        static const QString &humanReadable() { static const QString name("Model matching"); return name; }
    };
} // ns

#endif // guard
