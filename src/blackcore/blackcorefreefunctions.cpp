/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcorefreefunctions.h"
#include "blackcore/webreaderflags.h"
#include "blackcore/data/globalsetup.h"

#include "voice_channel.h"
#include "network.h"
#include "simulator.h"
#include "context_application.h"
#include <QThread>

namespace BlackCore
{
    void registerMetadata()
    {
        qDBusRegisterMetaType<BlackCore::IVoiceChannel::ConnectionStatus>();
        qDBusRegisterMetaType<BlackCore::INetwork::ConnectionStatus>();
        qDBusRegisterMetaType<BlackCore::INetwork::LoginMode>();
        qRegisterMetaType<BlackCore::CWebReaderFlags::WebReaderFlag>();
        qRegisterMetaType<BlackCore::CWebReaderFlags::WebReader>();
        qDBusRegisterMetaType<BlackCore::CLogSubscriptionHash>();
        qDBusRegisterMetaType<BlackCore::CLogSubscriptionPair>();

        BlackCore::Data::CGlobalSetup::registerMetadata();
    }
} // namespace
