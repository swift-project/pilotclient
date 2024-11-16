// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_SAMPLEPROVIDER_SAWTOOTHGENERATOR_H
#define SWIFT_SOUND_SAMPLEPROVIDER_SAWTOOTHGENERATOR_H

#include <array>

#include <QRandomGenerator>
#include <QVector>

#include "sound/sampleprovider/sampleprovider.h"
#include "sound/swiftsoundexport.h"

namespace swift::sound::sample_provider
{
    //! Saw tooth generator
    class SWIFT_SOUND_EXPORT CSawToothGenerator : public ISampleProvider
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
} // namespace swift::sound::sample_provider

#endif // guard
