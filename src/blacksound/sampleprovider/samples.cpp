/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "samples.h"
#include "blackmisc/directoryutils.h"

namespace BlackSound
{
    namespace SampleProvider
    {
        Samples &Samples::instance()
        {
            static Samples samples;
            return samples;
        }

        Samples::Samples() :
            m_crackle(BlackMisc::CDirectoryUtils::soundFilesDirectory() + "/Crackle_f32.wav"),
            m_click(BlackMisc::CDirectoryUtils::soundFilesDirectory() + "/Click_f32.wav"),
            m_whiteNoise(BlackMisc::CDirectoryUtils::soundFilesDirectory() + "/WhiteNoise_f32.wav")
        { }

        CResourceSound Samples::click() const
        {
            return m_click;
        }

        CResourceSound Samples::crackle() const
        {
            return m_crackle;
        }

        CResourceSound Samples::whiteNoise() const
        {
            return m_whiteNoise;
        }
    }
}
