// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_APPLICATION_APPLICATIONSETTINGS_H
#define SWIFT_CORE_APPLICATION_APPLICATIONSETTINGS_H

#include "misc/simulation/simulatorplugininfo.h"
#include "misc/input/actionhotkeylist.h"
#include "misc/settingscache.h"
#include "config/buildconfig.h"

#include <QStringList>

namespace swift::core::application
{
    //! User configured hotkeys
    struct TActionHotkeys : public swift::misc::TSettingTrait<swift::misc::input::CActionHotkeyList>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "application/actionhotkeys"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Hotkeys");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::isValid
        static bool isValid(const swift::misc::input::CActionHotkeyList &value, QString &)
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
    struct TEnabledSimulators : public swift::misc::TSettingTrait<QStringList>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "application/enabledsimulators"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Enabled simulators");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const QStringList &defaultValue()
        {
            // All default simulators
            static const QStringList enabledSimulators(swift::misc::simulation::CSimulatorPluginInfo::guessDefaultPlugins());
            return enabledSimulators;
        }

        //! \copydoc swift::misc::TSettingTrait::isValid
        static bool isValid(const QStringList &pluginIdentifiers, QString &)
        {
            for (const QString &pluginIdentifier : pluginIdentifiers)
            {
                if (!swift::misc::simulation::CSimulatorPluginInfo::allIdentifiers().contains(pluginIdentifier))
                {
                    return false;
                }
            }
            return true;
        }
    };

    //! Uploading of crash dumps is enabled or disabled
    struct TCrashDumpUploadEnabled : public swift::misc::TSettingTrait<bool>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "application/crashdumpuploadenabled"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Crash dumps");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static bool defaultValue() { return false; }
    };
} // ns

#endif
