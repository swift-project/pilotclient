// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSOUND_SAMPLEPROVIDER_SAWTOOTHGENERATOR_H
#define BLACKSOUND_SAMPLEPROVIDER_SAWTOOTHGENERATOR_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/sampleprovider/sampleprovider.h"
#include <QRandomGenerator>
#include <QVector>
#include <array>

namespace BlackSound::SampleProvider
{
    //! Saw tooth generator
    class BLACKSOUND_EXPORT CSawToothGenerator : public ISampleProvider
    {
        Q_OBJECT

    public:
        //! Ctor
        CSawToothGenerator(double frequency, QObject *parent = nullptr);

        //! \copydoc ISampleProvider::readSamples
        virtual int readSamples(QVector<float> &samples, qint64 count) override;

        //! Set the gain
        void setGain(double gain) { m_gain = gain; }

    private:
        double m_gain = 0.0;
        double m_frequency = 0.0;
        double m_sampleRate = 48000;
        int m_nSample = 0;
    };
} // ns

#endif // guard
