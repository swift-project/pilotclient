// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTAPPLICATION_PROXY_H
#define SWIFT_CORE_CONTEXT_CONTEXTAPPLICATION_PROXY_H

#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QTimer>

#include "core/context/contextapplication.h"
#include "core/corefacadeconfig.h"
#include "core/swiftcoreexport.h"
#include "misc/statusmessage.h"
#include "misc/valuecache.h"

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;

namespace swift::misc
{
    class CGenericDBusInterface;
}

namespace swift::core
{
    class CCoreFacade;
    namespace context
    {
        //! Application context proxy
        //! \ingroup dbus
        class SWIFT_CORE_EXPORT CContextApplicationProxy : public IContextApplication
        {
            Q_OBJECT
            friend class IContextApplication;

        public:
            //! Destructor
            virtual ~CContextApplicationProxy() override {}

        public slots:
            //! \copydoc swift::core::context::IContextApplication::changeSettings
            virtual void changeSettings(const swift::misc::CValueCachePacket &settings,
                                        const swift::misc::CIdentifier &origin) override;

            //! \copydoc swift::core::context::IContextApplication::getAllSettings
            virtual swift::misc::CValueCachePacket getAllSettings() const override;

            //! \copydoc swift::core::context::IContextApplication::getUnsavedSettingsKeys
            virtual QStringList getUnsavedSettingsKeys() const override;

            //! \copydoc swift::core::context::IContextApplication::getUnsavedSettingsKeysDescribed
            virtual swift::core::context::CSettingsDictionary getUnsavedSettingsKeysDescribed() const override;

            //! \copydoc swift::core::context::IContextApplication::synchronizeLocalSettings
            virtual void synchronizeLocalSettings() override;

            //! \copydoc swift::core::context::IContextApplication::saveSettings
            virtual swift::misc::CStatusMessage saveSettings(const QString &keyPrefix = {}) override;

            //! \copydoc swift::core::context::IContextApplication::saveSettingsByKey
            virtual swift::misc::CStatusMessage saveSettingsByKey(const QStringList &keys) override;

            //! \copydoc swift::core::context::IContextApplication::loadSettings
            virtual swift::misc::CStatusMessage loadSettings() override;

            //! \copydoc swift::core::context::IContextApplication::registerHotkeyActions
            virtual void registerHotkeyActions(const QStringList &actions,
                                               const swift::misc::CIdentifier &origin) override;

            //! \copydoc swift::core::context::IContextApplication::callHotkeyActionRemotely
            virtual void callHotkeyActionRemotely(const QString &action, bool argument,
                                                  const swift::misc::CIdentifier &origin) override;

            //! \copydoc swift::core::context::IContextApplication::registerApplication
            virtual swift::misc::CIdentifier registerApplication(const swift::misc::CIdentifier &application) override;

            //! \copydoc swift::core::context::IContextApplication::unregisterApplication
            virtual void unregisterApplication(const swift::misc::CIdentifier &application) override;

            //! \copydoc swift::core::context::IContextApplication::getRegisteredApplications
            virtual swift::misc::CIdentifierList getRegisteredApplications() const override;

            //! \copydoc swift::core::context::IContextApplication::getApplicationIdentifier
            virtual swift::misc::CIdentifier getApplicationIdentifier() const override;

            //! Used to test if there is a core running?
            //! \note creates and connects via proxy object, so not meant for very frequent tests
            //! \sa CDBusServer::isDBusAvailable as lightweight, but less accurate alternative
            static bool isContextResponsive(const QString &dbusAddress, QString &msg, int timeoutMs = 1500);

        protected:
            //! Constructor
            CContextApplicationProxy(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime)
                : IContextApplication(mode, runtime), m_dBusInterface(nullptr)
            {}

            //! DBus version constructor
            CContextApplicationProxy(const QString &serviceName, QDBusConnection &connection,
                                     CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

        private:
            swift::misc::CGenericDBusInterface *m_dBusInterface = nullptr; //!< interface
            QSet<swift::misc::CIdentifier> m_proxyPingIdentifiers; //!< automatically ping the implementing side
            QTimer m_pingTimer;

            //! Relay connection signals to local signals
            void relaySignals(const QString &serviceName, QDBusConnection &connection);

            //! Ping/heartbeat identifiers
            void reRegisterApplications();

            void processRemoteHotkeyActionCall(const QString &action, bool argument,
                                               const swift::misc::CIdentifier &origin);
        };
    } // namespace context
} // namespace swift::core

#endif // SWIFT_CORE_CONTEXT_CONTEXTAPPLICATION_PROXY_H
