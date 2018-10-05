/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIM_XSWIFTBUS_DBUSOBJECT_H
#define BLACKSIM_XSWIFTBUS_DBUSOBJECT_H

#include "dbusconnection.h"
#include <XPLM/XPLMDisplay.h>
#include <mutex>
#include <deque>

namespace XSwiftBus
{
    //! DBus base object
    class CDBusObject
    {
    public:
        //! Constructor
        CDBusObject();

        //! Destructor
        virtual ~CDBusObject();

        //! Set the assigned DBus connection.
        //! \remark Currently one object can only manage one connection at a time
        void setDBusConnection(const std::shared_ptr<CDBusConnection> &dbusConnection);

        //! Register itself with interfaceName and objectPath
        //! \warning Before calling this method, make sure that a valid DBus connection was set.
        void registerDBusObjectPath(const std::string &interfaceName, const std::string &objectPath);

    protected:
        //! Handler which is called when DBusCconnection is established
        virtual void dbusConnectedHandler() {}

        //! DBus message handler
        virtual DBusHandlerResult dbusMessageHandler(const CDBusMessage &message) = 0;

        //! Handler which is called when DBusConnection disconnected
        virtual void dbusDisconnectedHandler() {}

        //! Send DBus signal
        void sendDBusSignal(const std::string &name);

        //! Send DBus message
        void sendDBusMessage(const CDBusMessage &message);

        //! Maybe sends an empty DBus reply (acknowledgement)
        void maybeSendEmptyDBusReply(bool wantsReply, const std::string &destination, dbus_uint32_t serial);

        //! Send DBus reply
        template <typename T>
        void sendDBusReply(const std::string &destination, dbus_uint32_t serial, const T &argument)
        {
            CDBusMessage reply = CDBusMessage::createReply(destination, serial);
            reply.beginArgumentWrite();
            reply.appendArgument(argument);
            m_dbusConnection->sendMessage(reply);
        }

        //! Send DBus reply
        template <typename T>
        void sendDBusReply(const std::string &destination, dbus_uint32_t serial, const std::vector<T> &array)
        {
            CDBusMessage reply = CDBusMessage::createReply(destination, serial);
            reply.beginArgumentWrite();
            reply.appendArgument(array);
            m_dbusConnection->sendMessage(reply);
        }

        //! Queue a DBus call to be executed in a different thread
        void queueDBusCall(const std::function<void()> &func);

        //! Invoke all pending DBus calls. They will be executed in the calling thread.
        void invokeQueuedDBusCalls();

    private:
        static void dbusObjectPathUnregisterFunction(DBusConnection *connection, void *data);
        static DBusHandlerResult dbusObjectPathMessageFunction(DBusConnection *connection, DBusMessage *message, void *data);

        std::shared_ptr<CDBusConnection> m_dbusConnection;
        std::string m_interfaceName;
        std::string m_objectPath;

        std::mutex m_mutex;
        std::deque<std::function<void()>> m_qeuedDBusCalls;

        const DBusObjectPathVTable m_dbusObjectPathVTable = { dbusObjectPathUnregisterFunction, dbusObjectPathMessageFunction, nullptr, nullptr, nullptr, nullptr };
    };

}

#endif // guard
