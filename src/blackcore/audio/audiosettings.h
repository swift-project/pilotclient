/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AUDIO_SETTINGS_H
#define BLACKCORE_AUDIO_SETTINGS_H

#include "blackmisc/settingscache.h"
#include "blackmisc/audio/audiosettings.h"
#include <QString>

namespace BlackCore
{
    namespace Audio
    {
        //! Audio related settings
        struct TSettings : public BlackMisc::TSettingTrait<BlackMisc::Audio::CSettings>
        {
            //! \copydoc BlackMisc::TSettingTrait::key
            static const char *key() { return "audio/setup"; }

            //! \copydoc BlackMisc::TSettingTrait::isValid
            static bool isValid(const BlackMisc::Audio::CSettings &value) { Q_UNUSED(value); return true; }
        };

        //! Audio input device settings
        struct TInputDevice : public BlackMisc::TSettingTrait<QString>
        {
            //! \copydoc BlackMisc::TSettingTrait::key
            static const char *key() { return "audio/inputdevice"; }
        };

        //! Audio input device settings
        struct TOutputDevice : public BlackMisc::TSettingTrait<QString>
        {
            //! \copydoc BlackMisc::TSettingTrait::key
            static const char *key() { return "audio/outputdevice"; }
        };

    } // ns
} // ns

#endif
