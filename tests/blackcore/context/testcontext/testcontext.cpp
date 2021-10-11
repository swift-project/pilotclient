/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackcore

#include "blackcore/context/contextapplicationproxy.h"
#include "blackcore/context/contextaudioproxy.h"
#include "blackcore/context/contextnetworkproxy.h"
#include "blackcore/context/contextownaircraftproxy.h"
#include "blackcore/context/contextsimulatorproxy.h"
#include "blackcore/application.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/dbusutils.h"
#include "test.h"
#include <QTest>

#ifdef Q_OS_WIN
#include "comdef.h"
#endif

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackCoreTest
{
    //! Context implementation classes tests
    class CTestContext : public QObject
    {
        Q_OBJECT

    private slots:
        //! Init tests for context
        void initTestCase();

        //! Test the DBus signatures
        void dBusSignatures();
    };

    void CTestContext::initTestCase()
    {
        // will cause asserts when signal connects fail
        QDBusConnection connection = QDBusConnection::sessionBus();
        if (!connection.isConnected())
        {
            QSKIP("Cannot connect session DBus, skip unit test");
            return;
        }

        // CContextAudioProxy::unitTestRelaySignals() crashes in QAudioDeviceInfo::availableDevices(QAudio::AudioInput)
        // discussion https://discordapp.com/channels/539048679160676382/623947987822837779/631940817669455903
        // solved if unit test initializes a QGuiApplication
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

        constexpr int MaxSize = 220; // max size in dbus spec is 255
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

//! main
int main(int argc, char *argv[])
{
    // QCoreApplication app(argc, argv);
    // discussion https://discordapp.com/channels/539048679160676382/623947987822837779/631940817669455903
    // QDeviceInfo crashes using QCoreApplication

#ifdef _WIN32
    QGuiApplication app(argc, argv);
    const HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    Q_UNUSED(hr)
#else
    QCoreApplication app(argc, argv); // using QGuiApplication fails on Jenkins Linux, no display
#endif

    BLACKTEST_INIT(BlackCoreTest::CTestContext)
    CApplication a(CApplicationInfo::UnitTest);
    a.addVatlibOptions();
    const bool setup = a.parseAndSynchronizeSetup();
    if (!setup) { qWarning() << "No setup loaded"; }
    int r = EXIT_FAILURE;
    if (a.start())
    {
        r = QTest::qExec(&to, args);
    }
    a.gracefulShutdown();
    return r;
}

#include "testcontext.moc"

//! \endcond
