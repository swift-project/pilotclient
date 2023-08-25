// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSOUND_SAMPLEPROVIDER_SINUSGENERATOR_H
#define BLACKSOUND_SAMPLEPROVIDER_SINUSGENERATOR_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/sampleprovider/sampleprovider.h"

#include <QtMath>

namespace BlackSound::SampleProvider
{
    //! Saw tooth generator
    class BLACKSOUND_EXPORT CSinusGenerator : public ISampleProvider
    {
        Q_OBJECT

    public:
        //! Ctor
        CSinusGenerator(double frequencyHz, QObject *parent = nullptr);

        //! \copydoc ISampleProvider::readSamples
        virtual int readSamples(QVector<float> &samples, qint64 count) override;

        //! Set the gain
        void setGain(double gain) { m_gain = gain; }

        //! Set frequency in Hz
        void setFrequency(double frequencyHz);

    private:
        double m_gain = 0.0;
        double m_frequencyHz = 0.0;
        double m_sampleRate = 48000;
        int m_nSample = 0;
        static constexpr double s_twoPi = 2 * M_PI;
    };
} // ns

#endif // guard
