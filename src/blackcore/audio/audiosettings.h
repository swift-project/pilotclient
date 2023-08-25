// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_AUDIO_SETTINGS_H
#define BLACKCORE_AUDIO_SETTINGS_H

#include "blackmisc/settingscache.h"
#include <QString>

namespace BlackCore::Audio
{
    //! Audio input device settings
    struct TInputDevice : public BlackMisc::TSettingTrait<QString>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "audio/%Application%/inputdevice"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Input device");
            return name;
        }

        //! \copydoc BlackMisc::TSettingTrait::defaultValue
        static const QString &defaultValue()
        {
            static const QString device("default");
            return device;
        }
    };

    //! Audio input device settings
    struct TOutputDevice : public BlackMisc::TSettingTrait<QString>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "audio/%Application%/outputdevice"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Output device");
            return name;
        }

        //! \copydoc BlackMisc::TSettingTrait::defaultValue
        static const QString &defaultValue()
        {
            static const QString device("default");
            return device;
        }
    };
} // ns

#endif
