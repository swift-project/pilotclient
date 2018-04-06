/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbusmessage.h"

namespace XSwiftBus
{

    CDBusMessage::CDBusMessage(DBusMessage *message)
    {
        m_message = dbus_message_ref(message);
    }

    CDBusMessage::CDBusMessage(const CDBusMessage &other)
    {
        m_message = dbus_message_ref(other.m_message);
        m_serial = other.m_serial;
    }

    CDBusMessage::CDBusMessage(DBusMessage *message, dbus_uint32_t serial)
    {
        m_message = dbus_message_ref(message);
        m_serial = serial;
    }

    CDBusMessage::~CDBusMessage()
    {
        dbus_message_unref(m_message);
    }

    CDBusMessage &CDBusMessage::operator =(CDBusMessage other)
    {
        std::swap(m_serial, other.m_serial);
        m_message = dbus_message_ref(other.m_message);
        return *this;
    }

    bool CDBusMessage::isMethodCall() const
    {
        return dbus_message_get_type(m_message) == DBUS_MESSAGE_TYPE_METHOD_CALL;
    }

    bool CDBusMessage::wantsReply() const
    {
        return !dbus_message_get_no_reply(m_message);
    }

    std::string CDBusMessage::getSender() const
    {
        const char *sender = nullptr;
        sender = dbus_message_get_sender(m_message);
        return std::string(sender);
    }

    dbus_uint32_t CDBusMessage::getSerial() const
    {
        return dbus_message_get_serial(m_message);
    }

    std::string CDBusMessage::getInterfaceName() const
    {
        return dbus_message_get_interface(m_message);
    }

    std::string CDBusMessage::getObjectPath() const
    {
        return dbus_message_get_path(m_message);
    }

    std::string CDBusMessage::getMethodName() const
    {
        return dbus_message_get_member(m_message);
    }

    void CDBusMessage::beginArgumentWrite()
    {
        dbus_message_iter_init_append(m_message, &m_messageIterator);
    }

    void CDBusMessage::appendArgument(bool value)
    {
        dbus_bool_t boolean = value ? 1 : 0;
        dbus_message_iter_append_basic(&m_messageIterator, DBUS_TYPE_BOOLEAN, &boolean);
    }

    void CDBusMessage::appendArgument(const char *value)
    {
        dbus_message_iter_append_basic(&m_messageIterator, DBUS_TYPE_STRING, &value);
    }

    void CDBusMessage::appendArgument(const std::string &value)
    {
        const char *ptr = value.c_str();
        dbus_message_iter_append_basic(&m_messageIterator, DBUS_TYPE_STRING, &ptr);
    }

    void CDBusMessage::appendArgument(int value)
    {
        dbus_int32_t i = value;
        dbus_message_iter_append_basic(&m_messageIterator, DBUS_TYPE_INT32, &i);
    }

    void CDBusMessage::appendArgument(double value)
    {
        dbus_message_iter_append_basic(&m_messageIterator, DBUS_TYPE_DOUBLE, &value);
    }

    void CDBusMessage::appendArgument(const std::vector<double> &array)
    {
        DBusMessageIter arrayIterator;
        dbus_message_iter_open_container(&m_messageIterator, DBUS_TYPE_ARRAY, DBUS_TYPE_DOUBLE_AS_STRING, &arrayIterator);
        const double *ptr = array.data();
        dbus_message_iter_append_fixed_array(&arrayIterator, DBUS_TYPE_DOUBLE, &ptr, static_cast<int>(array.size()));
        dbus_message_iter_close_container(&m_messageIterator, &arrayIterator);
    }

    void CDBusMessage::appendArgument(const std::vector<std::string> &array)
    {
        DBusMessageIter arrayIterator;
        dbus_message_iter_open_container(&m_messageIterator, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING_AS_STRING, &arrayIterator);
        for (const auto &i : array)
        {
            const char *ptr = i.c_str();
            dbus_message_iter_append_basic(&arrayIterator, DBUS_TYPE_STRING, &ptr);
        }
        dbus_message_iter_close_container(&m_messageIterator, &arrayIterator);
    }

    void CDBusMessage::beginArgumentRead()
    {
        dbus_message_iter_init(m_message, &m_messageIterator);
    }

    void CDBusMessage::getArgument(int &value)
    {
        if (dbus_message_iter_get_arg_type(&m_messageIterator) != DBUS_TYPE_INT32) { return; }
        dbus_int32_t i;
        dbus_message_iter_get_basic(&m_messageIterator, &i);
        value = i;
        dbus_message_iter_next(&m_messageIterator);
    }

    void CDBusMessage::getArgument(bool &value)
    {
        if (dbus_message_iter_get_arg_type(&m_messageIterator) != DBUS_TYPE_BOOLEAN) { return; }
        dbus_bool_t v;
        dbus_message_iter_get_basic(&m_messageIterator, &v);
        value = v == TRUE ? true : false;
        dbus_message_iter_next(&m_messageIterator);
    }

    void CDBusMessage::getArgument(double &value)
    {
        if (dbus_message_iter_get_arg_type(&m_messageIterator) != DBUS_TYPE_DOUBLE) { return; }
        dbus_message_iter_get_basic(&m_messageIterator, &value);
        dbus_message_iter_next(&m_messageIterator);
    }

    void CDBusMessage::getArgument(std::string &value)
    {
        const char *str = nullptr;
        if (dbus_message_iter_get_arg_type(&m_messageIterator) != DBUS_TYPE_STRING) { return; }
        dbus_message_iter_get_basic(&m_messageIterator, &str);
        dbus_message_iter_next(&m_messageIterator);
        value = std::string(str);
    }

    CDBusMessage CDBusMessage::createSignal(const std::string &path, const std::string &interfaceName, const std::string &signalName)
    {
        DBusMessage *signal = dbus_message_new_signal(path.c_str(), interfaceName.c_str(), signalName.c_str());
        return CDBusMessage(signal);
    }

    CDBusMessage CDBusMessage::createReply(const std::string &destination, dbus_uint32_t serial)
    {
        DBusMessage *reply = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_RETURN);
        dbus_message_set_no_reply(reply, TRUE);
        dbus_message_set_destination(reply, destination.c_str());
        dbus_message_set_reply_serial(reply, serial);
        CDBusMessage msg(reply);
        dbus_message_unref(reply);
        return msg;
    }

}
