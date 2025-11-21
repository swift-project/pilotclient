// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_APPLICATION_APPLICATIONSETTINGS_H
#define SWIFT_CORE_APPLICATION_APPLICATIONSETTINGS_H

#include <QStringList>

#include "misc/input/actionhotkeylist.h"
#include "misc/settingscache.h"
#include "misc/simulation/simulatorplugininfo.h"

namespace swift::core::application
{
    //! User configured hotkeys
    struct TActionHotkeys : misc::TSettingTrait<misc::input::CActionHotkeyList>
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
        static bool isValid(const misc::input::CActionHotkeyList &value, QString &)
        {
            return std::all_of(value.cbegin(), value.cend(), [](const auto &actionHotkey) {
                return !actionHotkey.getApplicableMachine().getMachineName().isEmpty() &&
                       !actionHotkey.getAction().isEmpty() && !actionHotkey.getCombination().isEmpty();
            });
        }
    };

    //! Selected simulator plugins
    struct TEnabledSimulators : misc::TSettingTrait<QStringList>
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
            static const QStringList enabledSimulators(misc::simulation::CSimulatorPluginInfo::guessDefaultPlugins());
            return enabledSimulators;
        }

        //! \copydoc swift::misc::TSettingTrait::isValid
        static bool isValid(const QStringList &pluginIdentifiers, QString &)
        {
            bool allPositiveNonEmpty = true;
            return (
                allPositiveNonEmpty =
                    !pluginIdentifiers.empty() &&
                    std::all_of(pluginIdentifiers.cbegin(), pluginIdentifiers.cend(), [](const auto &pluginIdentifier) {
                        return misc::simulation::CSimulatorPluginInfo::allIdentifiers().contains(pluginIdentifier);
                    }));
        }
    };

    //! Uploading of crash dumps is enabled or disabled
    struct TCrashDumpUploadEnabled : misc::TSettingTrait<bool>
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

    //! Selected configuration options
    struct TEnabledConfigOptions : misc::TSettingTrait<QStringList>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "application/enabledconfigoptions"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Config Settings");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const QStringList &defaultValue() { return QStringList {}; }
    };
} // namespace swift::core::application

#endif // SWIFT_CORE_APPLICATION_APPLICATIONSETTINGS_H
