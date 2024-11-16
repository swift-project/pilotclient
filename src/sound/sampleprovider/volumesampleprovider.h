// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_SAMPLEPROVIDER_VOLUMESAMPLEPROVIDER_H
#define SWIFT_SOUND_SAMPLEPROVIDER_VOLUMESAMPLEPROVIDER_H

#include "sound/sampleprovider/sampleprovider.h"
#include "sound/swiftsoundexport.h"

namespace swift::sound::sample_provider
{
    //! Pink noise generator
    class SWIFT_SOUND_EXPORT CVolumeSampleProvider : public ISampleProvider
    {
        Q_OBJECT

    public:
        //! Noise generator
        CVolumeSampleProvider(ISampleProvider *sourceProvider, QObject *parent = nullptr);

        //! \copydoc ISampleProvider::readSamples
        virtual int readSamples(QVector<float> &samples, qint64 count) override;

        //! @{
        //! Gain ratio, value a amplitude need to be multiplied with
        //! \see http://www.sengpielaudio.com/calculator-amplification.htm
        //! \remark gain ratio is voltage ratio/or amplitude ratio, something between 0.001-7.95 for -60dB to 80dB
        double getGainRatio() const { return m_gainRatio; }
        bool setGainRatio(double gainRatio);
        //! @}
        // those used to be the original function names
        // double volume() const { return m_volume; }
        // bool setVolume(double volume);

    private:
        ISampleProvider *m_sourceProvider = nullptr;
        double m_gainRatio = 1.0;
    };
} // namespace swift::sound::sample_provider

#endif // guard
