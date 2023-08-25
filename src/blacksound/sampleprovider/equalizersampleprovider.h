// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSOUND_SAMPLEPROVIDER_EQUALIZERSAMPLEPROVIDER_H
#define BLACKSOUND_SAMPLEPROVIDER_EQUALIZERSAMPLEPROVIDER_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/sampleprovider/sampleprovider.h"
#include "blacksound/dsp/biquadfilter.h"

#include <QSharedPointer>
#include <QVector>

namespace BlackSound::SampleProvider
{
    //! Equalizer
    enum EqualizerPresets
    {
        VHFEmulation = 1
    };

    //! Equalizer
    class BLACKSOUND_EXPORT CEqualizerSampleProvider : public ISampleProvider
    {
        Q_OBJECT

    public:
        //! Ctor
        CEqualizerSampleProvider(ISampleProvider *sourceProvider, EqualizerPresets preset, QObject *parent = nullptr);

        //! \copydoc ISampleProvider::readSamples
        virtual int readSamples(QVector<float> &samples, qint64 count) override;

        //! Bypassing?
        void setBypassEffects(bool value) { m_bypass = value; }

        //! @{
        //! Gain
        double outputGain() const;
        void setOutputGain(double outputGain);
        //! @}

    private:
        void setupPreset(EqualizerPresets preset);

        ISampleProvider *m_sourceProvider = nullptr;
        int m_channels = 1;
        bool m_bypass = false;
        double m_outputGain = 1.0;
        QVector<Dsp::BiQuadFilter> m_filters;
    };
} // ns

#endif // guard
