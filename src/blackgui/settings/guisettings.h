/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SETTINGS_GUI_H
#define BLACKGUI_SETTINGS_GUI_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/simulation/aircraftmodel.h"

namespace BlackGui
{
    namespace Settings
    {
        //! Settings for last manual entries of own aircraft mode
        struct TOwnAircraftModel : public BlackMisc::TSettingTrait<BlackMisc::Simulation::CAircraftModel>
        {
            //! Key in data cache
            static const char *key() { return "guinownaircraftmodel"; }
        };
    } // ns
} // ns

#endif // guard
