/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKSOUND_SAMPLEPROVIDER_RESOURCESOUNDSAMPLEPROVIDER_H
#define BLACKSOUND_SAMPLEPROVIDER_RESOURCESOUNDSAMPLEPROVIDER_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/sampleprovider/sampleprovider.h"
#include "blacksound/sampleprovider/resourcesound.h"

namespace BlackSound::SampleProvider
{
    //! A sample provider
    class BLACKSOUND_EXPORT CResourceSoundSampleProvider : public ISampleProvider
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
        double m_gain    = 1.0;
        bool   m_looping = false;

        CResourceSound  m_resourceSound;
        qint64          m_position = 0;
        const int       m_tempBufferSize = 24000; //24000 = 500ms (avoid buffer overflow), m_tempBufferSize = 9600; //9600 = 200ms
        QVector<float>  m_tempBuffer;
        bool            m_isFinished = false;
    };
} // ns

#endif // guard
