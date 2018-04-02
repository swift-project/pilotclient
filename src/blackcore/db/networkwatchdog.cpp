/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "networkwatchdog.h"
#include "application.h"
#include "blackcore/data/globalsetup.h"
#include "blackmisc/network/networkutils.h"
#include <QNetworkReply>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackCore::Data;

namespace BlackCore
{
    namespace Db
    {
        CNetworkWatchdog::CNetworkWatchdog(QObject *parent) : CContinuousWorker(parent, "swift DB watchdog")
        {
            Q_ASSERT_X(sApp, Q_FUNC_INFO, "Need sApp");
            const bool network = sApp->isNetworkAccessible(); // default
            m_networkAccessible = network;
            m_dbAccessible = network && m_checkDbAccessibility;
            m_internetAccessible = network;

            if (network)
            {
                this->initWorkingSharedUrlFromSetup();
            }

            m_updateTimer.setInterval(10 * 1000);
            connect(&m_updateTimer, &QTimer::timeout, this, &CNetworkWatchdog::doWork);
        }

        void CNetworkWatchdog::setDbAccessibility(bool accessible)
        {
            m_dbAccessible = accessible;
            m_internetAccessible = m_internetAccessible && m_networkAccessible;
            QTimer::singleShot(0, &m_updateTimer, [this] { m_updateTimer.start(); }); // restart timer
        }

        bool CNetworkWatchdog::hasWorkingSharedUrl() const
        {
            if (!m_networkAccessible) { return false; }
            return !this->getWorkingSharedUrl().isEmpty();
        }

        CUrl CNetworkWatchdog::getWorkingSharedUrl() const
        {
            if (!m_networkAccessible) { return CUrl(); }
            QReadLocker l(&m_lockUrl);
            return m_workingSharedUrl;
        }

        int CNetworkWatchdog::triggerCheck()
        {
            if (!this->doWorkCheck()) { return false; } // senseless
            if (m_checkInProgress) { return -1; }

            const int n = this->getCheckCount();
            QTimer::singleShot(0, this, &CNetworkWatchdog::doWork);
            return n; // triggered
        }

        QString CNetworkWatchdog::getLastPingDbUrl() const
        {
            QReadLocker l(&m_lockUrl);
            return m_lastPingUrl;
        }

        QString CNetworkWatchdog::getCheckInfo() const
        {
            static const QString info("Internet accessible: %1 (good: %2 / bad: %3), swift DB accessible: %4 (good: %5 / bad: %6) DB last ping URL: '%7' canConnect: %8ms");
            const QString pUrl(this->getLastPingDbUrl());
            return info.
                   arg(boolToYesNo(this->isInternetAccessible())).arg(m_goodCountInternet).arg(m_badCountInternet).
                   arg(boolToYesNo(this->isSwiftDbAccessible())).arg(m_goodCountDb).arg(m_badCountDb).
                   arg(pUrl).arg(CanConnectTimeMs);
        }

        void CNetworkWatchdog::setWorkingSharedUrl(const CUrl &workingUrl)
        {
            QWriteLocker l(&m_lockUrl);
            m_workingSharedUrl = workingUrl;
        }

        bool CNetworkWatchdog::isDbUrl(const CUrl &url)
        {
            const QString host(url.getHost());
            return host == dbHost();
        }

        void CNetworkWatchdog::doWork()
        {
            if (!this->doWorkCheck()) { return; }
            if (m_checkInProgress) { return; }
            m_checkInProgress = true;

            do
            {
                const bool wasDbAvailable = m_dbAccessible;
                const bool wasInternetAvailable = m_internetAccessible;
                const bool networkAccess = m_networkAccessible;
                const CUrl testUrl(CNetworkWatchdog::dbTestUrl());
                bool canConnectDb = m_checkDbAccessibility && networkAccess &&
                                    CNetworkUtils::canConnect(testUrl, CanConnectTimeMs); // running here in background worker
                if (m_checkDbAccessibility && m_doDetailedCheck && canConnectDb)
                {
                    // test against real HTTP response
                    const bool lastHttpSuccess = m_lastClientPingSuccess;
                    if (lastHttpSuccess && m_checkCount % 10 == 0)
                    {
                        // seems to be OK, from time to time ping
                        this->pingDbClientService(PingStarted);
                    }
                    else if (!lastHttpSuccess && m_checkCount % 3 == 0)
                    {
                        // not OK, retry more frequently
                        this->pingDbClientService(PingStarted, true);
                    }
                    canConnectDb = lastHttpSuccess;
                }

                bool canConnectInternet = canConnectDb;
                bool checkInternetAccess = !canConnectDb;

                m_dbAccessible = canConnectDb;
                if (canConnectDb)
                {
                    // DB available means internet available
                    m_internetAccessible = canConnectDb;
                }

                // check shared URL
                if (!this->doWorkCheck()) { break; }
                if (m_checkSharedUrl && networkAccess)
                {
                    if (CNetworkUtils::canConnect(this->getWorkingSharedUrl()))
                    {
                        canConnectInternet = true;
                        checkInternetAccess = false;
                    }
                    else
                    {
                        const CUrl sharedUrl = this->getWorkingSharedUrl();
                        if (!sharedUrl.isEmpty())
                        {
                            canConnectInternet = true;
                            checkInternetAccess = false;
                            this->setWorkingSharedUrl(sharedUrl);
                        }
                    }
                }

                // check internet access
                if (!this->doWorkCheck()) { break; }
                if (checkInternetAccess)
                {
                    QString message;
                    static const QString testHost1("www.google.com"); // what else?
                    canConnectInternet = CNetworkUtils::canConnect(testHost1, 443, message, CanConnectTimeMs); // running in background worker
                    if (!canConnectInternet)
                    {
                        static const QString testHost2("www.microsoft.com"); // secondary test
                        canConnectInternet = CNetworkUtils::canConnect(testHost2, 80, message, CanConnectTimeMs); // running in background worker
                    }
                    if (canConnectInternet) { m_goodCountInternet++; }
                    else { m_badCountInternet++; }
                }
                m_internetAccessible = networkAccess && canConnectInternet;

                // signals
                this->triggerChangedSignals(wasDbAvailable, wasInternetAvailable);
            }
            while (false);

            m_updateTimer.start(); // restart
            m_checkCount++;
            m_checkInProgress = false;
        }

