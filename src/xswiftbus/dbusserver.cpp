
/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "dbusserver.h"
#include "dbusobject.h"
#include "utils.h"

#include <utility>
#include <algorithm>
#include <cassert>
#include <memory>

namespace XSwiftBus
{
    CDBusServer::CDBusServer()
    {
        dbus_threads_init_default();
    }

    CDBusServer::~CDBusServer()
    {
        close();
    }

    bool CDBusServer::listen(const std::string &address)
    {
        DBusError error;
        dbus_error_init(&error);
        m_server.reset(dbus_server_listen(address.c_str(), &error));

        if (!m_server)
        {
            ERROR_LOG("DBus failed to listen for new connections on given address: " + std::string(error.message));
            return false;
        }
        dbus_server_set_new_connection_function(m_server.get(), onNewConnection, this, nullptr);
        return true;
    }

    bool CDBusServer::isConnected() const
    {
        return m_server ? dbus_server_get_is_connected(m_server.get()) : false;
    }

    void CDBusServer::close()
    {
        if (m_server) { dbus_server_disconnect(m_server.get()); }
    }

    void CDBusServer::setDispatcher(CDBusDispatcher *dispatcher)
    {
        assert(dispatcher);
        assert(m_server);

        m_dispatcher = dispatcher;

        dbus_server_set_watch_functions(
            m_server.get(),
            dispatcher->m_watchCallbacks.add,
            dispatcher->m_watchCallbacks.remove,
            dispatcher->m_watchCallbacks.toggled,
            &dispatcher->m_watchCallbacks, nullptr);

        dbus_server_set_timeout_functions(
            m_server.get(),
            dispatcher->m_timeoutCallbacks.add,
            dispatcher->m_timeoutCallbacks.remove,
            dispatcher->m_timeoutCallbacks.toggled,
            &dispatcher->m_timeoutCallbacks, nullptr);
    }

    void CDBusServer::onNewConnection(DBusServer *, DBusConnection *conn)
    {
        // called by listener and the DBus connection for xSwiftBus afterwards
        INFO_LOG("onNewConnection");
        auto dbusConnection = std::make_shared<CDBusConnection>(conn);
        m_newConnectionFunc(dbusConnection);
    }

    void CDBusServer::onNewConnection(DBusServer *server, DBusConnection *conn, void *data)
    {
        auto *obj = static_cast<CDBusServer *>(data);
        obj->onNewConnection(server, conn);
    }
}
