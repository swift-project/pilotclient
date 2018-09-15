/* Copyright (C) 2018
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SETTINGS_INTERPOLATIONRENDERINGSETUP_H
#define BLACKMISC_SIMULATION_SETTINGS_INTERPOLATIONRENDERINGSETUP_H

#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/settingscache.h"

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Settings
        {
            //! Last model used
            struct TInterpolationAndRenderingSetupGlobal : public TSettingTrait<CInterpolationAndRenderingSetupGlobal>
            {
                //! \copydoc BlackCore::TSettingTrait::key
                static const char *key() { return "settingsglobalinterpolationsetup"; }

                //! \copydoc BlackCore::TSettingTrait::humanReadable
                static const QString &humanReadable() { static const QString name("Interpolation setup"); return name; }
            };
        } // ns
    } // ns
} // ns

#endif // guard
