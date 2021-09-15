/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSOUND_SAMPLEPROVIDER_SIMPLECOMPRESSOREFFECT_H
#define BLACKSOUND_SAMPLEPROVIDER_SIMPLECOMPRESSOREFFECT_H

#include "blacksound/blacksoundexport.h"
#include "sampleprovider.h"
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
        QTimer          *m_timer = nullptr;
        ISampleProvider *m_sourceStream = nullptr;
        bool             m_enabled = true;
        int              m_channels = 1;
        chunkware_simple::SimpleComp m_simpleCompressor;
    };
} // ns

#endif // guard
