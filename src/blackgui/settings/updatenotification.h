/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
            //! \copydoc BlackCore::TSettingTrait::key
            static const char *key() { return "guiupdatenotification/%Application%"; }

            //! \copydoc BlackCore::TSettingTrait::defaultValue
            static const bool &defaultValue() { static const bool d = true; return d; }

            //! \copydoc BlackCore::TSettingTrait::humanReadable
            static const QString &humanReadable() { static const QString name("Update notification"); return name; }
        };
    } // ns
} // ns

#endif // guard
