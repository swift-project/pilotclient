/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTAPPLICATION_PROXY_H
#define BLACKCORE_CONTEXTAPPLICATION_PROXY_H

#include "blackcoreexport.h"
#include "context_application.h"
#include "blackmisc/genericdbusinterface.h"

namespace BlackCore
{

    //! \brief Application context proxy
    //! \ingroup dbus
    class BLACKCORE_EXPORT CContextApplicationProxy : public IContextApplication
    {
        Q_OBJECT
        friend class IContextApplication;

    public:
        //! Destructor
        virtual ~CContextApplicationProxy() {}

    public slots:
        //! \copydoc IContextApplication::logMessage
        virtual void logMessage(const BlackMisc::CStatusMessage &message, const BlackMisc::COriginator &origin) override;

        //! \copydoc IContextApplication::registerApplication
        virtual BlackMisc::COriginator registerApplication(const BlackMisc::COriginator &application) override;

        //! \copydoc IContextApplication::unRegisterApplication
        virtual void unregisterApplication(const BlackMisc::COriginator &application) override;

        //! \copydoc IContextApplication::getRegisteredApplications
        virtual BlackMisc::COriginatorList getRegisteredApplications() const override;

        //! \copydoc IContextApplication::writeToFile
        virtual bool writeToFile(const QString &fileName, const QString &content) override;

        //! \copydoc IContextApplication::readFromFile
        virtual QString readFromFile(const QString &fileName) const override;

        //! \copydoc IContextApplication::removeFile
        virtual bool removeFile(const QString &fileName) override;

        //! \copydoc IContextApplication::existsFile
        virtual bool existsFile(const QString &fileName) const override;

        //! \copydoc IContextApplication::processHotkeyFuncEvent
        virtual void processHotkeyFuncEvent(const BlackMisc::Event::CEventHotkeyFunction &event) override;

    protected:
        //! Constructor
        CContextApplicationProxy(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextApplication(mode, runtime), m_dBusInterface(nullptr) {}

        //! DBus version constructor
        CContextApplicationProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime);

    private:
        BlackMisc::CGenericDBusInterface *m_dBusInterface;

        //! Relay connection signals to local signals
        void relaySignals(const QString &serviceName, QDBusConnection &connection);
    };
}

#endif // guard
