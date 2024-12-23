// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/audio/audioutils.h"

#include <QAudioDevice>
#include <QMediaDevices>
#include <QProcess>

#include "config/buildconfig.h"

using namespace swift::config;

namespace swift::misc::audio
{
    bool startWindowsMixer()
    {
        if (!CBuildConfig::isRunningOnWindowsNtPlatform()) { return false; }
        return QProcess::startDetached("SndVol.exe");
    }

} // namespace swift::misc::audio
