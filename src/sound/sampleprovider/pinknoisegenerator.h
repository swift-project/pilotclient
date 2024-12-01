// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef PINKNOISEGENERATOR_H
#define PINKNOISEGENERATOR_H

#include <array>

#include <QRandomGenerator>
#include <QVector>

#include "sound/sampleprovider/sampleprovider.h"
#include "sound/swiftsoundexport.h"

namespace swift::sound::sample_provider
{
    //! Pink noise generator
    class SWIFT_SOUND_EXPORT CPinkNoiseGenerator : public ISampleProvider
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
} // namespace swift::sound::sample_provider

#endif // PINKNOISEGENERATOR_H
