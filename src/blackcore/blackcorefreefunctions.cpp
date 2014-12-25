/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcorefreefunctions.h"
#include "voice_channel.h"
#include "simulator.h"
#include <QThread>

namespace BlackCore
{
    void registerMetadata()
    {
        // for some reasons (ask RW) these are registered twice
        qRegisterMetaType<ISimulator::ConnectionStatus>();
        qRegisterMetaType<ISimulator::ConnectionStatus>("Status");
        qRegisterMetaType<BlackCore::IVoiceChannel::ConnectionStatus>();
    }

    bool isCurrentThreadCreatingThread(QObject *toBeTested)
    {
        if (!toBeTested) return false;
        if (!toBeTested->thread()) return false;
        return (QThread::currentThreadId() == toBeTested->thread()->currentThreadId());
    }

} // namespace
