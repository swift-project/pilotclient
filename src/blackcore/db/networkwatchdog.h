/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DB_NETWORKWATCHDOG_H
#define BLACKCORE_DB_NETWORKWATCHDOG_H

#include "blackcore/data/globalsetup.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/network/url.h"
#include "blackmisc/worker.h"
#include "blackmisc/logcategories.h"

#include <atomic>
#include <QReadWriteLock>
#include <QNetworkAccessManager>

namespace BlackCore
{
    namespace Db
    {
        //! Monitoring the swift DB, internet access, shared URLs
        class BLACKCORE_EXPORT CNetworkWatchdog : public BlackMisc::CContinuousWorker
        {
            Q_OBJECT

        public:
            //! Log categories
            static const QStringList &getLogCategories();

            //! Ctor
            explicit CNetworkWatchdog(bool networkAccessible, QObject *owner);

            //! Network status changed, use this function to inform the watchdog
            //! \threadsafe
            void setNetworkAccessibility(QNetworkAccessManager::NetworkAccessibility accessibility);

            //! Configuration updates completed as reported by QNetworkConfigurationManager::updateCompleted
            void networkConfigurationsUpdateCompleted();

            //! Set online as reported by QNetworkConfigurationManager::onlineStateChanged
            //! \threadsafe
            void setOnline(bool online);

            //! DB available?
            //! \threadsafe
            bool isSwiftDbAccessible() const { return m_dbAccessible; }

            //! Set DB as avialable (from external)
            //! \remark if data was read from DB, this can save another check
            //! \threadsafe
            void setDbAccessibility(bool accessible);

            //! DB is accessible
            //! \threadsafe
            void setDbIsAccessible() { this->setDbAccessibility(true); }

            //! DB is NOT accessible
            //! \threadsafe
            void setDbIsNotAccessible() { this->setDbAccessibility(false); }

            //! Check the DB availability, can disable the check
            //! \threadsafe
            void setCheckDbAccessibility(bool check) { m_checkDbAccessibility = check; }

            //! Check the shared URL, can disable the check
            //! \threadsafe
            void setCheckSharedUrl(bool check) { m_checkSharedUrl = check; }

            //! Do a detailed check via HTTP
            //! \threadsafe
            void setDoDetailedCheck(bool check) { m_doDetailedCheck = check; }

            //! Internet available?
            //! \threadsafe
            bool isInternetAccessible() const { return m_internetAccessible; }

            //! Accesible or check disabled?
            bool isNetworkkAccessibleOrCheckDisabled() const { return m_networkAccessible || m_disableNetworkCheck; }

            //! Has working shared URL?
            //! \threadsafe
            bool hasWorkingSharedUrl() const;

            //! A working shared URL
            //! \threadsafe
            BlackMisc::Network::CUrl getWorkingSharedUrl() const;

            //! Log.own status messages
            //! \threadsafe
            void setLogOwnMessages(bool log) { m_logOwnMessages = log; }

            //! Run a check
            int triggerCheck();

            //! Number of completed checks
            //! \threadsafe
            int getCheckCount() const { return m_totalCheckCount; }

            //! Last URL used for ping /DB ping service)
            QString getLastPingDbUrl() const;

            //! Number of completed checks
            //! \threadsafe
            QString getCheckInfo() const;

            //! Set working URL from external
            //! \threadsafe
            void setWorkingSharedUrl(const BlackMisc::Network::CUrl &workingUrl);

            //! Graceful shutdown
            void gracefulShutdown();

            //! Ping the DB server, fire and forget (no feedback etc)
            void pingDbClientService(Data::CGlobalSetup::PingType type = Data::CGlobalSetup::PingUnspecific, bool force = false);

            //! Disable the network check
            //! \remark if disabled network reports always accessible
            //! \threadsafe
            bool disableNetworkAccessibilityCheck(bool disable);

            //! Has network check been disabled?
            //! \threadsafe
            bool isNetworkAccessibilityCheckDisabled() const { return m_disableNetworkCheck; }

            //! Network check enabled?
            //! \threadsafe
            bool isNetworkAccessibilityCheckEnabled() const { return !this->isNetworkAccessibilityCheckDisabled(); }

            //! URL referring to the DB
            //! \remark depends on BlackCore::Application::getGlobalSetup()
            static bool isDbUrl(const BlackMisc::Network::CUrl &url);

            //! The URL being tested
            //! \remark depends on BlackCore::Application::getGlobalSetup()
            //! \private primarily accessible for unit tests
            static BlackMisc::Network::CUrl dbTestUrl();

        signals:
            //! DB was available, but not longer is and vice versa
            void changedSwiftDbAccessibility(bool available, const BlackMisc::Network::CUrl &url);

            //! Internet was available, but not longer is and vice versa
            void changedInternetAccessibility(bool available);

            //! Cleaned version of QNetworkAccessManager::networkAccessibleChanged
            //! \remark does only fire if the accessibility changed
            void changedNetworkAccessible(QNetworkAccessManager::NetworkAccessibility accessible);

        private:
            static constexpr int CanConnectTimeMs = 5000;

            //! Do work, i.e. check connectivity
            void doWork();

            //! Do check
            bool doWorkCheck() const;

            //! Trigger the changed signals and avoid unneccessary signals
            void triggerChangedSignals(bool oldDbAccessible, bool oldInternetAccessible);

            //! Init a working shared URL
            void initWorkingSharedUrlFromSetup();

            //! Received reply of client service ping
            void replyPingClientService(QNetworkReply *nwReply);

            //! The DB server
            //! \remark depends on BlackCore::Application::getGlobalSetup()
            static QString dbHost();

            //! Obtain working DB data file location URL
            //! \remark depends on BlackCore::Application::getGlobalSetup()
            static BlackMisc::Network::CUrl workingSharedUrlFromSetup();

            std::atomic_bool m_logOwnMessages { true };
            std::atomic_bool m_doDetailedCheck { true };
            std::atomic_bool m_networkAccessible { true };
            std::atomic_bool m_disableNetworkCheck { false };  //!< if this is true, network accessible always reports true/accessible
            std::atomic_bool m_online { true };
            std::atomic_bool m_internetAccessible { true };
            std::atomic_bool m_dbAccessible { true };
            std::atomic_bool m_lastClientPingSuccess { true }; //!< ping swift DB client service, real HTTP response
            std::atomic_bool m_checkDbAccessibility { true };
            std::atomic_bool m_checkSharedUrl { true };
            std::atomic_bool m_checkInProgress { false };   //!< a check is currently in progress
            std::atomic<qint64> m_nextPingSecsSinceEpoch { 0 }; //!< time at which next ping will be sent
            std::atomic_int  m_networkAccessibility { QNetworkAccessManager::Accessible }; //!< last state
            std::atomic_int  m_totalCheckCount { 0 };       //!< counting number of checks
            std::atomic_int  m_totalBadCountDb { 0 };       //!< Total number of DB failing counts (only real responses when tried)
            std::atomic_int  m_totalBadCountInternet { 0 }; //!< Total number of Internet failing count (only when network is accessible)
            std::atomic_int  m_totalGoodCountDb { 0 };
            std::atomic_int  m_totalGoodCountInternet  { 0 };
            std::atomic_int  m_consecutivePingBadCount { 0 }; //!< Bad count of ping until a godd state is received
            QString m_lastPingUrl;
            BlackMisc::Network::CUrl m_workingSharedUrl;
            mutable QReadWriteLock m_lockUrl;
        };
    } // ns
} // ns

#endif // guard
