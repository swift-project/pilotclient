// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_SAMPLEPROVIDER_EQUALIZERSAMPLEPROVIDER_H
#define SWIFT_SOUND_SAMPLEPROVIDER_EQUALIZERSAMPLEPROVIDER_H

#include <QSharedPointer>
#include <QVector>

#include "sound/dsp/biquadfilter.h"
#include "sound/sampleprovider/sampleprovider.h"
#include "sound/swiftsoundexport.h"

namespace swift::sound::sample_provider
{
    //! Equalizer
    enum EqualizerPresets
    {
        VHFEmulation = 1
    };

    //! Equalizer
    class SWIFT_SOUND_EXPORT CEqualizerSampleProvider : public ISampleProvider
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
        QVector<dsp::BiQuadFilter> m_filters;
    };
} // namespace swift::sound::sample_provider

#endif // SWIFT_SOUND_SAMPLEPROVIDER_EQUALIZERSAMPLEPROVIDER_H
