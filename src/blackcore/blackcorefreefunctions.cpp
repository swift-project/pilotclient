/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcorefreefunctions.h"
#include "voice_channel.h"
#include "network.h"
#include "simulator.h"
#include <QThread>

namespace BlackCore
{
    void registerMetadata()
    {
        qRegisterMetaType<BlackCore::IVoiceChannel::ConnectionStatus>();
        qRegisterMetaType<BlackCore::INetwork::ConnectionStatus>();
    }

    bool isCurrentThreadObjectThread(QObject *toBeTested)
    {
        Q_ASSERT_X(toBeTested, Q_FUNC_INFO, "missing QObject");
        if (!toBeTested) { return false; }
        if (!toBeTested->thread()) { return false; }
        return (QThread::currentThread() == toBeTested->thread());
    }

    bool isApplicationThreadObjectThread(QObject *toBeTested)
    {
        Q_ASSERT_X(toBeTested, Q_FUNC_INFO, "missing QObject");
        if (!toBeTested) { return false; }
        if (!toBeTested->thread()) { return false; }
        return (QCoreApplication::instance()->thread() == toBeTested->thread());
    }

    bool isCurrentThreadApplicationThread()
    {
        return (QCoreApplication::instance()->thread() == QThread::currentThread());
    }

} // namespace
