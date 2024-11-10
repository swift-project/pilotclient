// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_AUDIO_SETTINGS_H
#define BLACKCORE_AUDIO_SETTINGS_H

#include "misc/settingscache.h"
#include <QString>

namespace BlackCore::Audio
{
    //! Audio input device settings
    struct TInputDevice : public swift::misc::TSettingTrait<QString>
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
    struct TOutputDevice : public swift::misc::TSettingTrait<QString>
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
} // ns

#endif
