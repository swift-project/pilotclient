/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
