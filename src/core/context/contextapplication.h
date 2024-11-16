// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTAPPLICATION_H
#define SWIFT_CORE_CONTEXT_CONTEXTAPPLICATION_H

#include <QDBusArgument>
#include <QHash>
#include <QList>
#include <QMetaType>
#include <QObject>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QtGlobal>

#include "core/context/context.h"
#include "core/corefacadeconfig.h"
#include "core/swiftcoreexport.h"
#include "misc/dictionary.h"
#include "misc/identifier.h"
#include "misc/identifierlist.h"
#include "misc/mixin/mixincompare.h"
#include "misc/statusmessage.h"
#include "misc/valuecache.h"

// clazy:excludeall=const-signal-or-slot

//! \ingroup dbus
//! DBus interface for context
#define SWIFT_CORE_CONTEXTAPPLICATION_INTERFACENAME "org.swift_project.swift_core.contextapplication"

//! \ingroup dbus
//! DBus object path for context
#define SWIFT_CORE_CONTEXTAPPLICATION_OBJECTPATH "/application"

class QDBusConnection;
namespace swift::misc
{
    class CDBusServer;
}
namespace swift::core
{
    class CCoreFacade;
    class CInputManager;

    namespace context
    {
        //! Value type for settings keys with descriptions
        using CSettingsDictionary = swift::misc::CDictionary<QString, QString, QMap>;

        //! Application context interface
        class SWIFT_CORE_EXPORT IContextApplication : public IContext
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", SWIFT_CORE_CONTEXTAPPLICATION_INTERFACENAME)

        public:
            //! Service name
            static const QString &InterfaceName()
            {
                static QString s(SWIFT_CORE_CONTEXTAPPLICATION_INTERFACENAME);
                return s;
            }

            //! Service path
            static const QString &ObjectPath()
            {
                static QString s(SWIFT_CORE_CONTEXTAPPLICATION_OBJECTPATH);
                return s;
            }

            //! \copydoc IContext::getPathAndContextId()
            virtual QString getPathAndContextId() const override { return this->buildPathAndContextId(ObjectPath()); }

            //! Factory method
            static IContextApplication *create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode,
                                               swift::misc::CDBusServer *server, QDBusConnection &connection);

            //! Destructor
            virtual ~IContextApplication() override {}

        signals:
            //! A component changes
            void registrationChanged();

            //! One or more settings were changed
            //! \note Used for cache relay, do not use directly
            void settingsChanged(const swift::misc::CValueCachePacket &settings,
                                 const swift::misc::CIdentifier &origin);

            //! New action was registered
            //! \note Used to register hotkey action, do not use directly
            void hotkeyActionsRegistered(const QStringList &actions, const swift::misc::CIdentifier &origin);

            //! Call a hotkey action on a remote process
            //! \note Used for hotkey action, do not use directly
            void remoteHotkeyAction(const QString &action, bool argument, const swift::misc::CIdentifier &origin);

        public slots:
            //! Ratify some settings changed by another process
            //! \note Not pure because it can be called from the base class constructor.
            //! \note This is the function which relays cache changes via DBus.
            virtual void changeSettings(const swift::misc::CValueCachePacket &settings,
                                        const swift::misc::CIdentifier &origin);

            //! Get all settings currently in core settings cache
            virtual swift::misc::CValueCachePacket getAllSettings() const = 0;

            //! Get keys of all unsaved settings currently in core settings cache
            virtual QStringList getUnsavedSettingsKeys() const = 0;

            //! Get keys and descriptions of all unsaved settings currently in core settings cache
            virtual swift::core::context::CSettingsDictionary getUnsavedSettingsKeysDescribed() const = 0;

            //! Update local settings with settings from core
            virtual void synchronizeLocalSettings() = 0;

            //! Save core settings to disk
            virtual swift::misc::CStatusMessage saveSettings(const QString &keyPrefix = {}) = 0;

            //! Save core settings to disk
            virtual swift::misc::CStatusMessage saveSettingsByKey(const QStringList &keys) = 0;

            //! Load core settings from disk
            virtual swift::misc::CStatusMessage loadSettings() = 0;

            //! Register hotkey action implemented by another process
            //! \note Not pure because it can be called from the base class constructor.
            //! \note This is the function which relays action registrations via DBus
            virtual void registerHotkeyActions(const QStringList &actions, const swift::misc::CIdentifier &origin);

            //! Call a hotkey action on a remote process
            //! \note Not pure because it can be called from the base class constructor.
            //! \note This is the function which relays action calls via DBus
            virtual void callHotkeyActionRemotely(const QString &action, bool argument,
                                                  const swift::misc::CIdentifier &origin);

            //! Register application, can also be used for ping
            virtual swift::misc::CIdentifier registerApplication(const swift::misc::CIdentifier &application) = 0;

            //! Unregister application
            virtual void unregisterApplication(const swift::misc::CIdentifier &application) = 0;

            //! All registered applications
            virtual swift::misc::CIdentifierList getRegisteredApplications() const = 0;

            //! Identifier of application, remote side if distributed
            virtual swift::misc::CIdentifier getApplicationIdentifier() const = 0;

            //! Forward to facade
            virtual bool parseCommandLine(const QString &commandLine,
                                          const swift::misc::CIdentifier &originator) override;

        protected:
            static constexpr int PingIdentifiersMs = 20000; //!< how often identifiers are pinged

            //! Constructor
            IContextApplication(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);
        };
    } // namespace context
} // namespace swift::core

Q_DECLARE_METATYPE(swift::core::context::CSettingsDictionary)

#endif // guard
