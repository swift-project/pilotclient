// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
