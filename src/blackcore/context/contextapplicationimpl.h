// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTAPPLICATION_IMPL_H
#define BLACKCORE_CONTEXT_CONTEXTAPPLICATION_IMPL_H

#include <QObject>
#include <QString>
#include <QStringList>

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/corefacadeconfig.h"
#include "misc/identifiable.h"
#include "misc/identifierlist.h"
#include "misc/statusmessage.h"
#include "misc/valuecache.h"

// clazy:excludeall=const-signal-or-slot

namespace swift::misc
{
    class CDBusServer;
}

namespace BlackCore
{
    class CCoreFacade;
    namespace Context
    {
        //! Application context
        class BLACKCORE_EXPORT CContextApplication :
            public IContextApplication,
            public swift::misc::CIdentifiable
        {
            Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME)
            Q_OBJECT
            friend class BlackCore::CCoreFacade;
            friend class IContextApplication;

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

        protected:
            //! Constructor
            CContextApplication(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

            //! Register myself in DBus, fail safe
            CContextApplication *registerWithDBus(swift::misc::CDBusServer *server);

        private:
            swift::misc::CIdentifierList m_registeredApplications;

            //! Housekeeping
            void cleanupRegisteredApplications();
        };
    } // namespace
} // namespace
#endif // guard
