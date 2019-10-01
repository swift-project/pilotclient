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

namespace BlackSound
{
    namespace SampleProvider
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

        private:
            QTimer m_timer;
            ISampleProvider *m_sourceStream = nullptr;
            bool m_enabled = true;
            chunkware_simple::SimpleComp m_simpleCompressor;
            const int channels = 1;
        };
    } // ns
} // ns

#endif // guard
