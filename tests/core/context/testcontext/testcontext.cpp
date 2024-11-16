// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testswiftcore

#include <QTest>

#include "test.h"

#include "core/application.h"
#include "core/context/contextapplicationproxy.h"
#include "core/context/contextaudioproxy.h"
#include "core/context/contextnetworkproxy.h"
#include "core/context/contextownaircraftproxy.h"
#include "core/context/contextsimulatorproxy.h"
#include "misc/dbusutils.h"
#include "misc/simulation/simulatedaircraftlist.h"

#ifdef Q_OS_WIN
#    include "comdef.h"
#endif

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;
using namespace swift::core;
using namespace swift::core::context;

namespace SwiftCoreTest
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
} // namespace SwiftCoreTest

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

    SWIFTTEST_INIT(SwiftCoreTest::CTestContext)
    CApplication a(CApplicationInfo::UnitTest);
    a.addNetworkOptions();
    const bool setup = a.parseCommandLineArgsAndLoadSetup();
    if (!setup) { qWarning() << "No setup loaded"; }
    int r = EXIT_FAILURE;
    if (a.start()) { r = QTest::qExec(&to, args); }
    a.gracefulShutdown();
    return r;
}

#include "testcontext.moc"

//! \endcond
