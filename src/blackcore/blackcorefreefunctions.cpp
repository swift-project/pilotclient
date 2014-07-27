/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcorefreefunctions.h"
#include "voice.h"
#include "simulator.h"
#include <QThread>

namespace BlackCore
{
    void registerMetadata()
    {
        // for some reasons (ask RW) these are registered twice
        qRegisterMetaType<ISimulator::Status>();
        qRegisterMetaType<ISimulator::Status>("Status");
        qRegisterMetaType<IVoice::ComUnit>();
        qRegisterMetaType<IVoice::ComUnit>("ComUnit");
        qRegisterMetaType<IVoice::ConnectionStatus>();
        qRegisterMetaType<IVoice::ConnectionStatus>("ConnectionStatus");
    }

    bool isCurrentThreadCreatingThread(QObject *toBeTested)
    {
        if (!toBeTested) return false;
        if (!toBeTested->thread()) return false;
        return (QThread::currentThreadId() == toBeTested->thread()->currentThreadId());
    }

} // namespace
