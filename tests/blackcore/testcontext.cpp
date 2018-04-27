/* Copyright (C) 2017
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

#include "testcontext.h"
#include "blackcore/context/contextallproxies.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/dbusutils.h"
#include <QTest>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackCore::Context;

namespace BlackCoreTest
{
    void CTestContext::contextInitTest()
    {
        // will cause asserts when signal connects fail
        QDBusConnection connection = QDBusConnection::sessionBus();
        if (!connection.isConnected())
        {
            QSKIP("Cannot connect session DBus, skip unit test");
            return;
        }

        CContextAudioProxy::unitTestRelaySignals();
        CContextNetworkProxy::unitTestRelaySignals();
        CContextOwnAircraftProxy::unitTestRelaySignals();
        CContextSimulatorProxy::unitTestRelaySignals();
    }

    void CTestContext::dBusSignatures()
    {
        const CAircraftModel model;
        const CAircraftSituation situation;
        const CSimulatedAircraft aircraft;
        const CSimulatedAircraftList aircraftList;

        constexpr int MaxSize = 210;
        int s = CDBusUtils::dBusSignatureSize(model);
        QVERIFY2(s < MaxSize, "CAircraftModel");

        s = CDBusUtils::dBusSignatureSize(situation);
        QVERIFY2(s < MaxSize, "CAircraftSituation");

        s = CDBusUtils::dBusSignatureSize(aircraft);
        QVERIFY2(s < MaxSize, "CSimulatedAircraft");

        s = CDBusUtils::dBusSignatureSize(aircraftList);
        QVERIFY2(s < MaxSize, "CSimulatedAircraftList");
    }
} // ns

//! \endcond
