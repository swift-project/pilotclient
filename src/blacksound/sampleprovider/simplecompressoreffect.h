// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSOUND_SAMPLEPROVIDER_SIMPLECOMPRESSOREFFECT_H
#define BLACKSOUND_SAMPLEPROVIDER_SIMPLECOMPRESSOREFFECT_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/sampleprovider/sampleprovider.h"
#include "blacksound/dsp/SimpleComp.h"

#include <QObject>
#include <QTimer>

namespace BlackSound::SampleProvider
{
    //! Compressor effect
    class BLACKSOUND_EXPORT CSimpleCompressorEffect : public ISampleProvider
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
} // ns

#endif // guard
