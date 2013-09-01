/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testnetwork.h"
#include "expect.h"

using namespace BlackCore;
using namespace BlackMisc;

void BlackCoreTest::CTestNetwork::networkTest(BlackCore::INetwork* net)
{
    Expect e(net);
    QString callsign = "TEST01";

    EXPECT_UNIT(e)
        .send(&INetwork::setServerDetails, "vatsim-germany.org", 6809)
        .send(&INetwork::setUserCredentials, "guest", "guest")
        .send(&INetwork::setRealName, "Pilot Client Tester")
        .send(&INetwork::setCallsign, callsign)
        .send(&INetwork::initiateConnection)
        .expect(&INetwork::connectionStatusConnecting, []{ qDebug() << "CONNECTING"; })
        .expect(&INetwork::connectionStatusConnected, []{ qDebug() << "CONNECTED"; })
        .wait(10);

    EXPECT_UNIT(e)
        .send(&INetwork::ping, "server")
        .expect(&INetwork::pong, [](QString s, PhysicalQuantities::CTime t){ qDebug() << "PONG" << s << t; })
        .wait(10);

    EXPECT_UNIT(e)
        .send(&INetwork::terminateConnection)
        .expect(&INetwork::connectionStatusDisconnected, []{ qDebug() << "DISCONNECTED"; })
        .wait(10);
}