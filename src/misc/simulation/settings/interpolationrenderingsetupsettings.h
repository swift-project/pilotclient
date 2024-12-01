// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_SETTINGS_INTERPOLATIONRENDERINGSETUP_H
#define SWIFT_MISC_SIMULATION_SETTINGS_INTERPOLATIONRENDERINGSETUP_H

#include "misc/settingscache.h"
#include "misc/simulation/interpolation/interpolationrenderingsetup.h"

namespace swift::misc::simulation::settings
{
    //! Last model used
    struct TInterpolationAndRenderingSetupGlobal : public TSettingTrait<CInterpolationAndRenderingSetupGlobal>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "settingsglobalinterpolationsetup"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Interpolation setup");
            return name;
        }
    };
} // namespace swift::misc::simulation::settings

#endif // SWIFT_MISC_SIMULATION_SETTINGS_INTERPOLATIONRENDERINGSETUP_H
