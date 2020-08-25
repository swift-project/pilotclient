/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTAPPLICATION_H
#define BLACKCORE_CONTEXT_CONTEXTAPPLICATION_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/context.h"
#include "blackcore/corefacadeconfig.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/identifier.h"
#include "blackmisc/identifierlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/valuecache.h"

#include <QDBusArgument>
#include <QHash>
#include <QList>
#include <QMetaType>
#include <QObject>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QtGlobal>

// clazy:excludeall=const-signal-or-slot

//! \addtogroup dbus
//! @{

//! DBus interface for context
#define BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME "org.swift_project.blackcore.contextapplication"

//! DBus object path for context
#define BLACKCORE_CONTEXTAPPLICATION_OBJECTPATH "/application"

//! @}

class QDBusConnection;
namespace BlackMisc { class CDBusServer; }
namespace BlackCore
{
    class CCoreFacade;
    class CInputManager;

    namespace Context
    {
        //! Value type for settings keys with descriptions
        using CSettingsDictionary = BlackMisc::CDictionary<QString, QString, QMap>;

        //! Application context interface
        class BLACKCORE_EXPORT IContextApplication : public IContext
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME)

        public:
            //! Service name
            static const QString &InterfaceName()
            {
                static QString s(BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME);
                return s;
            }

            //! Service path
            static const QString &ObjectPath()
            {
                static QString s(BLACKCORE_CONTEXTAPPLICATION_OBJECTPATH);
                return s;
            }

            //! \copydoc IContext::getPathAndContextId()
            virtual QString getPathAndContextId() const override { return this->buildPathAndContextId(ObjectPath()); }

            //! Factory method
            static IContextApplication *create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &connection);

            //! Destructor
            virtual ~IContextApplication() override {}

        signals:
            //! A component changes
            void registrationChanged();

            //! One or more settings were changed
            //! \note Used for cache relay, do not use directly
            void settingsChanged(const BlackMisc::CValueCachePacket &settings, const BlackMisc::CIdentifier &origin);

            //! New action was registered
            //! \note Used to register hotkey action, do not use directly
            void hotkeyActionsRegistered(const QStringList &actions, const BlackMisc::CIdentifier &origin);

            //! Call a hotkey action on a remote process
            //! \note Used for hotkey action, do not use directly
            void remoteHotkeyAction(const QString &action, bool argument, const BlackMisc::CIdentifier &origin);

            //! Request a message to be displayed on console, whatever the console is
            //! \note no guarantee it is really written to console
            void requestDisplayOnConsole(const QString &message);

        public slots:
            //! Ratify some settings changed by another process
            //! \note Not pure because it can be called from the base class constructor.
            //! \note This is the function which relays cache changes via DBus.
            virtual void changeSettings(const BlackMisc::CValueCachePacket &settings, const BlackMisc::CIdentifier &origin);

            //! Get all settings currently in core settings cache
            virtual BlackMisc::CValueCachePacket getAllSettings() const = 0;

            //! Get keys of all unsaved settings currently in core settings cache
            virtual QStringList getUnsavedSettingsKeys() const = 0;

            //! Get keys and descriptions of all unsaved settings currently in core settings cache
            virtual BlackCore::Context::CSettingsDictionary getUnsavedSettingsKeysDescribed() const = 0;

            //! Update local settings with settings from core
            virtual void synchronizeLocalSettings() = 0;

            //! Save core settings to disk
            virtual BlackMisc::CStatusMessage saveSettings(const QString &keyPrefix = {}) = 0;

            //! Save core settings to disk
            virtual BlackMisc::CStatusMessage saveSettingsByKey(const QStringList &keys) = 0;

            //! Load core settings from disk
            virtual BlackMisc::CStatusMessage loadSettings() = 0;

            //! Register hotkey action implemented by another process
            //! \note Not pure because it can be called from the base class constructor.
            //! \note This is the function which relays action registrations via DBus
            virtual void registerHotkeyActions(const QStringList &actions, const BlackMisc::CIdentifier &origin);

            //! Call a hotkey action on a remote process
            //! \note Not pure because it can be called from the base class constructor.
            //! \note This is the function which relays action calls via DBus
            virtual void callHotkeyActionRemotely(const QString &action, bool argument, const BlackMisc::CIdentifier &origin);

            //! Register application, can also be used for ping
            virtual BlackMisc::CIdentifier registerApplication(const BlackMisc::CIdentifier &application) = 0;

            //! Unregister application
            virtual void unregisterApplication(const BlackMisc::CIdentifier &application) = 0;

            //! All registered applications
            virtual BlackMisc::CIdentifierList getRegisteredApplications() const = 0;

            //! Identifier of application, remote side if distributed
            virtual BlackMisc::CIdentifier getApplicationIdentifier() const = 0;

            //! Remote enabled version of writing a text file
            virtual bool writeToFile(const QString &fileName, const QString &content) = 0;

            //! Remote enabled version of reading a text file
            virtual QString readFromFile(const QString &fileName) const = 0;

            //! Remote enabled version of deleting a file
            virtual bool removeFile(const QString &fileName) = 0;

            //! Remote enabled version of file exists
            virtual bool existsFile(const QString &fileName) const = 0;

            //! Forward to facade
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

            //! The HTML help for dot commands
            virtual QString dotCommandsHtmlHelp() const = 0;

        protected:
            static constexpr int PingIdentifiersMs = 20000; //!< how often identifiers are pinged

            //! Constructor
            IContextApplication(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackCore::Context::CSettingsDictionary)

#endif // guard
