/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SETTINGS_UPDATENOTIFICATIONSETTINGS_H
#define BLACKGUI_SETTINGS_UPDATENOTIFICATIONSETTINGS_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/settingscache.h"

namespace BlackGui
{
    namespace Settings
    {
        //! Trait for directory settings
        struct TUpdateNotificationSettings : public BlackMisc::TSettingTrait<bool>
        {
            //! Key in data cache
            static const char *key() { return "guiupdatenotification/%Application%"; }

            //! Default, not consolidating
            static const bool &defaultValue() { static const bool d = true; return d; }
        };
    } // ns
} // ns

#endif // guard
