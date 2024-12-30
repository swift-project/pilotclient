// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTAPPLICATION_IMPL_H
#define SWIFT_CORE_CONTEXT_CONTEXTAPPLICATION_IMPL_H

#include <QObject>
#include <QString>
#include <QStringList>

#include "core/context/contextapplication.h"
#include "core/corefacadeconfig.h"
#include "core/swiftcoreexport.h"
#include "misc/identifiable.h"
#include "misc/identifierlist.h"
#include "misc/statusmessage.h"
#include "misc/valuecache.h"

// clazy:excludeall=const-signal-or-slot

namespace swift::misc
{
    class CDBusServer;
}

namespace swift::core
{
    class CCoreFacade;
    namespace context
    {
        //! Application context
        class SWIFT_CORE_EXPORT CContextApplication : public IContextApplication, public swift::misc::CIdentifiable
        {
            Q_CLASSINFO("D-Bus Interface", SWIFT_CORE_CONTEXTAPPLICATION_INTERFACENAME)
            Q_OBJECT
            friend class swift::core::CCoreFacade;
            friend class IContextApplication;

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

        protected:
            //! Constructor
            CContextApplication(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

        private:
            swift::misc::CIdentifierList m_registeredApplications;

            //! Housekeeping
            void cleanupRegisteredApplications();
        };
    } // namespace context
} // namespace swift::core
#endif // SWIFT_CORE_CONTEXT_CONTEXTAPPLICATION_IMPL_H
