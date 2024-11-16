// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testswiftcore

#include <QDateTime>
#include <QDebug>
#include <QObject>
#include <QString>
#include <QTest>
#include <QtDebug>

#include "test.h"

#include "core/application.h"
#include "core/setupreader.h"
#include "misc/applicationinfo.h"
#include "misc/network/networkutils.h"
#include "misc/statusmessagelist.h"

using namespace swift::core;
using namespace swift::core::db;
using namespace swift::misc;
using namespace swift::misc::network;

namespace swiftcoretest
{
    //! Test connectivity such as \c canConnect \c ping and swift::core::db::CNetworkWatchdog
    class CTestConnectivity : public QObject
    {
        Q_OBJECT

    private slots:
        //! Init
        void initTestCase();

        //! Setup reader
        void checkSetupReader();

        //! Connecting test server
        void connectServer();

        //! Ping test server
        void pingServer();
    };

    void CTestConnectivity::initTestCase()
    {
        QVERIFY2(sApp, "sApp not available");
    }

    void CTestConnectivity::checkSetupReader()
    {
        if (!sApp->hasSetupReader()) { QSKIP("Cannot load bootstrap file, skip unit test"); }
    }

    void CTestConnectivity::connectServer()
    {
        if (!sApp->hasSetupReader()) { QSKIP("Cannot load bootstrap file, skip unit test"); }
        const CUrl url = sApp->getGlobalSetup().getDbHomePageUrl();
        QElapsedTimer timer;
        timer.start();
        constexpr int max = 5;
        for (int i = 0; i < max; i++)
        {
            bool ok = CNetworkUtils::canConnect(url, CNetworkUtils::getLongTimeoutMs());
            if (!ok) { QSKIP(qPrintable("Cannot connect " + url.getFullUrl())); }
        }
        int elapsedMs = timer.elapsed();
        qDebug() << "Completed" << max << "connection tests in" << elapsedMs << "ms to" << url.getFullUrl();
        QVERIFY2(true, "connectServer");
    }

    void CTestConnectivity::pingServer()
    {
        if (!sApp->hasSetupReader()) { QSKIP("Cannot load bootstrap file, skip unit test"); }
        const CUrl url = sApp->getGlobalSetup().getDbHomePageUrl();
        const QString host(url.getHost());
        QElapsedTimer timer;
        timer.start();
        constexpr int max = 5;
        for (int i = 0; i < max; i++)
        {
            bool ok = canPing(host);
            if (!ok) { QSKIP(qPrintable("Cannot ping " + url.getFullUrl())); }
        }
        int elapsedMs = timer.elapsed();
        qDebug() << "Completed" << max << "ping tests in" << elapsedMs << "ms to" << url.getFullUrl();
        QVERIFY2(true, "pingServer");
    }
} // namespace swiftcoretest

//! main
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    SWIFTTEST_INIT(swiftcoretest::CTestConnectivity)
    CApplication a(CApplicationInfo::UnitTest);
    a.addNetworkOptions();
    const bool setup = a.parseCommandLineArgsAndLoadSetup();
    if (!setup) { qWarning() << "No setup loaded"; }
    int r = EXIT_FAILURE;
    if (a.start())
    {
        r = QTest::qExec(&to, args);
    }
    a.gracefulShutdown();
    return r;
}

#include "testconnectivity.moc"

//! \endcond
