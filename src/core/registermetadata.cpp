// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/registermetadata.h"

#include <QDBusMetaType>
#include <QMetaType>

#include "core/afv/clients/afvclient.h"
#include "core/aircraftmatcher.h"
#include "core/context/contextapplication.h"
#include "core/data/globalsetup.h"
#include "core/data/launchersetup.h"
#include "core/data/vatsimsetup.h"
#include "core/db/databasereader.h"
#include "core/fsd/fsdclient.h"
#include "core/simulator.h"
#include "core/vatsim/vatsimsettings.h"
#include "core/webreaderflags.h"
#include "misc/dbus.h"
#include "misc/network/loginmode.h"
#include "misc/valueobject.h"

using namespace swift::core::fsd;

namespace swift::core
{
    void registerMetadata()
    {
        // not really clear when a type here has to be registered with qRegisterMetaType
        // however, does not harm if it is redundant
        qRegisterMetaType<CWebReaderFlags::WebReader>();
        qRegisterMetaType<CWebReaderFlags::WebReaderFlag>();
        qRegisterMetaType<swift::core::afv::clients::CAfvClient::ConnectionStatus>();
        qRegisterMetaType<swift::core::afv::clients::CAfvClient::ConnectionStatus>("ConnectionStatus");
        qRegisterMetaType<swift::core::afv::audio::TransceiverReceivingCallsignsChangedArgs>();
        qRegisterMetaType<swift::core::afv::audio::TransceiverReceivingCallsignsChangedArgs>(
            "TransceiverReceivingCallsignsChangedArgs");

        qDBusRegisterMetaType<context::CSettingsDictionary>();
        qDBusRegisterMetaType<swift::misc::network::CLoginMode>();
        qDBusRegisterMetaType<swift::core::ISimulator::SimulatorStatusFlag>();
        qDBusRegisterMetaType<swift::core::ISimulator::SimulatorStatus>();

        db::CDatabaseReaderConfig::registerMetadata();
        db::CDatabaseReaderConfigList::registerMetadata();
        data::CGlobalSetup::registerMetadata();
        data::CVatsimSetup::registerMetadata();
        data::CLauncherSetup::registerMetadata();
        vatsim::CReaderSettings::registerMetadata();
        vatsim::CRawFsdMessageSettings::registerMetadata();

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
} // namespace swift::core
