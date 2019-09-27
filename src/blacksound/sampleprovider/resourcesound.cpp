/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "resourcesound.h"
#include "audioutilities.h"

using namespace BlackSound::Wav;

namespace BlackSound
{
    namespace SampleProvider
    {
        CResourceSound::CResourceSound(const QString &audioFileName)
        {
            m_wavFile = new CWavFile();
            m_wavFile->open(audioFileName);
            if (m_wavFile->fileFormat().sampleType() == QAudioFormat::Float)
            {
                m_samples = convertFloatBytesTo16BitPCM(m_wavFile->audioData());
            }
            else
            {
                m_samples = convertBytesTo16BitPCM(m_wavFile->audioData());
            }
        }
    } // ns
} // ns
