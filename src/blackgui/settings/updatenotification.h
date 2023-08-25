// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_SETTINGS_UPDATENOTIFICATIONSETTINGS_H
#define BLACKGUI_SETTINGS_UPDATENOTIFICATIONSETTINGS_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/settingscache.h"

namespace BlackGui::Settings
{
    //! Trait for directory settings
    struct TUpdateNotificationSettings : public BlackMisc::TSettingTrait<bool>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "guiupdatenotification/%Application%"; }

        //! \copydoc BlackMisc::TSettingTrait::defaultValue
        static const bool &defaultValue()
        {
            static const bool d = true;
            return d;
        }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Update notification");
            return name;
        }
    };
} // ns

#endif // guard
