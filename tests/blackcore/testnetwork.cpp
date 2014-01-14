/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testnetwork.h"
#include "expect.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

void BlackCoreTest::CTestNetwork::networkTest(BlackCore::INetwork *net)
{
    Expect e(net);

    EXPECT_UNIT(e)
    .send(&INetwork::setServer, CServer("", "", "vatsim-germany.org", 6809, CUser("guest", "", "", "guest")))
    .send(&INetwork::setCallsign, "BLACK")
    .send(&INetwork::initiateConnection)
    .expect(&INetwork::connectionStatusChanged, [](INetwork::ConnectionStatus, INetwork::ConnectionStatus newStatus)
    {
        QVERIFY(newStatus == INetwork::Connecting);
        qDebug() << "CONNECTING";
    }
    ).expect(&INetwork::connectionStatusChanged, [](INetwork::ConnectionStatus, INetwork::ConnectionStatus newStatus)
    {
        QVERIFY(newStatus == INetwork::Connected);
        qDebug() << "CONNECTED";
    }
    ).wait(10);

    EXPECT_UNIT(e)
    .send(&INetwork::ping, "server")
    .expect(&INetwork::pong, [](CCallsign callsign, PhysicalQuantities::CTime elapsedTime)
    {
        qDebug() << "PONG" << callsign << elapsedTime;
    }
    ).wait(10);

    EXPECT_UNIT(e)
    .send(&INetwork::terminateConnection)
    .expect(&INetwork::connectionStatusChanged, [](INetwork::ConnectionStatus s)
    {
        QVERIFY(s == INetwork::Disconnected);
        qDebug() << "DISCONNECTED";
    }
    ).wait(10);
}
