/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTAPPLICATION_EMPTY_H
#define BLACKCORE_CONTEXTAPPLICATION_EMPTY_H

#include "blackcoreexport.h"
#include "contextapplication.h"
#include "corefacade.h"
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

            //! \copydoc IContextApplication::writeToFile
            virtual bool writeToFile(const QString &fileName, const QString &content) override
            {
                Q_UNUSED(fileName);
                Q_UNUSED(content);
                logEmptyContextWarning(Q_FUNC_INFO);
                return false;
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

            //! \copydoc IContextApplication::readFromFile
            virtual QString readFromFile(const QString &fileName) const override
            {
                Q_UNUSED(fileName);
                logEmptyContextWarning(Q_FUNC_INFO);
                return QString();
            }

            //! \copydoc IContextApplication::removeFile
            virtual bool removeFile(const QString &fileName) override
            {
                Q_UNUSED(fileName);
                logEmptyContextWarning(Q_FUNC_INFO);
                return false;
            }

            //! \copydoc IContextApplication::existsFile
            virtual bool existsFile(const QString &fileName) const override
            {
                Q_UNUSED(fileName);
                logEmptyContextWarning(Q_FUNC_INFO);
                return false;
            }

            //! \copydoc IContextApplication::dotCommandsHtmlHelp
            virtual QString dotCommandsHtmlHelp() const override
            {
                logEmptyContextWarning(Q_FUNC_INFO);
                return QString();
            }
        };
    } // namespace
} // namespace
#endif // guard
