// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
        virtual ~CDBusServer() override;

        //! Set the dispatcher
        void setDispatcher(CDBusDispatcher *dispatcher);

        //! Connect to bus
        bool listen(const std::string &address);

        //! Is connected?
        bool isConnected() const;

        void dispatch() override {}

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

        CDBusDispatcher *m_dispatcher = nullptr;
        std::unique_ptr<DBusServer, DBusServerDeleter> m_server;
        CDBusError m_lastError;
        NewConnectionFunc m_newConnectionFunc;
    };
} // ns

#endif // guard
