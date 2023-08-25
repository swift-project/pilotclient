// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_APPLICATION_APPLICATIONSETTINGS_H
#define BLACKCORE_APPLICATION_APPLICATIONSETTINGS_H

#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/input/actionhotkeylist.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/crashsettings.h"
#include "blackconfig/buildconfig.h"

#include <QStringList>

namespace BlackCore::Application
{
    //! User configured hotkeys
    struct TActionHotkeys : public BlackMisc::TSettingTrait<BlackMisc::Input::CActionHotkeyList>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "application/actionhotkeys"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Hotkeys");
            return name;
        }

        //! \copydoc BlackMisc::TSettingTrait::isValid
        static bool isValid(const BlackMisc::Input::CActionHotkeyList &value, QString &)
        {
            for (const auto &actionHotkey : value)
            {
                if (actionHotkey.getApplicableMachine().getMachineName().isEmpty() ||
                    actionHotkey.getAction().isEmpty() ||
                    actionHotkey.getCombination().isEmpty()) { return false; }
            }
            return true;
        }
    };

    //! Selected simulator plugins
    struct TEnabledSimulators : public BlackMisc::TSettingTrait<QStringList>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "application/enabledsimulators"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Enabled simulators");
            return name;
        }

        //! \copydoc BlackMisc::TSettingTrait::defaultValue
        static const QStringList &defaultValue()
        {
            // All default simulators
            static const QStringList enabledSimulators(BlackMisc::Simulation::CSimulatorPluginInfo::guessDefaultPlugins());
            return enabledSimulators;
        }

        //! \copydoc BlackMisc::TSettingTrait::isValid
        static bool isValid(const QStringList &pluginIdentifiers, QString &)
        {
            for (const QString &pluginIdentifier : pluginIdentifiers)
            {
                if (!BlackMisc::Simulation::CSimulatorPluginInfo::allIdentifiers().contains(pluginIdentifier))
                {
                    return false;
                }
            }
            return true;
        }
    };

    //! Uploading of crash dumps is enabled or disabled
    //! \deprecated remove after changing to
    struct TCrashDumpSettings : public BlackMisc::TSettingTrait<BlackMisc::Settings::CCrashSettings>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "application/crashdump"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Crash dumps");
            return name;
        }

        //! \copydoc BlackMisc::TSettingTrait::defaultValue
        // static bool defaultValue() { return BlackMisc::Settings::CCrashSettings(); }
    };
} // ns

#endif
