/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTAPPLICATION_H
#define BLACKCORE_CONTEXTAPPLICATION_H

#include "blackcore/dbus_server.h"
#include "blackcore/context_application_interface.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackcore/coreruntime.h"
#include <QObject>

#define BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME "blackcore.contextapplication"

namespace BlackCore
{
    class CCoreRuntime;

    /*!
     * \brief Application context
     */
    class CContextApplication : public IContextApplication
    {
        // Register by same name, make signals sender independent
        // http://dbus.freedesktop.org/doc/dbus-faq.html#idp48032144
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME)
        Q_OBJECT

    public:

        /*!
         * Context
         * \param parent
         */
        CContextApplication(CCoreRuntime *parent);

        /*!
         * Destructor
         */
        virtual ~CContextApplication() {}

        /*!
         * \brief Register myself in DBus
         * \param server
         */
        void registerWithDBus(CDBusServer *server)
        {
            server->addObject(IContextApplication::ServicePath(), this);
        }

        /*!
         * \brief Runtime
         * \return
         */
        const CCoreRuntime *getRuntime() const
        {
            return static_cast<CCoreRuntime *>(this->parent());
        }

    public slots:

        /*!
         * \brief Ping
         * \param token
         * \return
         */
        qint64 ping(qint64 token) const;

    };
}

#endif // guard
