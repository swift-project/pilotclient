/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKSIM_XSWIFTBUS_DBUSCONNECTION_H
#define BLACKSIM_XSWIFTBUS_DBUSCONNECTION_H

#include "dbusmessage.h"
#include "dbuserror.h"
#include "dbuscallbacks.h"
#include "dbusdispatcher.h"

#include <event2/event.h>
#include <dbus/dbus.h>
#include <string>
#include <unordered_map>
#include <memory>

namespace XSwiftBus
{

    class CDBusObject;

    //! DBus connection
    class CDBusConnection : public IDispatchable
    {
    public:
        //! Bus type
        enum BusType
        {
            SessionBus
        };

        //! Disconnect Callback
        using DisconnectedCallback = std::function<void()>;

        //! Default constructor
        CDBusConnection();

        //! Constructor
        CDBusConnection(DBusConnection *connection);

        //! Destructor
        ~CDBusConnection() override;

        // The ones below are not implemented yet.
        // If you need them, make sure that connection reference count is correct
        CDBusConnection(const CDBusConnection &) = delete;
        CDBusConnection &operator=(const CDBusConnection &) = delete;

        //! Connect to bus
        bool connect(BusType type);

        //! Set dispatcher
        void setDispatcher(CDBusDispatcher *dispatcher);

        //! Request name to the bus
        void requestName(const std::string &name);

        //! Is connected?
        bool isConnected() const;

        //! Register a disconnected callback
        void registerDisconnectedCallback(CDBusObject *obj, DisconnectedCallback func);

        //! Register a disconnected callback
        void unregisterDisconnectedCallback(CDBusObject *obj);

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

        //! Get the last error
        CDBusError lastError() const { return m_lastError; }

    protected:
        // cppcheck-suppress virtualCallInConstructor
        virtual void dispatch() override final;

    private:
        void setDispatchStatus(DBusConnection *connection, DBusDispatchStatus status);
        static void setDispatchStatus(DBusConnection *connection, DBusDispatchStatus status, void *data);
        static DBusHandlerResult filterDisconnectedFunction(DBusConnection *connection, DBusMessage *message, void *data);

        struct DBusConnectionDeleter
        {
            void operator()(DBusConnection *obj) const { dbus_connection_unref(obj); }
        };

        CDBusDispatcher *m_dispatcher = nullptr;
        std::unique_ptr<DBusConnection, DBusConnectionDeleter> m_connection;
        CDBusError m_lastError;
        std::unordered_map<CDBusObject *, DisconnectedCallback> m_disconnectedCallbacks;
    };

}

#endif // guard
