/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackmisc/audio/audioutils.h"

#include <QProcess>
#include <QAudioDeviceInfo>

using namespace BlackConfig;

namespace BlackMisc::Audio
{
    bool startWindowsMixer()
    {
        if (!CBuildConfig::isRunningOnWindowsNtPlatform()) { return false; }
        return QProcess::startDetached("SndVol.exe");
    }

    void initWindowsAudioDevices()
    {
        if (!CBuildConfig::isRunningOnWindowsNtPlatform()) { return; }

        // force init of device lists
        // see https://discordapp.com/channels/539048679160676382/539925070550794240/676418182038421534
        QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
        QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    }

} // ns
