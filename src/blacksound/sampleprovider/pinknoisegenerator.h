// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef PINKNOISEGENERATOR_H
#define PINKNOISEGENERATOR_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/sampleprovider/sampleprovider.h"

#include <QRandomGenerator>
#include <QVector>

#include <array>

namespace BlackSound::SampleProvider
{
    //! Pink noise generator
    class BLACKSOUND_EXPORT CPinkNoiseGenerator : public ISampleProvider
    {
        Q_OBJECT

    public:
        //! Noise generator
        CPinkNoiseGenerator(QObject *parent = nullptr) : ISampleProvider(parent) {}

        //! Read samples
        virtual int readSamples(QVector<float> &samples, qint64 count) override;

        //! Gain
        void setGain(double gain) { m_gain = gain; }

    private:
        QRandomGenerator m_random;
        std::array<double, 7> m_pinkNoiseBuffer = { { 0 } };
        double m_gain = 0.0;
    };
}

#endif // guard
