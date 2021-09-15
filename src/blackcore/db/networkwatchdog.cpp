/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "networkwatchdog.h"
#include "application.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/networkutils.h"

#include <QNetworkReply>
#include <QDateTime>
#include <QPointer>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackCore::Data;

namespace BlackCore::Db
{
    const QStringList &CNetworkWatchdog::getLogCategories()
    {
        static const QStringList cats = CContinuousWorker::getLogCategories() + QStringList
        {
            CLogCategories::swiftDbWebservice(), CLogCategories::webservice(), CLogCategories::network()
        };
        return cats;
    }

    CNetworkWatchdog::CNetworkWatchdog(bool networkAccessible, QObject *owner) : CContinuousWorker(owner, "swift DB watchdog")
    {
        Q_ASSERT_X(owner, Q_FUNC_INFO, "Need owner (normally sApp)");

        m_networkAccessible = networkAccessible;
        m_internetAccessible = networkAccessible;
        m_dbAccessible = networkAccessible && m_checkDbAccessibility;

        m_updateTimer.setInterval(10 * 1000);
        connect(&m_updateTimer, &QTimer::timeout, this, &CNetworkWatchdog::doWork);
    }

    void CNetworkWatchdog::setDbAccessibility(bool accessible)
    {
        m_dbAccessible = accessible;
        m_internetAccessible = m_internetAccessible && this->isNetworkkAccessibleOrCheckDisabled();

        // restart timer
        QPointer<CNetworkWatchdog> myself(this);
        QTimer::singleShot(0, &m_updateTimer, [ = ]
        {
            if (!myself) { return; }
            m_updateTimer.start();
        });
    }

    bool CNetworkWatchdog::hasWorkingSharedUrl() const
    {
        if (!this->isNetworkkAccessibleOrCheckDisabled()) { return false; }
        return !this->getWorkingSharedUrl().isEmpty();
    }

    CUrl CNetworkWatchdog::getWorkingSharedUrl() const
    {
        if (!this->isNetworkkAccessibleOrCheckDisabled()) { return CUrl(); }
        QReadLocker l(&m_lockUrl);
        return m_workingSharedUrl;
    }

    int CNetworkWatchdog::triggerCheck()
    {
        if (!this->doWorkCheck()) { return false; } // senseless
        if (m_checkInProgress) { return -1; }

        const int n = this->getCheckCount();
        const QPointer<CNetworkWatchdog> myself(this);
        QTimer::singleShot(0, this, [ = ]
        {
            if (!myself) { return; }
            this->doWork();
        });
        return n; // triggered
    }

    QString CNetworkWatchdog::getLastPingDbUrl() const
    {
        QReadLocker l(&m_lockUrl);
        return m_lastPingUrl;
    }

    QString CNetworkWatchdog::getCheckInfo() const
    {
        static const QString info("Network accessibility check: %1 | Internet accessible: %2 (good: %3 / bad: %4), swift DB accessible: %5 (good: %6 / bad: %7) DB last ping URL: '%8' canConnect: %9ms");
        const QString pUrl(this->getLastPingDbUrl());
        static const QString cct = QString::number(CanConnectTimeMs);
        return info.
                arg(boolToEnabledDisabled(!this->isNetworkAccessibilityCheckDisabled()),  boolToYesNo(this->isInternetAccessible())).arg(m_totalGoodCountInternet).arg(m_totalBadCountInternet).
                arg(boolToYesNo(this->isSwiftDbAccessible())).arg(m_totalGoodCountDb).arg(m_totalBadCountDb).
                arg(pUrl, cct); // cct has to be string, otherwise the % in the URL will be replaced
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

        // lazy init
        if (!this->hasWorkingSharedUrl())
        {
            this->initWorkingSharedUrlFromSetup();
        }

        // checks
        do
        {
            const bool wasDbAvailable = m_dbAccessible;
            const bool wasInternetAvailable = m_internetAccessible;
            const bool networkAccessible = this->isNetworkkAccessibleOrCheckDisabled();
            const CUrl testUrl(CNetworkWatchdog::dbTestUrl());
            bool canConnectDb = m_checkDbAccessibility && networkAccessible;
            if (canConnectDb)
            {
                // running here in background worker check twice
                canConnectDb = CNetworkUtils::canConnect(testUrl, CanConnectTimeMs);
                if (!canConnectDb)
                {
                    canConnectDb = CNetworkUtils::canConnect(testUrl, CanConnectTimeMs);
                }
            }

            if (m_checkDbAccessibility && m_doDetailedCheck && canConnectDb)
            {
                const qint64 pingIntervalSecs = sApp->getGlobalSetup().getDbClientPingIntervalSecs();
                if (QDateTime::currentSecsSinceEpoch() >= pingIntervalSecs)
                {
                    m_nextPingSecsSinceEpoch = QDateTime::currentSecsSinceEpoch() + pingIntervalSecs;
                    this->pingDbClientService(CGlobalSetup::PingStarted);
                    canConnectDb = m_lastClientPingSuccess;
                }
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
            if (m_checkSharedUrl && networkAccessible)
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
                if (canConnectInternet) { m_totalGoodCountInternet++; }
                else { m_totalBadCountInternet++; }
            }
            m_internetAccessible = networkAccessible && canConnectInternet;

            // signals
            this->triggerChangedSignals(wasDbAvailable, wasInternetAvailable);
        }
        while (false);

        m_updateTimer.start(); // restart
        m_totalCheckCount++;
        m_checkInProgress = false;
    }

