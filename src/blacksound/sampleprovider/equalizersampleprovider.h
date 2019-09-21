/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSOUND_SAMPLEPROVIDER_EQUALIZERSAMPLEPROVIDER_H
#define BLACKSOUND_SAMPLEPROVIDER_EQUALIZERSAMPLEPROVIDER_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/sampleprovider/sampleprovider.h"
#include "blacksound/dsp/biquadfilter.h"

#include <QSharedPointer>
#include <QVector>

namespace BlackSound
{
    namespace SampleProvider
    {
        //! Equalizer
        enum EqualizerPresets
        {
            VHFEmulation = 1
        };

        class BLACKSOUND_EXPORT CEqualizerSampleProvider : public ISampleProvider
        {
            Q_OBJECT

        public:
            CEqualizerSampleProvider(ISampleProvider *sourceProvider, EqualizerPresets preset, QObject *parent = nullptr);

            virtual int readSamples(QVector<qint16> &samples, qint64 count) override;

            void setBypassEffects(bool value) { m_bypass = value; }

            double outputGain() const;
            void setOutputGain(double outputGain);

        private:
            void setupPreset(EqualizerPresets preset);

            ISampleProvider *m_sourceProvider;

            int    m_channels = 1;
            bool   m_bypass = false;
            double m_outputGain = 1.0;
            QVector<BiQuadFilter> m_filters;
        };
    } // ns
} // ns

#endif // guard
