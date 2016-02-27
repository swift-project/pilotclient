/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackcore
 */

#include "testnetwork.h"
#include "expect.h"
#include "blackmisc/network/networkutils.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackCoreTest
{

    void CTestNetwork::networkTest(BlackCore::INetwork *net)
    {
        CServer fsdServer("", "", "vatsim-germany.org", 6809, CUser("1234567", "", "", "123456"));
        if (!this->pingServer(fsdServer))
        {
            qWarning() << "Skipping unit test because fsd server is not reachable.";
            return;
        }

        QString string = net->connectionStatusToString(INetwork::Connected);
        QVERIFY(string == "Connected");

        Expect e(net);
        CSimulatedAircraft aircraft;
        aircraft.setIcaoCodes(CAircraftIcaoCode("C172", "L1P"), CAirlineIcaoCode("YYY"));

        EXPECT_UNIT(e)
        .send(&INetwork::presetServer, fsdServer)
        .send(&INetwork::presetCallsign, "SWIFT")
        .send(&INetwork::presetIcaoCodes, aircraft)
        .send(&INetwork::initiateConnection)
        .expect(&INetwork::connectionStatusChanged, [](INetwork::ConnectionStatus, INetwork::ConnectionStatus newStatus)
        {
            QVERIFY(newStatus == INetwork::Connecting);
            qDebug() << "CONNECTING";
        })
        .expect(&INetwork::connectionStatusChanged, [](INetwork::ConnectionStatus, INetwork::ConnectionStatus newStatus)
        {
            //! \todo verify how we want to handle the situation if the connect fails. On Jenkins that would cause a failed test case and hence a failed build
            QVERIFY(newStatus == INetwork::Connected);
            qDebug() << "CONNECTED";
        })
        .wait(10);

        EXPECT_UNIT(e)
        .send(&INetwork::sendPing, "server")
        .expect(&INetwork::pongReceived, [](CCallsign callsign, PhysicalQuantities::CTime elapsedTime)
        {
            qDebug() << "PONG" << callsign << elapsedTime;
        })
        .wait(10);

        EXPECT_UNIT(e)
        .send(&INetwork::terminateConnection)
        .expect(&INetwork::connectionStatusChanged, [](INetwork::ConnectionStatus, INetwork::ConnectionStatus newStatus)
        {
            QVERIFY(newStatus == INetwork::Disconnecting);
            qDebug() << "DISCONNECTING";
        })
        .expect(&INetwork::connectionStatusChanged, [](INetwork::ConnectionStatus, INetwork::ConnectionStatus newStatus)
        {
            QVERIFY(newStatus == INetwork::Disconnected);
            qDebug() << "DISCONNECTED";
        })
        .wait(10);

        QThread::msleep(250); // make sure the last debug messages are written
    }

    bool CTestNetwork::pingServer(const CServer &server)
    {
        QString m;
        CUrl url(server.getAddress(), server.getPort());
        if (!CNetworkUtils::canConnect(url, m, 2500))
        {
            return false;
        }
        return true;
    }

}

//! \endcond
