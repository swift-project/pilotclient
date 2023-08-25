// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "resourcesound.h"
#include "blacksound/audioutilities.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/stringutils.h"
#include <QCoreApplication>

using namespace BlackMisc;
using namespace BlackSound::Wav;

namespace BlackSound::SampleProvider
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
        return m_data ? m_data->fileName : empty;
    }

    bool CResourceSound::isSameFileName(const QString &fn) const
    {
        if (fn.isEmpty()) { return false; }
        return stringCompare(fn, m_data->fileName, CFileUtils::osFileNameCaseSensitivity());
    }
} // ns
