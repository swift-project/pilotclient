// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_SETTINGS_INTERPOLATIONRENDERINGSETUP_H
#define BLACKMISC_SIMULATION_SETTINGS_INTERPOLATIONRENDERINGSETUP_H

#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/settingscache.h"

namespace BlackMisc::Simulation::Settings
{
    //! Last model used
    struct TInterpolationAndRenderingSetupGlobal : public TSettingTrait<CInterpolationAndRenderingSetupGlobal>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "settingsglobalinterpolationsetup"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Interpolation setup");
            return name;
        }
    };
} // ns

#endif // guard
