/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSOUND_SAMPLEPROVIDER_SAMPLES_H
#define BLACKSOUND_SAMPLEPROVIDER_SAMPLES_H

#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/settingscache.h"
#include "blacksound/blacksoundexport.h"
#include "resourcesound.h"

#include <QObject>

namespace BlackSound::SampleProvider
{
    //! Sound samples from resources (wav files)
    class BLACKSOUND_EXPORT Samples : public QObject
    {
    public:
        //! Singleton
        static const Samples &instance();

        //! Avoid to copy
        Samples(const Samples &) = delete;

        //! Various samples (sounds)
        //! @{
        const CResourceSound &crackle()    const { return m_crackle; }
        const CResourceSound &click()      const { return m_click; }
        const CResourceSound &whiteNoise() const { return m_whiteNoise; }
        const CResourceSound &hfWhiteNoise() const { return m_hfWhiteNoise; }
        //! @}

        //! Play the click sound
        bool playClick() const { return m_audioSettings.get().pttClickUp(); }

        //! File names
        //! @{
        static const QString &fnCrackle()    { static const QString f = "afv_crackle_f32.wav"; return f; }
        static const QString &fnClick()      { static const QString f = "afv_click_f32.wav"; return f; }
        static const QString &fnWhiteNoise() { static const QString f = "afv_whitenoise_f32.wav"; return f; }
        static const QString &fnHfWhiteNoise() { static const QString f = "afv_hf_whiteNoise_f32.wav"; return f; }
        //! @}

    private:
        //! Ctor
        Samples();

        CResourceSound m_crackle;
        CResourceSound m_click;
        CResourceSound m_whiteNoise;
        CResourceSound m_hfWhiteNoise;

        BlackMisc::CSetting<BlackMisc::Audio::TSettings> m_audioSettings { this, &Samples::onSettingsChanged };

        //! Init sounds
        void initSounds();

        //! Settings have been changed
        void onSettingsChanged();
    };

} // ns

#endif // guard
