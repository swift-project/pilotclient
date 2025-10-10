// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_SAMPLEPROVIDER_SIMPLECOMPRESSOREFFECT_H
#define SWIFT_SOUND_SAMPLEPROVIDER_SIMPLECOMPRESSOREFFECT_H

#include <QObject>
#include <QTimer>

#include "chunkware_dsp/SimpleComp.h"

#include "sound/sampleprovider/sampleprovider.h"
#include "sound/swiftsoundexport.h"

namespace swift::sound::sample_provider
{
    //! Compressor effect
    class SWIFT_SOUND_EXPORT CSimpleCompressorEffect : public ISampleProvider
    {
        Q_OBJECT

    public:
        //! Ctor
        CSimpleCompressorEffect(ISampleProvider *source, QObject *parent = nullptr);

        //! \copydoc ISampleProvider::readSamples
        virtual int readSamples(QVector<float> &samples, qint64 count) override;

        //! Enable
        void setEnabled(bool enabled);

        //! Set gain
        void setMakeUpGain(double gain);

        //! Set channels 1 or 2
        void setChannels(int channels);

    private:
        QTimer *m_timer = nullptr;
        ISampleProvider *m_sourceStream = nullptr;
        bool m_enabled = true;
        int m_channels = 1;
        chunkware_simple::SimpleComp m_simpleCompressor;
    };
} // namespace swift::sound::sample_provider

#endif // SWIFT_SOUND_SAMPLEPROVIDER_SIMPLECOMPRESSOREFFECT_H
