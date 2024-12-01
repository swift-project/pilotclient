// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_SETTINGS_UPDATENOTIFICATIONSETTINGS_H
#define SWIFT_GUI_SETTINGS_UPDATENOTIFICATIONSETTINGS_H

#include "gui/swiftguiexport.h"
#include "misc/settingscache.h"

namespace swift::gui::settings
{
    //! Trait for directory settings
    struct TUpdateNotificationSettings : public swift::misc::TSettingTrait<bool>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "guiupdatenotification/%Application%"; }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const bool &defaultValue()
        {
            static const bool d = true;
            return d;
        }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Update notification");
            return name;
        }
    };
} // namespace swift::gui::settings

#endif // SWIFT_GUI_SETTINGS_UPDATENOTIFICATIONSETTINGS_H
