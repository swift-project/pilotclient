// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_AUDIO_SETTINGS_H
#define SWIFT_CORE_AUDIO_SETTINGS_H

#include <QString>

#include "misc/settingscache.h"

namespace swift::core::audio
{
    //! Audio input device settings
    struct TInputDevice : misc::TSettingTrait<QString>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "audio/%Application%/inputdevice"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Input device");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const QString &defaultValue()
        {
            static const QString device("default");
            return device;
        }
    };

    //! Audio input device settings
    struct TOutputDevice : misc::TSettingTrait<QString>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "audio/%Application%/outputdevice"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Output device");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const QString &defaultValue()
        {
            static const QString device("default");
            return device;
        }
    };
} // namespace swift::core::audio

#endif // SWIFT_CORE_AUDIO_SETTINGS_H
