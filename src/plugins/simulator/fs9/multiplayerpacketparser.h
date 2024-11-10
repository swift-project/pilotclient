// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIMPLUGIN_FS9_MP_PACKET_PARSER_H
#define SWIFT_SIMPLUGIN_FS9_MP_PACKET_PARSER_H

#include "fs9.h"
#include <QtGlobal>
#include <QDebug>
#include <QByteArray>
#include <QString>
#include <tuple>

//! \file

namespace swift::simplugin::fs9
{
    namespace Private
    {
        // Inhibit doxygen warnings about missing documentation
        //! \cond PRIVATE

        // overload the following two for types that need special treatment
        template <typename Value>
        QByteArray readValue(const QByteArray &data, Value &val)
        {
            static_assert(std::is_arithmetic<Value>::value ||
                              std::is_same<CFs9Sdk::EngineType, Value>::value,
                          "Unsupported type");

            val = *reinterpret_cast<const Value *>(data.constData());
            return data.mid(sizeof(Value));
        }

        template <typename Value>
        QByteArray writeValue(QByteArray data, const Value &value)
        {
            static_assert(std::is_arithmetic<Value>::value ||
                              std::is_same<CFs9Sdk::EngineType, Value>::value,
                          "Unsupported type");

            data.append(reinterpret_cast<const char *>(&value), sizeof(Value));
            return data;
        }

        QByteArray readValue(const QByteArray &data, QString &val);

        QByteArray writeValue(QByteArray data, const QString &value);

        template <typename MessageTuple, quint32 Size = std::tuple_size<MessageTuple>::value>
        struct Serializer;

        template <typename MsgTuple>
        struct Serializer<MsgTuple, 0>
        {
            static const QByteArray &read(const QByteArray &data, MsgTuple &) { return data; }
            static QByteArray write(const QByteArray &data, const MsgTuple &) { return data; }
        };

        template <typename MessageTuple, quint32 Size>
        struct Serializer
        {
            static QByteArray read(const QByteArray &data, MessageTuple &messageTuple)
            {
                return readValue(Serializer<MessageTuple, Size - 1>::read(data, messageTuple), std::get<Size - 1>(messageTuple));
            }
            static QByteArray write(QByteArray &data, const MessageTuple &messageTuple)
            {
                return writeValue(Serializer<MessageTuple, Size - 1>::write(data, messageTuple), std::get<Size - 1>(messageTuple));
            }
        };

        template <class MessageTuple>
        inline QByteArray doReadMessage(const QByteArray &data, MessageTuple messageTuple)
        {
            return Serializer<MessageTuple>::read(data, messageTuple);
        }

        template <class MessageTuple>
        inline QByteArray doWriteMessage(QByteArray &data, const MessageTuple &messageTuple)
        {
            return Serializer<MessageTuple>::write(data, messageTuple);
        }

        //! \endcond
    } // namespace Private

    //! FS9 multiplayer protocol packet parser
    class MultiPlayerPacketParser
    {
    public:
        //! Read message from byte stream
        template <class Message>
        static QByteArray readMessage(const QByteArray &data, Message &message)
        {
            return Private::doReadMessage(data.mid(2 * sizeof(qint32)), message.getTuple());
        }

        //! Write message to byte stream
        template <class Message>
        static QByteArray writeMessage(QByteArray &data, const Message &message)
        {
            return Private::doWriteMessage(data, message.getTuple());
        }

        //! Read the multiplayer packet type
        static CFs9Sdk::MULTIPLAYER_PACKET_ID readType(const QByteArray &data)
        {
            QByteArray type = data.left(sizeof(CFs9Sdk::MULTIPLAYER_PACKET_ID));
            const quint32 *val = reinterpret_cast<const quint32 *>(type.constData());
            return static_cast<CFs9Sdk::MULTIPLAYER_PACKET_ID>(*val);
        }

        //! Read the multiplayer packet size
        static qint32 readSize(const QByteArray &data)
        {
            return *reinterpret_cast<const qint32 *>(data.mid(sizeof(qint32), sizeof(qint32)).constData());
        }

        //! Write the multiplayer packet type
        static void writeType(QByteArray &data, CFs9Sdk::MULTIPLAYER_PACKET_ID type)
        {
            data.append(reinterpret_cast<const char *>(&type), sizeof(CFs9Sdk::MULTIPLAYER_PACKET_ID));
        }

        //! Write the multiplayer packet size
        static void writeSize(QByteArray &data, qint32 size)
        {
            data.append(reinterpret_cast<const char *>(&size), sizeof(qint32));
        }
    };
}

#endif // SWIFT_SIMPLUGIN_FS9_MP_PACKET_PARSER_H
