/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "registermetadata.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/data/launchersetup.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/data/vatsimsetup.h"
#include "blackcore/db/databasereader.h"
#include "blackcore/vatsim/vatsimsettings.h"
#include "blackcore/fsd/fsdclient.h"
#include "blackcore/afv/clients/afvclient.h"
#include "blackcore/simulator.h"
#include "blackcore/webreaderflags.h"
#include "blackcore/aircraftmatcher.h"
#include "blackmisc/network/network.h"
#include "blackmisc/dbus.h"
#include "blackmisc/valueobject.h"

#include <QDBusMetaType>
#include <QMetaType>

using namespace BlackCore::Fsd;

namespace BlackCore
{
    void registerMetadata()
    {
        // not really clear when a type here has to be registered with qRegisterMetaType
        // however, does not harm if it is redundant
        qRegisterMetaType<CWebReaderFlags::WebReader>();
        qRegisterMetaType<CWebReaderFlags::WebReaderFlag>();
        qRegisterMetaType<BlackCore::Afv::Clients::CAfvClient::ConnectionStatus>();
        qRegisterMetaType<BlackCore::Afv::Clients::CAfvClient::ConnectionStatus>("ConnectionStatus");
        qRegisterMetaType<BlackCore::Afv::Audio::TransceiverReceivingCallsignsChangedArgs>();
        qRegisterMetaType<BlackCore::Afv::Audio::TransceiverReceivingCallsignsChangedArgs>("TransceiverReceivingCallsignsChangedArgs");

        qDBusRegisterMetaType<Context::CSettingsDictionary>();
        qDBusRegisterMetaType<BlackMisc::Network::CLoginMode>();
        qDBusRegisterMetaType<BlackCore::ISimulator::SimulatorStatusFlag>();
        qDBusRegisterMetaType<BlackCore::ISimulator::SimulatorStatus>();

        qRegisterMetaTypeStreamOperators<Context::CSettingsDictionary>();
        qRegisterMetaTypeStreamOperators<BlackMisc::Network::CLoginMode>();

        Db::CDatabaseReaderConfig::registerMetadata();
        Db::CDatabaseReaderConfigList::registerMetadata();
        Data::CGlobalSetup::registerMetadata();
        Data::CVatsimSetup::registerMetadata();
        Data::CLauncherSetup::registerMetadata();
        Vatsim::CReaderSettings::registerMetadata();
        Vatsim::CRawFsdMessageSettings::registerMetadata();

        // not really clear when a type here has to be registered with qRegisterMetaType
        // however, does not harm if it is redundant
        qRegisterMetaType<AtcRating>();
        qRegisterMetaType<PilotRating>();
        qRegisterMetaType<SimType>();
        qRegisterMetaType<ClientQueryType>();
        qRegisterMetaType<FlightType>();
        qRegisterMetaType<ServerErrorCode>();
        qRegisterMetaType<ServerType>();
        qRegisterMetaType<Capabilities>();
    }
} // namespace
