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
#include "sampleprovider.h"
#include "resourcesound.h"

namespace BlackSound
{
    namespace SampleProvider
    {
        //! A sample provider
        class BLACKSOUND_EXPORT CResourceSoundSampleProvider : public ISampleProvider
        {
            Q_OBJECT

        public:
            //! Ctor
            CResourceSoundSampleProvider(const CResourceSound &resourceSound, QObject *parent = nullptr);

            virtual int readSamples(QVector<qint16> &samples, qint64 count) override;
            virtual bool isFinished() const override { return m_isFinished; }

            bool looping() const { return m_looping; }
            void setLooping(bool looping) { m_looping = looping; }

            double gain() const { return m_gain; }
            void setGain(double gain) { m_gain = gain; }

            QVector<qint16> getTempBuffer() const { return m_tempBuffer; }
            void setTempBuffer(const QVector<qint16> &value) { m_tempBuffer = value; }

        private:
            double m_gain = 1.0;
            bool m_looping = false;

            CResourceSound  m_resourceSound;
            qint64          m_position = 0;
            const int       m_tempBufferSize = 9600; //9600 = 200ms
            QVector<qint16> m_tempBuffer;
            bool            m_isFinished = false;
        };
    }
}

#endif // guard
