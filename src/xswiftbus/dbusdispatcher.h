// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIM_XSWIFTBUS_DBUSDISPATCHER_H
#define SWIFT_SIM_XSWIFTBUS_DBUSDISPATCHER_H

#include <dbus/dbus.h>
#include <event2/event.h>

#include <memory>
#include <unordered_map>
#include <vector>

#include "dbuscallbacks.h"

namespace XSwiftBus
{

    class WatchHandler;
    class TimeoutHandler;
    class CDBusConnection;
    class CDBusDispatcher;

    //! Dispatchable Interface
    class IDispatchable
    {
    public:
        //! Default constructor
        IDispatchable() = default;

        //! Default destructor
        virtual ~IDispatchable() = default;

        //! Dispatch execution method
        virtual void dispatch() = 0;

    private:
        friend CDBusDispatcher;
    };

    //! DBus Dispatcher
    class CDBusDispatcher
    {
    public:
        //! Constructor
        CDBusDispatcher();

        //! Destructor
        virtual ~CDBusDispatcher();

        //! Add dispatchable object
        void add(IDispatchable *dispatchable);

        //! Remove dispatchable object
        void remove(IDispatchable *dispatchable);

        //! Waits for events to be dispatched and handles them
        void waitAndRun();

        //! Dispatches ready handlers and returns without waiting
        void runOnce();

    private:
        friend class WatchHandler;
        friend class TimeoutHandler;
        friend class Timer;
        friend class CDBusConnection;
        friend class CDBusServer;

        struct EventBaseDeleter
        {
            void operator()(event_base *obj) const { event_base_free(obj); }
        };

        using WatchCallbacks = DBusAsyncCallbacks<DBusWatch>;
        using TimeoutCallbacks = DBusAsyncCallbacks<DBusTimeout>;

        void dispatch();

        dbus_bool_t dbusAddWatch(DBusWatch *watch);
        void dbusRemoveWatch(const DBusWatch *watch);
        void dbusWatchToggled(DBusWatch *watch);

        dbus_bool_t dbusAddTimeout(DBusTimeout *timeout);
        void dbusRemoveTimeout(DBusTimeout *timeout);
        void dbusTimeoutToggled(DBusTimeout *timeout);

        WatchCallbacks m_watchCallbacks;
        TimeoutCallbacks m_timeoutCallbacks;
        std::unordered_multimap<evutil_socket_t, std::unique_ptr<WatchHandler>> m_watchers;
        std::vector<std::unique_ptr<TimeoutHandler>> m_timeouts;
        std::unique_ptr<event_base, EventBaseDeleter> m_eventBase;

        std::vector<IDispatchable *> m_dispatchList;
    };
} // namespace XSwiftBus

#endif // SWIFT_SIM_XSWIFTBUS_DBUSDISPATCHER_H
