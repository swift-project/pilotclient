/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIM_XSWIFTBUS_DBUSCONNECTION_H
#define BLACKSIM_XSWIFTBUS_DBUSCONNECTION_H

#include "dbusmessage.h"
#include "dbuserror.h"
#include "dbuscallbacks.h"

#include <event2/event.h>
#include <dbus/dbus.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace XSwiftBus
{

    class WatchHandler;
    class TimeoutHandler;
    class CDBusObject;

    //! DBus connection
    class CDBusConnection
    {
    public:
        //! Bus type
        enum BusType { SessionBus };

        //! Constructor
        CDBusConnection();

        //! Destructor
        ~CDBusConnection();

        // The ones below are not implemented yet.
        // If you need them, make sure that connection reference count is correct
        CDBusConnection(const CDBusConnection &) = delete;
        CDBusConnection &operator=(const CDBusConnection &) = delete;

        //! Connect to bus
        bool connect(BusType type, const std::string &service);

        //! Is connected?
        bool isConnected() const;

        //! Register DBus object with interfaceName and objectPath.
        //! \param object
        //! \param interfaceName
        //! \param objectPath
        //! \param dbusObjectPathVTable Virtual table handling DBus messages
        void registerObjectPath(CDBusObject *object, const std::string &interfaceName, const std::string &objectPath, const DBusObjectPathVTable &dbusObjectPathVTable);

        //! Send message to bus
        void sendMessage(const CDBusMessage &message);

        //! Close connection
        void close();

        //! Run DBus event loop (non-blocking)
        void runEventLoop();

        //! Run DBus event loop (blocking)
        void runBlockingEventLoop();

        //! Get the last error
        CDBusError lastError() const { return m_lastError; }

    private:
        friend class WatchHandler;
        friend class TimeoutHandler;
        friend class Timer;

        using WatchCallbacks = DBusAsyncCallbacks<DBusWatch>;
        using TimeoutCallbacks = DBusAsyncCallbacks<DBusTimeout>;

        bool setupMainloop();

        dbus_bool_t dbusAddWatch(DBusWatch *watch);
        void dbusRemoveWatch(DBusWatch *watch);
        void dbusWatchToggled(DBusWatch *watch);

        dbus_bool_t dbusAddTimeout(DBusTimeout *timeout);
        void dbusRemoveTimeout(DBusTimeout *timeout);
        void dbusTimeoutToggled(DBusTimeout *timeout);

        void scheduleDBusDispatch();
        void handleSocketReady(evutil_socket_t fd, short event);
        void dbusDispatch();
        void dbusUpdateDispatchStatus(DBusConnection *connection, DBusDispatchStatus newStatus);

        static void dbusUpdateDispatchStatus(DBusConnection *connection, DBusDispatchStatus newStatus, void *data);

        struct EventBaseDeleter
        {
            void operator()(event_base *obj) const { event_base_free(obj); }
        };

        struct DBusConnectionDeleter
        {
            void operator()(DBusConnection *obj) const { dbus_connection_unref(obj); }
        };

        std::unique_ptr<event_base, EventBaseDeleter> m_eventBase;
        std::unique_ptr<DBusConnection, DBusConnectionDeleter> m_connection;
        CDBusError m_lastError;
        WatchCallbacks m_watchCallbacks;
        TimeoutCallbacks m_timeoutCallbacks;

        std::unordered_multimap<evutil_socket_t, std::unique_ptr<WatchHandler>> m_watchers;
        std::vector<std::unique_ptr<TimeoutHandler>> m_timeouts;
    };

}

#endif // guard
