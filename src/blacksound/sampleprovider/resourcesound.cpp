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
#include <QCoreApplication>

using namespace BlackMisc;
using namespace BlackSound::Wav;

namespace BlackSound
{
    namespace SampleProvider
    {
        CResourceSound::CResourceSound() : m_data(new CResourceSoundData)
        {
            // void
        }

        CResourceSound::CResourceSound(const QString &audioFileName) : m_data(new CResourceSoundData)
        {
            m_data->fileName = audioFileName;
        }

        bool CResourceSound::load()
        {
            if (!m_data || m_data->fileName.isEmpty()) { return false; }

            CWavFile wavFile;
            m_data->samples.clear();
            if (wavFile.open(m_data->fileName))
            {
                if (wavFile.fileFormat().sampleType() == QAudioFormat::Float)
                {
                    // Not implemented
                    // m_samples = convertFloatBytesTo16BitPCM(wavFile.audioData());
                }
                else
                {
                    m_data->samples = convertBytesTo32BitFloatPCM(wavFile.audioData());
                }
            }

            m_data->isLoaded = true;
            return true;
        }

        const QString &CResourceSound::getFileName() const
        {
            static const QString empty;
            return m_data ? empty : m_data->fileName;
        }

        bool CResourceSound::isSameFileName(const QString &fn) const
        {
            if (fn.isEmpty()) { return false; }
            return stringCompare(fn, m_data->fileName, CFileUtils::osFileNameCaseSensitivity());
        }
    } // ns
} // ns
