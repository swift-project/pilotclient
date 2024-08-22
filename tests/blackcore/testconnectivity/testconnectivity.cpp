// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackcore

#include "blackcore/application.h"
#include "blackcore/setupreader.h"
#include "blackmisc/applicationinfo.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/statusmessagelist.h"
#include "test.h"
#include <QObject>
#include <QDateTime>
#include <QDebug>
#include <QTest>
#include <QString>
#include <QtDebug>

using namespace BlackCore;
using namespace BlackCore::Db;
using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackCoreTest
{
    //! Test connectivity such as \c canConnect \c ping and BlackCore::Db::CNetworkWatchdog
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
} // ns

//! main
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    BLACKTEST_INIT(BlackCoreTest::CTestConnectivity)
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
