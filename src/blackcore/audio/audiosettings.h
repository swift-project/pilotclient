/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
