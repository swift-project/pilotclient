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
#include "blackmisc/identifiable.h"
#include "blackmisc/identifierlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/valuecache.h"

// clazy:excludeall=const-signal-or-slot

namespace BlackMisc
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
            public BlackMisc::CIdentifiable
        {
            Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME)
            Q_OBJECT
            friend class BlackCore::CCoreFacade;
            friend class IContextApplication;

        public slots:
            //! \copydoc BlackCore::Context::IContextApplication::changeSettings
            virtual void changeSettings(const BlackMisc::CValueCachePacket &settings, const BlackMisc::CIdentifier &origin) override;

            //! \copydoc BlackCore::Context::IContextApplication::getAllSettings
            virtual BlackMisc::CValueCachePacket getAllSettings() const override;

            //! \copydoc BlackCore::Context::IContextApplication::getUnsavedSettingsKeys
            virtual QStringList getUnsavedSettingsKeys() const override;

            //! \copydoc BlackCore::Context::IContextApplication::getUnsavedSettingsKeysDescribed
            virtual BlackCore::Context::CSettingsDictionary getUnsavedSettingsKeysDescribed() const override;

            //! \copydoc BlackCore::Context::IContextApplication::synchronizeLocalSettings
            virtual void synchronizeLocalSettings() override;

            //! \copydoc BlackCore::Context::IContextApplication::saveSettings
            virtual BlackMisc::CStatusMessage saveSettings(const QString &keyPrefix = {}) override;

            //! \copydoc BlackCore::Context::IContextApplication::saveSettingsByKey
            virtual BlackMisc::CStatusMessage saveSettingsByKey(const QStringList &keys) override;

            //! \copydoc BlackCore::Context::IContextApplication::loadSettings
            virtual BlackMisc::CStatusMessage loadSettings() override;

            //! \copydoc BlackCore::Context::IContextApplication::registerHotkeyActions
            virtual void registerHotkeyActions(const QStringList &actions, const BlackMisc::CIdentifier &origin) override;

            //! \copydoc BlackCore::Context::IContextApplication::callHotkeyActionRemotely
            virtual void callHotkeyActionRemotely(const QString &action, bool argument, const BlackMisc::CIdentifier &origin) override;

            //! \copydoc BlackCore::Context::IContextApplication::registerApplication
            virtual BlackMisc::CIdentifier registerApplication(const BlackMisc::CIdentifier &application) override;

            //! \copydoc BlackCore::Context::IContextApplication::unregisterApplication
            virtual void unregisterApplication(const BlackMisc::CIdentifier &application) override;

            //! \copydoc BlackCore::Context::IContextApplication::getRegisteredApplications
            virtual BlackMisc::CIdentifierList getRegisteredApplications() const override;

            //! \copydoc BlackCore::Context::IContextApplication::getApplicationIdentifier
            virtual BlackMisc::CIdentifier getApplicationIdentifier() const override;

        protected:
            //! Constructor
            CContextApplication(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

            //! Register myself in DBus, fail safe
            CContextApplication *registerWithDBus(BlackMisc::CDBusServer *server);

        private:
            BlackMisc::CIdentifierList m_registeredApplications;

            //! Housekeeping
            void cleanupRegisteredApplications();
        };
    } // namespace
} // namespace
#endif // guard
