/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DB_NETWORKWATCHDOG_H
#define BLACKCORE_DB_NETWORKWATCHDOG_H

#include "blackmisc/worker.h"
#include "blackmisc/network/url.h"
#include <atomic>
#include <QReadWriteLock>
#include <QNetworkAccessManager>

namespace BlackCore
{
    namespace Db
    {
        /**
         * Monitoring the swift DB, internet access, shared URL
         */
        class CNetworkWatchdog : public BlackMisc::CContinuousWorker
        {
            Q_OBJECT

        public:
            //! Ctor
            explicit CNetworkWatchdog(QObject *parent = nullptr);

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

            //! Internet available?
            //! \threadsafe
            bool isInternetAccessible() const { return m_internetAccessible; }

            //! A working shared URL
            //! \threadsafe
            BlackMisc::Network::CUrl getWorkingSharedUrl() const;

            //! Run a check
            int triggerCheck();

            //! Number of completed checks
            int getCheckCount() const { return m_checkCount; }

            //! Set working URL from external
            //! \threadsafe
            void setWorkingSharedUrl(const BlackMisc::Network::CUrl &workingUrl);

            //! Network status changed, use this function to inform the watchdog
            void onChangedNetworkAccessibility(QNetworkAccessManager::NetworkAccessibility accessible);

            //! URL referring to the DB
            //! \remark depends on BlackCore::Application::getGlobalSetup()
            static bool isDbUrl(const BlackMisc::Network::CUrl &url);

        signals:
            //! DB was available, but not longer is and vice versa
            void changedSwiftDbAccessibility(bool available);

            //! Internet was available, but not longer is and vice versa
            void changedInternetAccessibility(bool available);

        private:
            //! Do work, i.e. check connectivity
            void doWork();

            //! Do check
            bool doWorkCheck() const;

            //! Trigger the changed signals
            void triggerChangedSignals(bool oldDbAccessible, bool oldInternetAccessible);

            //! Init a working shared URL
            void initWorkingSharedUrlFromSetup();

            //! The URL being tested
            //! \remark depends on BlackCore::Application::getGlobalSetup()
            static BlackMisc::Network::CUrl dbTestUrl();

            //! The DB server
            //! \remark depends on BlackCore::Application::getGlobalSetup()
            static QString dbHost();

            //! Obtain working DB data file location URL
            //! \remark depends on BlackCore::Application::getGlobalSetup()
            static BlackMisc::Network::CUrl workingSharedUrlFromSetup();

            std::atomic_bool m_networkAccessible { true };
            std::atomic_bool m_internetAccessible { true };
            std::atomic_bool m_dbAccessible { true };
            std::atomic_bool m_checkDbAccessibility { true };
            std::atomic_bool m_checkSharedUrl { true };
            std::atomic_bool m_checkInProgress { false }; //!< a check is currently in progress
            std::atomic_int  m_checkCount { 0 }; //!< counting number of checks
            BlackMisc::Network::CUrl m_workingSharedUrl;
            mutable QReadWriteLock m_lockSharedUrl;
        };
    } // ns
} // ns
#endif // guard