    bool CNetworkWatchdog::doWorkCheck() const
    {
        if (!sApp || sApp->isShuttingDown()) { return false; }
        if (!this->isEnabled()) { return false; }
        return true;
    }

    void CNetworkWatchdog::setNetworkAccessibility(QNetworkAccessManager::NetworkAccessibility accessibility)
    {
        // avoid unnecessary signals
        const int accessiblityInt = static_cast<int>(accessibility);
        if (m_networkAccessibility == accessiblityInt) { return; }
        if (m_disableNetworkCheck) { return; } // ignore with disabled check

        // shift to thread
        if (!CThreadUtils::isInThisThread(this))
        {
            QPointer<CNetworkWatchdog> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!sApp || sApp->isShuttingDown() || !myself) { return; }
                this->setNetworkAccessibility(accessibility);
            });
            return;
        }

        // set values
        m_networkAccessibility = accessiblityInt;
        const bool db = m_dbAccessible;
        const bool internet = m_internetAccessible;

        // Intentionally rating unknown as "accessible"
        if (accessibility == QNetworkAccessManager::NotAccessible)
        {
            m_networkAccessible = false;
            m_dbAccessible = false;
            m_internetAccessible = false;
            this->triggerChangedSignals(db, internet);
        }
        else
        {
            m_networkAccessible = true;
            const QPointer<CNetworkWatchdog> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself) { return; }
                this->doWork();
            });
        }

        emit this->changedNetworkAccessible(accessibility);
    }

    void CNetworkWatchdog::networkConfigurationsUpdateCompleted()
    {
        // void
    }

    void CNetworkWatchdog::setOnline(bool online)
    {
        m_online = online;
    }

    void CNetworkWatchdog::gracefulShutdown()
    {
        this->pingDbClientService(CGlobalSetup::PingCompleteShutdown);
        this->quit();
    }

    void CNetworkWatchdog::pingDbClientService(CGlobalSetup::PingType type, bool force)
    {
        if (!sApp || sApp->isShuttingDown()) { return; }
        if (!force && !this->isSwiftDbAccessible()) { return; }
        const CGlobalSetup gs = sApp->getGlobalSetup();
        if (!gs.wasLoadedFromWeb()) { return; }
        const CUrl pingUrl = gs.getDbClientPingServiceUrl(type);
        sApp->getFromNetwork(pingUrl, { this, &CNetworkWatchdog::replyPingClientService });
    }

    bool CNetworkWatchdog::disableNetworkAccessibilityCheck(bool disable)
    {
        if (disable == m_disableNetworkCheck) { return false; }
        m_disableNetworkCheck = disable;
        return true;
    }

    void CNetworkWatchdog::replyPingClientService(QNetworkReply *nwReply)
    {
        // init and clean up
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nw(nwReply); // delete reply
        const bool ok = (nw->error() == QNetworkReply::NoError);
        const QString errorString = nw->errorString();
        const QString url = nw->url().toString();
        nw->close();

        if (!sApp || sApp->isShuttingDown()) { return; }
        Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "Wrong thread");

        m_lastClientPingSuccess = ok;
        {
            QWriteLocker l(&m_lockUrl);
            m_lastPingUrl = url;
        }

        if (ok)
        {
            // be a little less verbose
            if ((m_totalGoodCountDb % 5 == 0) || m_consecutivePingBadCount > 0)
            {
                CLogMessage(this).info(u"Watchdog pinged '%1'")  << url;
            }
            m_totalGoodCountDb++;
            m_consecutivePingBadCount = 0;
        }
        else
        {
            m_totalBadCountDb++;
            m_consecutivePingBadCount++;
            if (m_logOwnMessages)
            {
                CStatusMessage(this).warning(u"Watchdog ping failed, error: '%1', total good/bad DB counts: %2/%3") << errorString << m_totalGoodCountDb << m_totalBadCountDb;
            }
        }
        this->setDbAccessibility(ok);
    }

    void CNetworkWatchdog::triggerChangedSignals(bool oldDbAccessible, bool oldInternetAccessible)
    {
        if (!this->doWorkCheck()) { return; }

        if (oldDbAccessible != m_dbAccessible)
        {
            const CUrl testUrl(this->dbTestUrl());
            emit this->changedSwiftDbAccessibility(m_dbAccessible, testUrl);
        }
        if (oldInternetAccessible != m_internetAccessible)
        {
            emit this->changedInternetAccessibility(m_internetAccessible);
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
        if (!sApp || sApp->isShuttingDown()) { return CUrl(); }
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
