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
#include "blackmisc/fileutils.h"
#include "blackmisc/stringutils.h"

using namespace BlackMisc;
using namespace BlackSound::Wav;

namespace BlackSound
{
    namespace SampleProvider
    {
        CResourceSound::CResourceSound(const QString &audioFileName)
        {
            CWavFile wavFile;
            m_fn.clear();
            m_samples.clear();
            if (wavFile.open(audioFileName))
            {
                if (wavFile.fileFormat().sampleType() == QAudioFormat::Float)
                {
                    // Not implemented
                    // m_samples = convertFloatBytesTo16BitPCM(wavFile.audioData());
                }
                else
                {
                    m_samples = convertBytesTo32BitFloatPCM(wavFile.audioData());
                }
                m_fn = audioFileName;
            }
        }

        bool CResourceSound::isSameFileName(const QString &fn) const
        {
            if (fn.isEmpty()) { return false; }
            return stringCompare(fn, m_fn, CFileUtils::osFileNameCaseSensitivity());
        }
    } // ns
} // ns
