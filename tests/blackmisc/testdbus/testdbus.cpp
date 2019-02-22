/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include "blackmisc/registermetadata.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/test/testservice.h"
#include "blackmisc/test/testserviceinterface.h"
#include "blackmisc/dbusutils.h"
#include "test.h"
#include <QDBusConnection>
#include <QTest>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Test;

namespace BlackMiscTest
{
    //! DBus implementation classes tests
    class CTestDBus : public QObject
    {
        Q_OBJECT

    private slots:
        //! Init test case data
        void initTestCase();

        //! Test marshalling/unmarshalling
        void marshallUnmarshall();

        //! Signature size
        void signatureSize();
    };

    void CTestDBus::initTestCase()
    {
        BlackMisc::registerMetadata();
    }

    void CTestDBus::marshallUnmarshall()
    {
        QDBusConnection connection = QDBusConnection::sessionBus();
        if (!CTestService::canRegisterTestService(connection))
        {
            QSKIP("Cannot register DBus service, skip unit test");
            return;
        }
        CTestService *testService = CTestService::registerTestService(connection, false, QCoreApplication::instance());
        Q_UNUSED(testService);
        ITestServiceInterface testServiceInterface(CTestService::InterfaceName(), CTestService::ObjectPath(), connection);
        const int errors = ITestServiceInterface::pingTests(testServiceInterface, false);
        QVERIFY2(errors == 0, "DBus Ping tests fail");
    }

    void CTestDBus::signatureSize()
    {
        constexpr int max = 255;
        QString s;

        // normally CSimulatedAircraftList is expected to be the biggest one
        const CAircraftModel model;
        s = CDBusUtils::dBusSignature(model);
        QVERIFY2(s.length() <= max, "Signature CAircraftModel");

        const CSimulatedAircraft aircraft;
        s = CDBusUtils::dBusSignature(aircraft);
        QVERIFY2(s.length() <= max, "Signature CSimulatedAircraft");

        const CSimulatedAircraftList al;
        s = CDBusUtils::dBusSignature(al);
        QVERIFY2(s.length() <= max, "Signature CSimulatedAircraftList");
    }
}

//! main
BLACKTEST_MAIN(BlackMiscTest::CTestDBus);

#include "testdbus.moc"

//! \endcond
