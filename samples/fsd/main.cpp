// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplefsd

#include "core/fsd/fsdclient.h"
#include "misc/network/clientprovider.h"
#include "misc/simulation/ownaircraftproviderdummy.h"
#include "misc/simulation/remoteaircraftproviderdummy.h"
#include <QCoreApplication>
#include <QThread>

using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::core::fsd;

//! main
int main(int argc, char *argv[])
{
    QCoreApplication qa(argc, argv);
    COwnAircraftProviderDummy::instance()->updateOwnCallsign("BER368");

    CFSDClient client(CClientProviderDummy::instance(), COwnAircraftProviderDummy::instance(), CRemoteAircraftProviderDummy::instance(), &qa);
    client.setClientName("Test Client");
    client.setHostApplication("None");
    client.setVersion(0, 8);
#ifdef SWIFT_VATSIM_SUPPORT
    QString key("727d1efd5cb9f8d2c28372469d922bb4");
    client.setClientIdAndKey(0xb9ba, key.toLocal8Bit());
#endif
    client.setClientCapabilities(Capabilities::AtcInfo | Capabilities::AircraftInfo | Capabilities::AircraftConfig);

    const CUser user("1234567", "Test user - EDDM", "", "123456");
    CServer server("fsd.swift-project.org", 6809, user);
    server.setServerType(CServer::FSDServerVatsim);
    client.setServer(server);
    client.setSimType(CSimulatorInfo::xplane());
    client.setPilotRating(PilotRating::Student);
    client.printToConsole(true);

    /*client.sendFsdMessage("$CRLOWW_F_APP:LHA449:ATIS:V:voice.vacc.ch/loww_f_app\r\n");
    client.sendFsdMessage("$CRLOWW_F_APP:LHA449:ATIS:T:Wien Director, Servus\r\n");
    client.sendFsdMessage("$CRLOWW_F_APP:LHA449:ATIS:T:CALLSIGN ONLY\r\n");
    client.sendFsdMessage("$CRLOWW_F_APP:LHA449:ATIS:T:For charts visit www.vacc-austria.org\r\n");
    client.sendFsdMessage("$CRLOWW_F_APP:LHA449:ATIS:Z:z\r\n");
    client.sendFsdMessage("$CRLOWW_F_APP:LHA449:ATIS:E:6\r\n");
    client.sendFsdMessage("$CRN1234:BAW345:CAPS:INTERIMPOS=1:MODELDESC=1:ATCINFO=1:STEALTH=1:ACCONFIG=1\r\n");
    client.sendFsdMessage("#SBBAW106:LHA449:PI:GEN:EQUIPMENT=B744:AIRLINE=BAW:LIVERY=UNION\r\n");
    client.sendFsdMessage("#SBGEC55F:DESWL:FSIPIR:1::MD11:12.93209:-0.01354:3648.00000:4.CB8FB1E0.984745A0::PMDG MD-11F Lufthansa Cargo WOW\r\n");
    client.sendFsdMessage("#TMAFR529:@20500&@26000:taxi to entry N1 via M A4\r\n");
    client.sendFsdMessage("$CQDLH123:BER368:ACC:json config\r\n");
    client.sendFsdMessage("$CQDLH123:@94836:ACC:{\"request\":\"full\"}\r\n");
    client.sendFsdMessage("#SBAUA417C:LHA449:PI:X:0:1:~B737");
    client.sendFsdMessage("#SBBER368:someone:I:47.29946:14.45892:41082:473:4278194872\r\n");
    client.sendFsdMessage("$CQURRR_R_APP:@94835:SC:VTD740:ÈËÑ\r\n");
    client.sendFsdMessage("@N:SVA732:346:1:53.10591:2.50108:37010:529:4261225460:42\r\n");
    client.sendFsdMessage("$CRLHA449:LOWW_TWR:RN:Peter Buchegger - LOWL:NONE:1\r\n");*/

    client.connectToServer();

    while (client.getConnectionStatus() == CConnectionStatus::Disconnected)
    {
        QCoreApplication::processEvents();
        QThread::msleep(100);
    }

    bool functionTestsDone = false;
    while (client.getConnectionStatus() == CConnectionStatus::Connected)
    {
        QThread::msleep(100);
        if (!functionTestsDone)
        {
            /*client.sendFlightPlan(FlightType::VFR, "B744", 420, "EGLL", 1530, 1535, "FL350", "KORD", 8, 15,
                                   9, 30, "NONE", "Unit Test", "EGLL.KORD");
            client.sendClientQuery(ClientQueryType::FP, "SERVER", { "BER368" });*/
            functionTestsDone = true;
        }
        QCoreApplication::processEvents();
    }

    return qa.exec();
}
