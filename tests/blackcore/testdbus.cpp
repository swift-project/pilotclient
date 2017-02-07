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

#include "testdbus.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackcore/test/testutils.h"
#include "blackcore/test/testservice.h"
#include "blackcore/test/testserviceinterface.h"
#include <QDBusConnection>
#include <QTest>

using namespace BlackMisc::Simulation;
using namespace BlackCore::Test;

namespace BlackCoreTest
{
    void CTestDBus::marshallUnmarshall()
    {
        ITestServiceInterface testServiceInterface(CTestService::InterfaceName(), CTestService::ObjectPath(), QDBusConnection::sessionBus());
        int errors = ITestServiceInterface::pingTests(testServiceInterface, false);
        QVERIFY2(errors == 0, "DBus Ping tests fail");
    }

    void CTestDBus::signatureSize()
    {
        constexpr int max = 255;
        QString s;

        // normally CSimulatedAircraftList is expected to be the biggest one
        const CAircraftModel model;
        s = CTestUtils::dBusSignature(model);
        QVERIFY2(s.length() <= max, "Signature CAircraftModel");

        const CSimulatedAircraft aircraft;
        s = CTestUtils::dBusSignature(aircraft);
        QVERIFY2(s.length() <= max, "Signature CSimulatedAircraft");

        const CSimulatedAircraftList al;
        s = CTestUtils::dBusSignature(al);
        QVERIFY2(s.length() <= max, "Signature CSimulatedAircraftList");
    }
}

//! \endcond
