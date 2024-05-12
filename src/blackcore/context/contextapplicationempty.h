// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CONTEXTAPPLICATION_EMPTY_H
#define BLACKCORE_CONTEXTAPPLICATION_EMPTY_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/corefacade.h"
#include "blackmisc/identifierlist.h"

// clazy:excludeall=const-signal-or-slot

namespace BlackCore
{
    class CCoreFacade;
    namespace Context
    {
        //! Application context
        class BLACKCORE_EXPORT CContextApplicationEmpty : public IContextApplication
        {
            Q_OBJECT

        public:
            //! Constructor
            CContextApplicationEmpty(CCoreFacade *runtime) : IContextApplication(CCoreFacadeConfig::NotUsed, runtime) {}

        public slots:
            //! \copydoc IContextApplication::changeSettings
            virtual void changeSettings(const BlackMisc::CValueCachePacket &settings, const BlackMisc::CIdentifier &origin) override
            {
                Q_UNUSED(settings);
                Q_UNUSED(origin);
                logEmptyContextWarning(Q_FUNC_INFO);
            }

            //! \copydoc IContextApplication::getAllSettings
            virtual BlackMisc::CValueCachePacket getAllSettings() const override
            {
                logEmptyContextWarning(Q_FUNC_INFO);
                return BlackMisc::CValueCachePacket();
            }

            //! \copydoc IContextApplication::getUnsavedSettingsKeys
            virtual QStringList getUnsavedSettingsKeys() const override
            {
                logEmptyContextWarning(Q_FUNC_INFO);
                return QStringList();
            }

            //! \copydoc IContextApplication::getUnsavedSettingsKeys
            virtual BlackCore::Context::CSettingsDictionary getUnsavedSettingsKeysDescribed() const override
            {
                logEmptyContextWarning(Q_FUNC_INFO);
                return CSettingsDictionary();
            }

            //! \copydoc IContextApplication::synchronizeLocalSettings
            virtual void synchronizeLocalSettings() override
            {
                logEmptyContextWarning(Q_FUNC_INFO);
            }

            //! \copydoc IContextApplication::saveSettings
            virtual BlackMisc::CStatusMessage saveSettings(const QString &keyPrefix = {}) override
            {
                Q_UNUSED(keyPrefix);
                logEmptyContextWarning(Q_FUNC_INFO);
                return BlackMisc::CStatusMessage();
            }

            //! \copydoc IContextApplication::saveSettingsByKey
            virtual BlackMisc::CStatusMessage saveSettingsByKey(const QStringList &keys) override
            {
                Q_UNUSED(keys);
                logEmptyContextWarning(Q_FUNC_INFO);
                return BlackMisc::CStatusMessage();
            }

            //! \copydoc IContextApplication::loadSettings
            virtual BlackMisc::CStatusMessage loadSettings() override
            {
                logEmptyContextWarning(Q_FUNC_INFO);
                return BlackMisc::CStatusMessage();
            }

            //! \copydoc IContextApplication::registerHotkeyActions
            virtual void registerHotkeyActions(const QStringList &actions, const BlackMisc::CIdentifier &origin) override
            {
                Q_UNUSED(actions);
                Q_UNUSED(origin);
                logEmptyContextWarning(Q_FUNC_INFO);
            }

            //! \copydoc IContextApplication::callHotkeyActionRemotely
            virtual void callHotkeyActionRemotely(const QString &action, bool argument, const BlackMisc::CIdentifier &origin) override
            {
                Q_UNUSED(action);
                Q_UNUSED(argument);
                Q_UNUSED(origin);
                logEmptyContextWarning(Q_FUNC_INFO);
            }

            //! \copydoc IContextApplication::registerApplication
            virtual BlackMisc::CIdentifier registerApplication(const BlackMisc::CIdentifier &application) override
            {
                Q_UNUSED(application);
                logEmptyContextWarning(Q_FUNC_INFO);
                return BlackMisc::CIdentifier();
            }

            //! \copydoc IContextApplication::unregisterApplication
            virtual void unregisterApplication(const BlackMisc::CIdentifier &application) override
            {
                Q_UNUSED(application);
                logEmptyContextWarning(Q_FUNC_INFO);
            }

            //! \copydoc IContextApplication::getRegisteredApplications
            virtual BlackMisc::CIdentifierList getRegisteredApplications() const override
            {
                logEmptyContextWarning(Q_FUNC_INFO);
                return BlackMisc::CIdentifierList();
            }

            //! \copydoc IContextApplication::getApplicationIdentifier
            virtual BlackMisc::CIdentifier getApplicationIdentifier() const override
            {
                logEmptyContextWarning(Q_FUNC_INFO);
                return BlackMisc::CIdentifier();
            }
        };
    } // namespace
} // namespace
#endif // guard
