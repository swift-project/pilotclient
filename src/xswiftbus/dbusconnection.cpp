#include <utility>

/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbusconnection.h"
#include "dbusobject.h"

#include <algorithm>
#include <cassert>
#include <memory>

namespace XSwiftBus
{

    //! Functor struct deleteing an event
    struct EventDeleter
    {
        //! Delete functor
        void operator()(event *obj) const
        {
            event_del(obj);
            event_free(obj);
        }
    };

    //! DBus watch handler
    class WatchHandler
    {
    public:
        //! Constructor
        WatchHandler(CDBusConnection *parent, DBusWatch *watch)
            : m_parent(parent), m_watch(watch)
        {
            const unsigned int flags = dbus_watch_get_flags(watch);
            short monitoredEvents = EV_PERSIST;

            if (flags & DBUS_WATCH_READABLE) { monitoredEvents |= EV_READ; }
            if (flags & DBUS_WATCH_WRITABLE) { monitoredEvents |= EV_WRITE; }

            const int fd = dbus_watch_get_unix_fd(watch);
            m_event.reset(event_new(parent->m_eventBase.get(), fd, monitoredEvents, callback, this));
            event_add(m_event.get(), nullptr);
        }

        //! Get DBus watch
        DBusWatch *getWatch() { return m_watch; }

        //! Get DBus watch
        const DBusWatch *getWatch() const { return m_watch; }

    private:
        //! Event callback
        static void callback(evutil_socket_t fd, short event, void *data)
        {
            auto *watchHandler = static_cast<WatchHandler *>(data);
            watchHandler->m_parent->handleSocketReady(fd, event);
        }

        CDBusConnection *m_parent = nullptr;
        std::unique_ptr<event, EventDeleter> m_event;
        DBusWatch *m_watch = nullptr;
    };

    //! DBus timeout handler
    class TimeoutHandler
    {
    public:
        //! Constructor
        TimeoutHandler(CDBusConnection *parent, DBusTimeout *timeout)
            : m_timeout(timeout)
        {
            timeval timer;
            const int interval = dbus_timeout_get_interval(timeout);
            timer.tv_sec = interval / 1000;
            timer.tv_usec = (interval % 1000) * 1000;

            m_event.reset(evtimer_new(parent->m_eventBase.get(), callback, this));
            evtimer_add(m_event.get(), &timer);
        }

        //! Get DBus timeout
        const DBusTimeout *getTimeout() const { return m_timeout; }

    private:
        //! Event callback
        static void callback(evutil_socket_t fd, short event, void *data)
        {
            (void) fd; // unused
            (void) event; // unused
            auto *timeoutHandler = static_cast<TimeoutHandler *>(data);
            dbus_timeout_handle(timeoutHandler->m_timeout);
        }

        std::unique_ptr<event, EventDeleter> m_event;
        DBusTimeout *m_timeout = nullptr;
    };

    //! Generic Timer
    class Timer
    {
    public:
        Timer() = default;
        //! Constructor
        Timer(CDBusConnection *parent, const timeval &timeout, const std::function<void()> &func)
            : m_func(func)
        {
            m_event.reset(evtimer_new(parent->m_eventBase.get(), callback, this));
            evtimer_add(m_event.get(), &timeout);
        }

    private:
        //! Event callback
        static void callback(evutil_socket_t fd, short event, void *data)
        {
            (void) fd; // unused
            (void) event; // unused
            auto *timer = static_cast<Timer *>(data);
            timer->m_func();
            delete timer;
        }

        std::unique_ptr<event, EventDeleter> m_event;
        std::function<void()> m_func;
    };

