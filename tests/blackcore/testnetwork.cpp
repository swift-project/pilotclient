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
#include "blackcore/network.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/url.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/stringutils.h"

#include <QString>
#include <QTest>
#include <QThread>
#include <QtDebug>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackCoreTest
{
    CTestNetwork::CTestNetwork(QObject *parent) :
        QObject(parent),
        m_networkVatlib(CClientProviderDummy::instance(), COwnAircraftProviderDummy::instance(), parent)
    {}

    void CTestNetwork::networkTest(BlackCore::INetwork *net)
    {
        const CServer fsdServer = CServer::swiftFsdTestServer(true);
        if (!this->pingServer(fsdServer)) { QSKIP("Server not reachable."); }

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
            // we skip the test at the beginning if the server cannot be reached
            // otherwise the whole build on Jenkins may fail
            QVERIFY(newStatus == INetwork::Connected);
            qDebug() << "CONNECTED";
        })
        .wait(10);

        EXPECT_UNIT(e)
        .send(&INetwork::sendPing, "server")
        .expect(&INetwork::pongReceived, [](const CCallsign & callsign, const PhysicalQuantities::CTime & elapsedTime)
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
        const CUrl url(server.getAddress(), server.getPort());
        if (!CNetworkUtils::canConnect(url, m, 2500))
        {
            qWarning() << "Skipping unit test as" << url.getFullUrl() << "cannot be connected";
            return false;
        }
        return true;
    }
}

//! \endcond
