/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_SETTINGS_AUDIO_H
#define BLACKCORE_SETTINGS_AUDIO_H

#include "blackmisc/settingscache.h"
#include "blackmisc/audio/settings/settingsaudio.h"

namespace BlackCore
{
    namespace Settings
    {
        namespace Audio
        {
            //! Name of text codec to use with text in FSD protocol
            struct AudioSettings : public BlackMisc::CSettingTrait<BlackMisc::Audio::Settings::CSettingsAudio>
            {
                //! \copydoc BlackMisc::CSettingTrait::key
                static const char *key() { return "audio/setup"; }

                //! \copydoc BlackMisc::CSettingTrait::defaultValue
                static const BlackMisc::Audio::Settings::CSettingsAudio &defaultValue() { static const BlackMisc::Audio::Settings::CSettingsAudio a; return a; }

                //! \copydoc BlackMisc::CSettingTrait::isValid
                static bool isValid(const BlackMisc::Audio::Settings::CSettingsAudio &value) { Q_UNUSED(value); return true; }
            };

        } // ns
    } // ns
} // ns

#endif