    CDBusConnection::CDBusConnection()
        : m_eventBase(event_base_new())
    {
        dbus_threads_init_default();
        using namespace std::placeholders;
        m_watchCallbacks = WatchCallbacks(std::bind(&CDBusConnection::dbusAddWatch, this, _1),
                                          std::bind(&CDBusConnection::dbusRemoveWatch, this, _1),
                                          std::bind(&CDBusConnection::dbusWatchToggled, this, _1));

        m_timeoutCallbacks = TimeoutCallbacks(std::bind(&CDBusConnection::dbusAddTimeout, this, _1),
                                              std::bind(&CDBusConnection::dbusRemoveTimeout, this, _1),
                                              std::bind(&CDBusConnection::dbusTimeoutToggled, this, _1));
    }

    CDBusConnection::~CDBusConnection()
    {
        close();
    }

    bool CDBusConnection::connect(BusType type, const std::string &service)
    {
        assert(type == SessionBus);
        DBusError error;
        dbus_error_init(&error);

        DBusBusType dbusBusType;
        switch (type)
        {
        case SessionBus: dbusBusType = DBUS_BUS_SESSION; break;
        }

        m_connection.reset(dbus_bus_get_private(dbusBusType, &error));
        if (dbus_error_is_set(&error))
        {
            m_lastError = CDBusError(&error);
            return false;
        }

        // Don't exit application, if the connection is disconnected
        dbus_connection_set_exit_on_disconnect(m_connection.get(), false);

        if (!setupMainloop())
        {
            m_connection.release();
            return false;
        }

        dbus_bus_request_name(m_connection.get(), service.c_str(), 0, &error);
        if (dbus_error_is_set(&error))
        {
            m_lastError = CDBusError(&error);
            return false;
        }

        return true;
    }

    bool CDBusConnection::isConnected() const
    {
        return static_cast<bool>(m_connection);
    }

    void CDBusConnection::registerObjectPath(CDBusObject *object, const std::string &interfaceName, const std::string &objectPath, const DBusObjectPathVTable &dbusObjectPathVTable)
    {
        (void) interfaceName;
        if (!m_connection) { return; }

        dbus_connection_try_register_object_path(m_connection.get(), objectPath.c_str(), &dbusObjectPathVTable, object, nullptr);
    }

    void CDBusConnection::sendMessage(const CDBusMessage &message)
    {
        if (!isConnected()) { return; }
        dbus_uint32_t serial = message.getSerial();
        dbus_connection_send(m_connection.get(), message.m_message, &serial);
    }

    void CDBusConnection::close()
    {
        if (m_connection) { dbus_connection_close(m_connection.get()); }
    }

    void CDBusConnection::runEventLoop()
    {
        if (!m_eventBase || !isConnected()) { return; }
        event_base_loop(m_eventBase.get(), EVLOOP_NONBLOCK);
    }

    void CDBusConnection::runBlockingEventLoop()
    {
        if (!m_eventBase || !isConnected()) { return; }
        event_base_dispatch(m_eventBase.get());
    }

    bool CDBusConnection::setupMainloop()
    {
        DBusDispatchStatus status;

        if (dbus_connection_set_watch_functions(
                    m_connection.get(),
                    m_watchCallbacks.add,
                    m_watchCallbacks.remove,
                    m_watchCallbacks.toggled,
                    &m_watchCallbacks, nullptr) == FALSE)
        {
            return false;
        }


        if (dbus_connection_set_timeout_functions(
                    m_connection.get(),
                    m_timeoutCallbacks.add,
                    m_timeoutCallbacks.remove,
                    m_timeoutCallbacks.toggled,
                    &m_timeoutCallbacks, nullptr) == FALSE)
        {
            return false;
        }

        dbus_connection_set_dispatch_status_function(
            m_connection.get(),
            dbusUpdateDispatchStatus,
            this, nullptr);

        status = dbus_connection_get_dispatch_status(m_connection.get());
        if (status == DBUS_DISPATCH_DATA_REMAINS) { scheduleDBusDispatch(); }

        return true;
    }

