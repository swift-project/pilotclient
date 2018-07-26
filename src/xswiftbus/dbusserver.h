/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIM_XSWIFTBUS_DBUSSERVER_H
#define BLACKSIM_XSWIFTBUS_DBUSSERVER_H

#include "dbusmessage.h"
#include "dbuserror.h"
#include "dbuscallbacks.h"
#include "dbusdispatcher.h"

#include <event2/event.h>
#include <dbus/dbus.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

namespace XSwiftBus
{

    class CDBusObject;

    //! DBus connection
    class CDBusServer : public IDispatchable
    {
    public:
        //! New connection handler function
        using NewConnectionFunc = std::function<void(std::shared_ptr<CDBusConnection>)>;

        //! Constructor
        CDBusServer();

        //! Destructor
        ~CDBusServer();

        //! Set the dispatcher
        void setDispatcher(CDBusDispatcher *dispatcher);

        //! Connect to bus
        bool listen(const std::string &address);

        //! Is connected?
        bool isConnected() const;

        void dispatch() {}

        //! Close connection
        void close();

        //! Get the last error
        CDBusError lastError() const { return m_lastError; }

        //! Set the function to be used for handling new connections.
        void setNewConnectionFunc(const NewConnectionFunc &func)
        {
            m_newConnectionFunc = func;
        }

    private:
        void onNewConnection(DBusServer *server, DBusConnection *conn);
        static void onNewConnection(DBusServer *server, DBusConnection *conn, void *data);

        struct DBusServerDeleter
        {
            void operator()(DBusServer *obj) const { dbus_server_unref(obj); }
        };

        std::unique_ptr<DBusServer, DBusServerDeleter> m_server;
        CDBusError m_lastError;
        CDBusDispatcher *m_dispatcher;
        NewConnectionFunc m_newConnectionFunc;
    };

}

#endif // guard
