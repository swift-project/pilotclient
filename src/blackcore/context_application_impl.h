/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTAPPLICATION_IMPL_H
#define BLACKCORE_CONTEXTAPPLICATION_IMPL_H

#include "context_application.h"
#include "context_runtime.h"
#include "dbus_server.h"

namespace BlackCore
{
    class CRuntime;

    /*!
     * Application context
     */
    class CContextApplication : public IContextApplication
    {
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME)
        Q_OBJECT
        friend class CRuntime;
        friend class IContextApplication;

    public slots:
        //! \copydoc IContextApplication::logMessage
        virtual void logMessage(const BlackMisc::CStatusMessage &message, const BlackMisc::Event::COriginator &origin) override;

        //! \copydoc IContextApplication::ping()
        virtual qint64 ping(qint64 token) const override;

        //! \copydoc IContextApplication::notifyAboutComponentChange
        virtual void notifyAboutComponentChange(uint component, uint action) override;

        //! \copydoc IContextApplication::writeToFile
        virtual bool writeToFile(const QString &fileName, const QString &content) override;

        //! \copydoc IContextApplication::readFromFile
        virtual QString readFromFile(const QString &fileName) override;

        //! \copydoc IContextApplication::removeFile
        virtual bool removeFile(const QString &fileName) override;

        //! \copydoc IContextApplication::existsFile
        virtual bool existsFile(const QString &fileName) override;

        //! \copydoc IContextApplication::processHotkeyFuncEvent
        virtual void processHotkeyFuncEvent(const BlackMisc::Event::CEventHotkeyFunction &event) override;

    protected:
        //! Constructor
        CContextApplication(CRuntimeConfig::ContextMode mode, CRuntime *runtime);

        //! Register myself in DBus, fail safe
        CContextApplication *registerWithDBus(CDBusServer *server)
        {
            if (!server || this->m_mode != CRuntimeConfig::LocalInDbusServer) { return this; }
            server->addObject(IContextApplication::ObjectPath(), this);
            return this;
        }
    };
}

#endif // guard
