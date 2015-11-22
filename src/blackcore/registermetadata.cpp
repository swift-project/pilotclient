/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "registermetadata.h"
#include "blackcore/contextapplication.h"
#include "blackcore/webreaderflags.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/data/updateinfo.h"
#include "voicechannel.h"
#include "network.h"
#include "setupreader.h"

namespace BlackCore
{
    void registerMetadata()
    {
        qDBusRegisterMetaType<BlackCore::CLogSubscriptionHash>();
        qDBusRegisterMetaType<BlackCore::CLogSubscriptionPair>();
        qDBusRegisterMetaType<BlackCore::INetwork::ConnectionStatus>();
        qDBusRegisterMetaType<BlackCore::INetwork::LoginMode>();
        qDBusRegisterMetaType<BlackCore::IVoiceChannel::ConnectionStatus>();
        qRegisterMetaType<BlackCore::CWebReaderFlags::WebReader>();
        qRegisterMetaType<BlackCore::CWebReaderFlags::WebReaderFlag>();

        BlackCore::Data::CGlobalSetup::registerMetadata();
        BlackCore::Data::CUpdateInfo::registerMetadata();
        BlackCore::CSetupReader::instance(); //! \todo will go into new runtime
    }
} // namespace
