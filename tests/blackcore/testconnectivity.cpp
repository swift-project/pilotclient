/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackcore

#include "testconnectivity.h"
#include "blackcore/application.h"
#include "blackcore/db/networkwatchdog.h"
#include <QDateTime>
#include <QDebug>
#include <QTest>
#include <QString>
#include <QtDebug>

using namespace BlackCore;
using namespace BlackCore::Db;
using namespace BlackMisc::Network;

namespace BlackCoreTest
{
    CTestConnectivity::CTestConnectivity(QObject *parent) :
        QObject(parent)
    { }

    CTestConnectivity::~CTestConnectivity()
    { }

    void CTestConnectivity::initTestCase()
    {
        QVERIFY2(sApp, "sApp not available");
        QVERIFY2(sApp->getNetworkWatchdog(), "No network watchdog");

        const int n = sApp->triggerNetworkWatchdogChecks();
        QVERIFY2(n >= 0, "Cannot trigger setup reader");
        m_networkCheckCount = n;
        qDebug() << "Initial network check count:" << n;
    }

    void CTestConnectivity::checkSetupReader()
    {
        if (!sApp->hasSetupReader()) { QSKIP("Cannot load bootstrap file, skip unit test"); }
    }

    void CTestConnectivity::connectServer()
    {
        if (!sApp->hasSetupReader()) { QSKIP("Cannot load bootstrap file, skip unit test"); }
        const CUrl url = sApp->getGlobalSetup().getDbHomePageUrl();
        QTime timer;
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
        QTime timer;
        timer.start();
        constexpr int max = 5;
        for (int i = 0; i < max; i++)
        {
            bool ok = CNetworkUtils::canPing(host);
            if (!ok) { QSKIP(qPrintable("Cannot ping " + url.getFullUrl())); }
        }
        int elapsedMs = timer.elapsed();
        qDebug() << "Completed" << max << "ping tests in" << elapsedMs << "ms to" << url.getFullUrl();
        QVERIFY2(true, "pingServer");
    }

    void CTestConnectivity::testNetworkWatchdog()
    {
        QVERIFY2(sApp->getNetworkWatchdog(), "No network watchdog");
        const CUrl dbUrl = CNetworkWatchdog::dbTestUrl();
        qDebug() << "Using DB test URL: " << dbUrl.toQString();
        const bool ok = CNetworkUtils::canPing(dbUrl);
        if (!ok) { QSKIP(qPrintable("Cannot ping " + dbUrl.getFullUrl())); }

        // only if URL is reachable
        QTRY_VERIFY2_WITH_TIMEOUT(sApp->isSwiftDbAccessible(), "Watchdog cannot connect db", 20000);
        QTRY_VERIFY2_WITH_TIMEOUT(sApp->getNetworkWatchdog()->getCheckCount() >= m_networkCheckCount + 1, "Timeout of network check", 30000);
        qDebug() << "Current network check count:" << sApp->getNetworkWatchdog()->getCheckCount();
    }
} // ns

//! \endcond