    dbus_bool_t CDBusConnection::dbusAddWatch(DBusWatch *watch)
    {
        if (dbus_watch_get_enabled(watch) == FALSE) { return true; }

        int fd = dbus_watch_get_unix_fd(watch);
        m_watchers.emplace(fd, std::make_unique<WatchHandler>(this, watch));
        return true;
    }

    void CDBusConnection::dbusRemoveWatch(DBusWatch *watch)
    {
        for (auto it = m_watchers.begin(); it != m_watchers.end();)
        {
            if (it->second->getWatch() == watch) { it = m_watchers.erase(it); }
            else { ++it; }
        }
    }

    void CDBusConnection::dbusWatchToggled(DBusWatch *watch)
    {
        if (dbus_watch_get_enabled(watch) == TRUE) { dbusAddWatch(watch); }
        else { dbusRemoveWatch(watch); }
    }

    dbus_bool_t CDBusConnection::dbusAddTimeout(DBusTimeout *timeout)
    {
        if (dbus_timeout_get_enabled(timeout) == FALSE) { return TRUE; }
        m_timeouts.emplace(m_timeouts.end(), std::make_unique<TimeoutHandler>(this, timeout));
        return true;
    }

    void CDBusConnection::dbusRemoveTimeout(DBusTimeout *timeout)
    {
        auto predicate = [timeout](const std::unique_ptr<TimeoutHandler> &ptr)
        {
            return ptr->getTimeout() == timeout;
        };

        m_timeouts.erase(std::remove_if(m_timeouts.begin(), m_timeouts.end(), predicate), m_timeouts.end());
    }

    void CDBusConnection::dbusTimeoutToggled(DBusTimeout *timeout)
    {
        if (dbus_timeout_get_enabled(timeout) == TRUE)
            dbusAddTimeout(timeout);
        else
            dbusRemoveTimeout(timeout);
    }

    void CDBusConnection::scheduleDBusDispatch()
    {
        const timeval timeout = {0, 0};
        // This is no memory leak. The allocated timer will be deleted in its own callback
        new Timer(this, timeout, [this]() { dbusDispatch(); });
    }

    void CDBusConnection::handleSocketReady(evutil_socket_t fd, short event)
    {
        DBusDispatchStatus status;
        unsigned int flags = 0;

        auto watcher = m_watchers.find(fd);
        if (watcher == m_watchers.end()) { return; }

        dbus_connection_ref(m_connection.get());

        if (evutil_socket_geterror(fd) != 0) { flags |= DBUS_WATCH_ERROR; }
        if (event & EV_READ) { flags |= DBUS_WATCH_READABLE; }
        if (event & EV_WRITE) { flags |= DBUS_WATCH_WRITABLE; }

        dbus_watch_handle(watcher->second->getWatch(), flags);

        status = dbus_connection_get_dispatch_status(m_connection.get());
        if (status == DBUS_DISPATCH_DATA_REMAINS) { dbusDispatch(); }

        dbus_connection_unref(m_connection.get());
    }

    void CDBusConnection::dbusDispatch()
    {
        dbus_connection_ref(m_connection.get());
        while (dbus_connection_dispatch(m_connection.get()) == DBUS_DISPATCH_DATA_REMAINS);
        dbus_connection_unref(m_connection.get());
    }

    void CDBusConnection::dbusUpdateDispatchStatus(DBusConnection *connection, DBusDispatchStatus newStatus)
    {
        (void)newStatus; // unused

        DBusDispatchStatus status;

        if (dbus_connection_get_is_connected(connection) == FALSE) { return; }

        status = dbus_connection_get_dispatch_status(connection);
        if (status == DBUS_DISPATCH_DATA_REMAINS) { scheduleDBusDispatch(); }
    }

    void CDBusConnection::dbusUpdateDispatchStatus(DBusConnection *connection, DBusDispatchStatus newStatus, void *data)
    {
        auto *obj = static_cast<CDBusConnection *>(data);
        return obj->dbusUpdateDispatchStatus(connection, newStatus);
    }

}
