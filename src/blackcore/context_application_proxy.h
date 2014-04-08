/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTAPPLICATION_PROXY_H
#define BLACKCORE_CONTEXTAPPLICATION_PROXY_H

#include "context_application_base.h"
#include "blackmisc/genericdbusinterface.h"

namespace BlackCore
{

    /*!
     * \brief Application context proxy
     */
    class CContextApplicationProxy : public CContextApplicationBase
    {
        Q_OBJECT
        friend class CRuntime;

    public:
        //! Destructor
        virtual ~CContextApplicationProxy() {}

    public slots:
        //! \copydoc IContextApplication::ping()
        virtual qint64 ping(qint64 token) const override;

        //! \brief Status message
        virtual void sendStatusMessage(const BlackMisc::CStatusMessage &message) override;

        //! Send status messages
        virtual void sendStatusMessages(const BlackMisc::CStatusMessageList &messages) override;

        //! \copydoc CContext::reEmitSignalFromProxy
        void signalFromProxy(const QString &signalName) override;

    protected:
        //! Constructor
        CContextApplicationProxy(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : CContextApplicationBase(mode, runtime), m_dBusInterface(nullptr) {}

        //! DBus version constructor
        CContextApplicationProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime);

    private:
        BlackMisc::CGenericDBusInterface *m_dBusInterface;

        //! Relay connection signals to local signals
        //! No idea why this has to be wired and is not done automatically
        void relaySignals(const QString &serviceName, QDBusConnection &connection);
    };
}

#endif // guard
