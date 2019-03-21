/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AUDIO_SETTINGS_VOICESETTINGS_H
#define BLACKMISC_AUDIO_SETTINGS_VOICESETTINGS_H

#include "blackmisc/audio/voicesetup.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/settingscache.h"

namespace BlackMisc
{
    namespace Audio
    {
        namespace Settings
        {
            //! Voice settings
            struct TVoiceSetup : public TSettingTrait<CVoiceSetup>
            {
                //! \copydoc BlackMisc::TSettingTrait::key
                static const char *key() { return "audio/currentvoicesetup"; }

                //! \copydoc BlackMisc::TSettingTrait::humanReadable
                static const QString &humanReadable() { static const QString name("Voice setup"); return name; }

                //! \copydoc BlackMisc::TSettingTrait::isValid
                static bool isValid(const CVoiceSetup &setup) { return setup.validate().isSuccess(); }
            };
        } // ns
    } // ns
} // ns

#endif