        bool CNetworkWatchdog::doWorkCheck() const
        {
            if (!sApp) { return false; }
            if (sApp->isShuttingDown()) { return false; }
            if (!this->isEnabled()) { return false; }
            return true;
        }

        void CNetworkWatchdog::onChangedNetworkAccessibility(QNetworkAccessManager::NetworkAccessibility accessible)
        {
            const bool db = m_dbAccessible;
            const bool internet = m_internetAccessible;

            // Intentionally rating unknown as "accessible"
            if (accessible == QNetworkAccessManager::NotAccessible)
            {
                m_networkAccessible = false;
                m_dbAccessible = false;
                m_internetAccessible = false;
                this->triggerChangedSignals(db, internet);
            }
            else
            {
                m_networkAccessible = true;
                QTimer::singleShot(0, this, &CNetworkWatchdog::doWork);
            }
        }

        void CNetworkWatchdog::gracefulShutdown()
        {
            this->pingDbClientService(PingCompleteShutdown);
            this->quit();
        }

        void CNetworkWatchdog::pingDbClientService(CNetworkWatchdog::PingType type, bool force)
        {
            if (!force && !this->isSwiftDbAccessible()) { return; }
            if (!sApp) { return; }
            const CGlobalSetup gs = sApp->getGlobalSetup();
            if (!gs.wasLoaded()) { return; }
            CUrl pingUrl = gs.getDbClientPingServiceUrl();
            if (pingUrl.isEmpty()) { return; }

            pingUrl.appendQuery("uuid", this->identifier().toUuidString());
            pingUrl.appendQuery("application", sApp->getApplicationNameAndVersion());
            if (type.testFlag(PingLogoff)) { pingUrl.appendQuery("logoff", "true"); }
            if (type.testFlag(PingShutdown)) { pingUrl.appendQuery("shutdown", "true"); }
            if (type.testFlag(PingStarted)) { pingUrl.appendQuery("started", "true"); }

            sApp->getFromNetwork(pingUrl, { this, &CNetworkWatchdog::replyPingClientService });
        }

        void CNetworkWatchdog::replyPingClientService(QNetworkReply *nwReply)
        {
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nw(nwReply); // delete reply
            const bool ok = (nw->error() == QNetworkReply::NoError);
            nw->close();
            if (!sApp || sApp->isShuttingDown()) { return; }
            m_lastClientPingSuccess = ok;
            const QString url = nwReply->url().toString();
            {
                QWriteLocker l(&m_lockUrl);
                m_lastPingUrl = url;
            }

            if (ok) { m_goodCountDb++; }
            else { m_badCountDb++; }
            this->setDbAccessibility(ok);
        }

        void CNetworkWatchdog::triggerChangedSignals(bool oldDbAccessible, bool oldInternetAccessible)
        {
            if (!this->doWorkCheck()) { return; }

            // trigger really queued
            if (oldDbAccessible != m_dbAccessible)
            {
                const CUrl testUrl(this->dbTestUrl());
                QTimer::singleShot(0, this, [ = ] { emit this->changedSwiftDbAccessibility(m_dbAccessible, testUrl); });
            }
            if (oldInternetAccessible != m_internetAccessible)
            {
                QTimer::singleShot(0, this, [this] { emit this->changedInternetAccessibility(m_internetAccessible); });
            }
        }

        void CNetworkWatchdog::initWorkingSharedUrlFromSetup()
        {
            const CUrl workingUrl(CNetworkWatchdog::workingSharedUrlFromSetup()); // takes long
            this->setWorkingSharedUrl(workingUrl);
        }

        CUrl CNetworkWatchdog::dbTestUrl()
        {
            // requires global setup to be read
            const CUrl testUrl(sApp->getGlobalSetup().getDbHomePageUrl());
            return testUrl;
        }

        QString CNetworkWatchdog::dbHost()
        {
            const QString host = dbTestUrl().getHost();
            return host;
        }

        CUrl CNetworkWatchdog::workingSharedUrlFromSetup()
        {
            const CUrlList urls(sApp->getGlobalSetup().getSwiftSharedUrls());
            CFailoverUrlList failoverUrls(urls);
            return failoverUrls.getRandomWorkingUrl(2, CanConnectTimeMs); // uses CNetworkUtils::canConnect
        }
    } // ns
} // ns
