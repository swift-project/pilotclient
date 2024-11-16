// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testmisc
 */

#include <QDBusConnection>
#include <QTest>

#include "test.h"

#include "misc/dbusutils.h"
#include "misc/registermetadata.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/test/testservice.h"
#include "misc/test/testserviceinterface.h"

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::misc::test;

namespace MiscTest
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

    void CTestDBus::initTestCase() { swift::misc::registerMetadata(); }

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
        ITestServiceInterface testServiceInterface(CTestService::InterfaceName(), CTestService::ObjectPath(),
                                                   connection);
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
} // namespace MiscTest

//! main
SWIFTTEST_MAIN(MiscTest::CTestDBus);

#include "testdbus.moc"

//! \endcond
