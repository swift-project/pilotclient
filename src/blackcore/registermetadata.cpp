/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/registermetadata.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/data/launchersetup.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/data/vatsimsetup.h"
#include "blackcore/db/databasereader.h"
#include "blackcore/vatsim/vatsimsettings.h"
#include "blackcore/network.h"
#include "blackcore/voicechannel.h"
#include "blackcore/webreaderflags.h"
#include "blackmisc/dbus.h"
#include "blackmisc/network/network.h"
#include "blackmisc/valueobject.h"

#include <QDBusMetaType>
#include <QMetaType>

namespace BlackCore
{
    void registerMetadata()
    {
        // not really clear when a type here has to be registered with qRegisterMetaType
        // however, does not harm if it is redundant
        qRegisterMetaType<INetwork::ConnectionStatus>();
        qRegisterMetaType<INetwork::LoginMode>();
        qRegisterMetaType<IVoiceChannel::ConnectionStatus>();
        qRegisterMetaType<CWebReaderFlags::WebReader>();
        qRegisterMetaType<CWebReaderFlags::WebReaderFlag>();

        qDBusRegisterMetaType<Context::CLogSubscriptionHash>();
        qDBusRegisterMetaType<Context::CLogSubscriptionPair>();
        qDBusRegisterMetaType<Context::CSettingsDictionary>();
        qDBusRegisterMetaType<INetwork::ConnectionStatus>();
        qDBusRegisterMetaType<INetwork::LoginMode>();
        qDBusRegisterMetaType<IVoiceChannel::ConnectionStatus>();

        Db::CDatabaseReaderConfig::registerMetadata();
        Db::CDatabaseReaderConfigList::registerMetadata();
        Data::CGlobalSetup::registerMetadata();
        Data::CVatsimSetup::registerMetadata();
        Data::CLauncherSetup::registerMetadata();
        Vatsim::CReaderSettings::registerMetadata();
    }
} // namespace
