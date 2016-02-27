/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "registermetadata.h"
#include "blackcore/contextapplication.h"
#include "blackcore/webreaderflags.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/data/updateinfo.h"
#include "blackcore/data/vatsimsetup.h"
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
        BlackCore::Data::CVatsimSetup::registerMetadata();
    }
} // namespace
