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
#include "context_application.h"
#include "context_runtime.h"
#include "dbus_server.h"
#include "blackmisc/identifierlist.h"

namespace BlackCore
{
    class CRuntime;

    //! Application context
    class BLACKCORE_EXPORT CContextApplication : public IContextApplication
    {
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME)
        Q_OBJECT
        friend class CRuntime;
        friend class IContextApplication;

    public slots:
        //! \copydoc IContextApplication::logMessage
        virtual void logMessage(const BlackMisc::CStatusMessage &message, const BlackMisc::CIdentifier &origin) override;

        //! \copydoc IContextApplication::changeSettings
        virtual void changeSettings(const BlackMisc::CVariantMap &settings, const BlackMisc::CIdentifier &origin) override;

        //! \copydoc IContextApplication::writeToFile
        virtual bool writeToFile(const QString &fileName, const QString &content) override;

        //! \copydoc IContextApplication::registerApplication
        virtual BlackMisc::CIdentifier registerApplication(const BlackMisc::CIdentifier &application) override;

        //! \copydoc IContextApplication::unRegisterApplication
        virtual void unregisterApplication(const BlackMisc::CIdentifier &application) override;

        //! \copydoc IContextApplication::getRegisteredApplications
        virtual BlackMisc::CIdentifierList getRegisteredApplications() const override;

        //! \copydoc IContextApplication::readFromFile
        virtual QString readFromFile(const QString &fileName) const override;

        //! \copydoc IContextApplication::removeFile
        virtual bool removeFile(const QString &fileName) override;

        //! \copydoc IContextApplication::existsFile
        virtual bool existsFile(const QString &fileName) const override;

        //! \copydoc IContextApplication::processHotkeyFuncEvent
        virtual void processHotkeyFuncEvent(const BlackMisc::Event::CEventHotkeyFunction &event) override;

    public:
        //! \todo Remove with old settings
        using IContextApplication::changeSettings;

    protected:
        //! Constructor
        CContextApplication(CRuntimeConfig::ContextMode mode, CRuntime *runtime);

        //! Register myself in DBus, fail safe
        CContextApplication *registerWithDBus(CDBusServer *server);

    private:
        BlackMisc::CIdentifierList m_registeredApplications;
    };
} // namespace

#endif // guard
