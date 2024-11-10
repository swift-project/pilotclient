// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SOUND_SAMPLEPROVIDER_RESOURCESOUNDSAMPLEPROVIDER_H
#define SWIFT_SOUND_SAMPLEPROVIDER_RESOURCESOUNDSAMPLEPROVIDER_H

#include "sound/swiftsoundexport.h"
#include "sound/sampleprovider/sampleprovider.h"
#include "sound/sampleprovider/resourcesound.h"

namespace swift::sound::sample_provider
{
    //! A sample provider
    class SWIFT_SOUND_EXPORT CResourceSoundSampleProvider : public ISampleProvider
    {
        Q_OBJECT

    public:
        //! Ctor
        CResourceSoundSampleProvider(const CResourceSound &resourceSound, QObject *parent = nullptr);

        //! copydoc ISampleProvider::readSamples
        virtual int readSamples(QVector<float> &samples, qint64 count) override;

        //! copydoc ISampleProvider::isFinished
        virtual bool isFinished() const override { return m_isFinished; }

        //! @{
        //! Looping
        bool looping() const { return m_looping; }
        void setLooping(bool looping) { m_looping = looping; }
        //! @}

        //! @{
        //! Gain
        double gain() const { return m_gain; }
        void setGain(double gain) { m_gain = gain; }
        //! @}

    private:
        double m_gain = 1.0;
        bool m_looping = false;

        CResourceSound m_resourceSound;
        qint64 m_position = 0;
        const int m_tempBufferSize = 24000; // 24000 = 500ms (avoid buffer overflow), m_tempBufferSize = 9600; //9600 = 200ms
        QVector<float> m_tempBuffer;
        bool m_isFinished = false;
    };
} // ns

#endif // guard
