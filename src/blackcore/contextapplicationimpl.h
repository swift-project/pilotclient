/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORE_CONTEXTAPPLICATION_IMPL_H
#define BLACKCORE_CONTEXTAPPLICATION_IMPL_H

#include "blackcoreexport.h"
#include "contextapplication.h"
#include "corefacade.h"
#include "blackmisc/identifierlist.h"

namespace BlackMisc { class CDBusServer; }
namespace BlackCore
{
    class CCoreFacade;

    //! Application context
    class BLACKCORE_EXPORT CContextApplication : public IContextApplication
    {
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME)
        Q_OBJECT
        friend class CCoreFacade;
        friend class IContextApplication;

    public slots:
        //! \copydoc IContextApplication::logMessage
        virtual void logMessage(const BlackMisc::CStatusMessage &message, const BlackMisc::CIdentifier &origin) override;

        //! \copydoc IContextApplication::addLogSubscription
        virtual void addLogSubscription(const BlackMisc::CIdentifier &subscriber, const BlackMisc::CLogPattern &pattern) override;

        //! \copydoc IContextApplication::removeLogSubscription
        virtual void removeLogSubscription(const BlackMisc::CIdentifier &subscriber, const BlackMisc::CLogPattern &pattern) override;

        //! \copydoc IContextApplication::getAllLogSubscriptions
        virtual CLogSubscriptionHash getAllLogSubscriptions() const override;

        //! \copydoc IContextApplication::synchronizeLogSubscriptions
        virtual void synchronizeLogSubscriptions() override;

        //! \copydoc IContextApplication::changeSettings
        virtual void changeSettings(const BlackMisc::CValueCachePacket &settings, const BlackMisc::CIdentifier &origin) override;

        //! \copydoc IContextApplication::getAllSettings
        virtual BlackMisc::CValueCachePacket getAllSettings() const override;

        //! \copydoc IContextApplication::synchronizeLocalSettings
        virtual void synchronizeLocalSettings() override;

        //! \copydoc IContextApplication::saveSettings
        virtual BlackMisc::CStatusMessage saveSettings(const QString &keyPrefix = {}) override;

        //! \copydoc IContextApplication::loadSettings
        virtual BlackMisc::CStatusMessage loadSettings() override;

        //! \copydoc IContextApplication::registerHotkeyActions
        virtual void registerHotkeyActions(const QStringList &actions, const BlackMisc::CIdentifier &origin) override;

        //! \copydoc IContextApplication::callHotkeyAction
        virtual void callHotkeyAction(const QString &action, bool argument, const BlackMisc::CIdentifier &origin) override;

        //! \copydoc IContextApplication::writeToFile
        virtual bool writeToFile(const QString &fileName, const QString &content) override;

        //! \copydoc IContextApplication::registerApplication
        virtual BlackMisc::CIdentifier registerApplication(const BlackMisc::CIdentifier &application) override;

        //! \copydoc IContextApplication::unregisterApplication
        virtual void unregisterApplication(const BlackMisc::CIdentifier &application) override;

        //! \copydoc IContextApplication::getRegisteredApplications
        virtual BlackMisc::CIdentifierList getRegisteredApplications() const override;

        //! \copydoc IContextApplication::readFromFile
        virtual QString readFromFile(const QString &fileName) const override;

        //! \copydoc IContextApplication::removeFile
        virtual bool removeFile(const QString &fileName) override;

        //! \copydoc IContextApplication::existsFile
        virtual bool existsFile(const QString &fileName) const override;

    public:
        //! \todo Remove with old settings
        using IContextApplication::changeSettings;

    protected:
        //! Constructor
        CContextApplication(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

        //! Register myself in DBus, fail safe
        CContextApplication *registerWithDBus(BlackMisc::CDBusServer *server);

    private:
        BlackMisc::CIdentifierList m_registeredApplications;
    };
} // namespace

#endif // guard
