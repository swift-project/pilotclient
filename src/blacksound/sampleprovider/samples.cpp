// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "samples.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/fileutils.h"

using namespace BlackMisc;
using namespace BlackMisc::Audio;

namespace BlackSound::SampleProvider
{
    const Samples &Samples::instance()
    {
        static const Samples samples;
        return samples;
    }

    Samples::Samples()
    {
        this->initSounds();
    }

    void Samples::initSounds()
    {
        const CSettings settings = m_audioSettings.get();
        QString f = settings.getNotificationFilePath(fnCrackle());
        if (!m_crackle.isSameFileName(f))
        {
            m_crackle = CResourceSound(f);
            m_crackle.load();
        }

        f = settings.getNotificationFilePath(fnClick());
        if (!m_click.isSameFileName(f))
        {
            m_click = CResourceSound(f);
            m_click.load();
        }

        f = settings.getNotificationFilePath(fnWhiteNoise());
        if (!m_whiteNoise.isSameFileName(f))
        {
            m_whiteNoise = CResourceSound(f);
            m_whiteNoise.load();
        }

        f = settings.getNotificationFilePath(fnHfWhiteNoise());
        if (!m_hfWhiteNoise.isSameFileName(f))
        {
            m_hfWhiteNoise = CResourceSound(f);
            m_hfWhiteNoise.load();
        }
    }

    void Samples::onSettingsChanged()
    {
        this->initSounds();
    }
} // ns
