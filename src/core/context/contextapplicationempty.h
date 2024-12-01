// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXTAPPLICATION_EMPTY_H
#define SWIFT_CORE_CONTEXTAPPLICATION_EMPTY_H

#include "core/context/contextapplication.h"
#include "core/corefacade.h"
#include "core/swiftcoreexport.h"
#include "misc/identifierlist.h"

// clazy:excludeall=const-signal-or-slot

namespace swift::core
{
    class CCoreFacade;
    namespace context
    {
        //! Application context
        class SWIFT_CORE_EXPORT CContextApplicationEmpty : public IContextApplication
        {
            Q_OBJECT

        public:
            //! Constructor
            CContextApplicationEmpty(CCoreFacade *runtime) : IContextApplication(CCoreFacadeConfig::NotUsed, runtime) {}

        public slots:
            //! \copydoc IContextApplication::changeSettings
            virtual void changeSettings(const swift::misc::CValueCachePacket &settings,
                                        const swift::misc::CIdentifier &origin) override
            {
                Q_UNUSED(settings);
                Q_UNUSED(origin);
                logEmptyContextWarning(Q_FUNC_INFO);
            }

            //! \copydoc IContextApplication::getAllSettings
            virtual swift::misc::CValueCachePacket getAllSettings() const override
            {
                logEmptyContextWarning(Q_FUNC_INFO);
                return swift::misc::CValueCachePacket();
            }

            //! \copydoc IContextApplication::getUnsavedSettingsKeys
            virtual QStringList getUnsavedSettingsKeys() const override
            {
                logEmptyContextWarning(Q_FUNC_INFO);
                return QStringList();
            }

            //! \copydoc IContextApplication::getUnsavedSettingsKeys
            virtual swift::core::context::CSettingsDictionary getUnsavedSettingsKeysDescribed() const override
            {
                logEmptyContextWarning(Q_FUNC_INFO);
                return CSettingsDictionary();
            }

            //! \copydoc IContextApplication::synchronizeLocalSettings
            virtual void synchronizeLocalSettings() override { logEmptyContextWarning(Q_FUNC_INFO); }

            //! \copydoc IContextApplication::saveSettings
            virtual swift::misc::CStatusMessage saveSettings(const QString &keyPrefix = {}) override
            {
                Q_UNUSED(keyPrefix);
                logEmptyContextWarning(Q_FUNC_INFO);
                return swift::misc::CStatusMessage();
            }

            //! \copydoc IContextApplication::saveSettingsByKey
            virtual swift::misc::CStatusMessage saveSettingsByKey(const QStringList &keys) override
            {
                Q_UNUSED(keys);
                logEmptyContextWarning(Q_FUNC_INFO);
                return swift::misc::CStatusMessage();
            }

            //! \copydoc IContextApplication::loadSettings
            virtual swift::misc::CStatusMessage loadSettings() override
            {
                logEmptyContextWarning(Q_FUNC_INFO);
                return swift::misc::CStatusMessage();
            }

            //! \copydoc IContextApplication::registerHotkeyActions
            virtual void registerHotkeyActions(const QStringList &actions,
                                               const swift::misc::CIdentifier &origin) override
            {
                Q_UNUSED(actions);
                Q_UNUSED(origin);
                logEmptyContextWarning(Q_FUNC_INFO);
            }

            //! \copydoc IContextApplication::callHotkeyActionRemotely
            virtual void callHotkeyActionRemotely(const QString &action, bool argument,
                                                  const swift::misc::CIdentifier &origin) override
            {
                Q_UNUSED(action);
                Q_UNUSED(argument);
                Q_UNUSED(origin);
                logEmptyContextWarning(Q_FUNC_INFO);
            }

            //! \copydoc IContextApplication::registerApplication
            virtual swift::misc::CIdentifier registerApplication(const swift::misc::CIdentifier &application) override
            {
                Q_UNUSED(application);
                logEmptyContextWarning(Q_FUNC_INFO);
                return swift::misc::CIdentifier();
            }

            //! \copydoc IContextApplication::unregisterApplication
            virtual void unregisterApplication(const swift::misc::CIdentifier &application) override
            {
                Q_UNUSED(application);
                logEmptyContextWarning(Q_FUNC_INFO);
            }

            //! \copydoc IContextApplication::getRegisteredApplications
            virtual swift::misc::CIdentifierList getRegisteredApplications() const override
            {
                logEmptyContextWarning(Q_FUNC_INFO);
                return swift::misc::CIdentifierList();
            }

            //! \copydoc IContextApplication::getApplicationIdentifier
            virtual swift::misc::CIdentifier getApplicationIdentifier() const override
            {
                logEmptyContextWarning(Q_FUNC_INFO);
                return swift::misc::CIdentifier();
            }
        };
    } // namespace context
} // namespace swift::core
#endif // SWIFT_CORE_CONTEXTAPPLICATION_EMPTY_H
