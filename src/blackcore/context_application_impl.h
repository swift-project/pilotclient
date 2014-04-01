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
     * \brief Application context
     */
    class CContextApplication : public IContextApplication
    {
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME)
        Q_OBJECT

    public:

        //! Constructor
        CContextApplication(CRuntimeConfig::ContextMode mode, CRuntime *runtime = nullptr);

        //! Destructor
        virtual ~CContextApplication() {}

        //! Register myself in DBus
        CContextApplication *registerWithDBus(CDBusServer *server)
        {
            server->addObject(IContextApplication::ObjectPath(), this);
            return this;
        }

    public slots:

        //! \copydoc IContextApplication::ping()
        virtual qint64 ping(qint64 token) const override;

    };
}

#endif // guard
