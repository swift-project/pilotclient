/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORE_CONTEXTAPPLICATION_EMPTY_H
#define BLACKCORE_CONTEXTAPPLICATION_EMPTY_H

#include "blackcoreexport.h"
#include "context_application.h"
#include "context_runtime.h"
#include "blackmisc/identifierlist.h"

namespace BlackCore
{
    class CRuntime;

    //! Application context
    class BLACKCORE_EXPORT CContextApplicationEmpty : public IContextApplication
    {
    public:
        //! Constructor
        CContextApplicationEmpty(CRuntime *runtime) : IContextApplication(CRuntimeConfig::NotUsed, runtime) {}

    public slots:
        //! \copydoc IContextApplication::logMessage
        virtual void logMessage(const BlackMisc::CStatusMessage &message, const BlackMisc::CIdentifier &origin) override
        {
            Q_UNUSED(message);
            Q_UNUSED(origin);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextApplication::addLogSubscription
        virtual void addLogSubscription(const BlackMisc::CIdentifier &subscriber, const BlackMisc::CLogPattern &pattern) override
        {
            Q_UNUSED(subscriber);
            Q_UNUSED(pattern);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextApplication::removeLogSubscription
        virtual void removeLogSubscription(const BlackMisc::CIdentifier &subscriber, const BlackMisc::CLogPattern &pattern) override
        {
            Q_UNUSED(subscriber);
            Q_UNUSED(pattern);
            logEmptyContextWarning(Q_FUNC_INFO);

        }

        //! \copydoc IContextApplication::getAllLogSubscriptions
        virtual CLogSubscriptionHash getAllLogSubscriptions() const
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return CLogSubscriptionHash();
        }

        //! \copydoc IContextApplication::synchronizeLogSubscriptions
        virtual void synchronizeLogSubscriptions()
        {
            logEmptyContextWarning(Q_FUNC_INFO);
        }

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

        //! \copydoc IContextApplication::callHotkeyAction
        virtual void callHotkeyAction(const QString &action, bool argument, const BlackMisc::CIdentifier &origin) override
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

        //! \copydoc IContextApplication::unRegisterApplication
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
    };
} // namespace

#endif // guard
