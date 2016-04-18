/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_SETTINGS_DISTRIBUTORPREFERENCES_H
#define BLACKCORE_SETTINGS_DISTRIBUTORPREFERENCES_H

#include "blackmisc/settingscache.h"
#include "blackmisc/simulation/distributorlistpreferences.h"

namespace BlackCore
{
    namespace Settings
    {
        namespace Simulation
        {
            //! DBus server address
            struct DistributorListPreferences : public BlackMisc::CSettingTrait<BlackMisc::Simulation::CDistributorListPreferences>
            {
                //! \copydoc BlackMisc::CSettingTrait::key
                static const char *key() { return "mapping/distributorpreferences"; }
            };
        } // ns
    } // ns
} // ns

#endif
