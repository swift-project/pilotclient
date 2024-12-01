// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_SAMPLEPROVIDER_SAMPLES_H
#define SWIFT_SOUND_SAMPLEPROVIDER_SAMPLES_H

#include <QObject>

#include "misc/audio/audiosettings.h"
#include "misc/settingscache.h"
#include "sound/sampleprovider/resourcesound.h"
#include "sound/swiftsoundexport.h"

namespace swift::sound::sample_provider
{
    //! Sound samples from resources (wav files)
    class SWIFT_SOUND_EXPORT Samples : public QObject
    {
    public:
        //! Singleton
        static const Samples &instance();

        //! Avoid to copy
        Samples(const Samples &) = delete;

        //! @{
        //! Various samples (sounds)
        const CResourceSound &crackle() const { return m_crackle; }
        const CResourceSound &click() const { return m_click; }
        const CResourceSound &whiteNoise() const { return m_whiteNoise; }
        const CResourceSound &hfWhiteNoise() const { return m_hfWhiteNoise; }
        //! @}

        //! Play the click sound
        bool playClick() const { return m_audioSettings.get().pttClickUp(); }

        //! @{
        //! File names
        static const QString &fnCrackle()
        {
            static const QString f = "afv_crackle_f32.wav";
            return f;
        }
        static const QString &fnClick()
        {
            static const QString f = "afv_click_f32.wav";
            return f;
        }
        static const QString &fnWhiteNoise()
        {
            static const QString f = "afv_whitenoise_f32.wav";
            return f;
        }
        static const QString &fnHfWhiteNoise()
        {
            static const QString f = "afv_hf_whiteNoise_f32.wav";
            return f;
        }
        //! @}

    private:
        //! Ctor
        Samples();

        CResourceSound m_crackle;
        CResourceSound m_click;
        CResourceSound m_whiteNoise;
        CResourceSound m_hfWhiteNoise;

        swift::misc::CSetting<swift::misc::audio::TSettings> m_audioSettings { this, &Samples::onSettingsChanged };

        //! Init sounds
        void initSounds();

        //! Settings have been changed
        void onSettingsChanged();
    };

} // namespace swift::sound::sample_provider

#endif // SWIFT_SOUND_SAMPLEPROVIDER_SAMPLES_H
