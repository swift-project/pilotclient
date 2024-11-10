// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTAPPLICATION_PROXY_H
#define BLACKCORE_CONTEXT_CONTEXTAPPLICATION_PROXY_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/corefacadeconfig.h"
#include "misc/statusmessage.h"
#include "misc/valuecache.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QSet>

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;

namespace swift::misc
{
    class CGenericDBusInterface;
}

namespace BlackCore
{
    class CCoreFacade;
    namespace Context
    {
        //! Application context proxy
        //! \ingroup dbus
        class BLACKCORE_EXPORT CContextApplicationProxy : public IContextApplication
        {
            Q_OBJECT
            friend class IContextApplication;

        public:
            //! Destructor
            virtual ~CContextApplicationProxy() override {}

        public slots:
            //! \copydoc BlackCore::Context::IContextApplication::changeSettings
            virtual void changeSettings(const swift::misc::CValueCachePacket &settings, const swift::misc::CIdentifier &origin) override;

            //! \copydoc BlackCore::Context::IContextApplication::getAllSettings
            virtual swift::misc::CValueCachePacket getAllSettings() const override;

            //! \copydoc BlackCore::Context::IContextApplication::getUnsavedSettingsKeys
            virtual QStringList getUnsavedSettingsKeys() const override;

            //! \copydoc BlackCore::Context::IContextApplication::getUnsavedSettingsKeysDescribed
            virtual BlackCore::Context::CSettingsDictionary getUnsavedSettingsKeysDescribed() const override;

            //! \copydoc BlackCore::Context::IContextApplication::synchronizeLocalSettings
            virtual void synchronizeLocalSettings() override;

            //! \copydoc BlackCore::Context::IContextApplication::saveSettings
            virtual swift::misc::CStatusMessage saveSettings(const QString &keyPrefix = {}) override;

            //! \copydoc BlackCore::Context::IContextApplication::saveSettingsByKey
            virtual swift::misc::CStatusMessage saveSettingsByKey(const QStringList &keys) override;

            //! \copydoc BlackCore::Context::IContextApplication::loadSettings
            virtual swift::misc::CStatusMessage loadSettings() override;

            //! \copydoc BlackCore::Context::IContextApplication::registerHotkeyActions
            virtual void registerHotkeyActions(const QStringList &actions, const swift::misc::CIdentifier &origin) override;

            //! \copydoc BlackCore::Context::IContextApplication::callHotkeyActionRemotely
            virtual void callHotkeyActionRemotely(const QString &action, bool argument, const swift::misc::CIdentifier &origin) override;

            //! \copydoc BlackCore::Context::IContextApplication::registerApplication
            virtual swift::misc::CIdentifier registerApplication(const swift::misc::CIdentifier &application) override;

            //! \copydoc BlackCore::Context::IContextApplication::unregisterApplication
            virtual void unregisterApplication(const swift::misc::CIdentifier &application) override;

            //! \copydoc BlackCore::Context::IContextApplication::getRegisteredApplications
            virtual swift::misc::CIdentifierList getRegisteredApplications() const override;

            //! \copydoc BlackCore::Context::IContextApplication::getApplicationIdentifier
            virtual swift::misc::CIdentifier getApplicationIdentifier() const override;

            //! Used to test if there is a core running?
            //! \note creates and connects via proxy object, so not meant for very frequent tests
            //! \sa CDBusServer::isDBusAvailable as lightweight, but less accurate alternative
            static bool isContextResponsive(const QString &dbusAddress, QString &msg, int timeoutMs = 1500);

        protected:
            //! Constructor
            CContextApplicationProxy(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextApplication(mode, runtime), m_dBusInterface(nullptr) {}

            //! DBus version constructor
            CContextApplicationProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

        private:
            swift::misc::CGenericDBusInterface *m_dBusInterface = nullptr; //!< interface
            QSet<swift::misc::CIdentifier> m_proxyPingIdentifiers; //!< automatically ping the implementing side
            QTimer m_pingTimer;

            //! Relay connection signals to local signals
            void relaySignals(const QString &serviceName, QDBusConnection &connection);

            //! Ping/heartbeat identifiers
            void reRegisterApplications();

            void processRemoteHotkeyActionCall(const QString &action, bool argument, const swift::misc::CIdentifier &origin);
        };
    } // ns
} // ns

#endif // guard
