//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include <QTcpSocket>
#include <QDataStream>

#include "blackmisc/debug.h"

#include "blackmisc/com_handler.h"

namespace BlackMisc
{

    IComHandler::IComHandler(QObject *parent) :
    QObject(parent)
    {
    }

    void IComHandler::createFrame(const QString& messageID, const QByteArray &data)
    {
        m_sender_buffer.clear();

        QDataStream stream(&m_sender_buffer, QIODevice::WriteOnly);

        qint32 message_size = data.size();
        quint16 crc = qChecksum(data.constData(), message_size);

        stream << (qint32)Sync_Marker
               << (qint32)message_size
               << (QString)messageID;
        stream.writeRawData(data.constData(), message_size);
        stream << (quint16)crc;
    }

    bool IComHandler::parseFrame(QString &messageID, QByteArray &data)
    {
		if (m_receive_buffer.isEmpty())
			return false;

        qint32 total_length = 0;
        qint32 min_size = 3 * (int)sizeof(qint32);

        if (m_receive_buffer.size() < min_size)
        {
            bDebug << "Received data is to small to read SYNC and frame length!";
            return false;
        }

        bool found_sync = false;
        QDataStream stream (m_receive_buffer);

        /////////////////////////////////////////////////
        // Read the Sync
        /////////////////////////////////////////////////

        do
        {
            qint32 sync = 0;
            stream >> (qint32)sync;
            if (sync == Sync_Marker)
                found_sync = true;
        }
        while (!found_sync && !stream.atEnd());

        if (!found_sync)
        {
            bWarning << "Could not find sync pattern in the stream. Discarding all data!";
            m_receive_buffer.clear();
            return false;
        }

        total_length += (int)sizeof(qint32);    // Sync

        /////////////////////////////////////////////////
        // Read size of the message itself
        /////////////////////////////////////////////////

        qint32 message_length = 0;
        stream >> (qint32)message_length;

        total_length += (int)sizeof(qint32);    // Length
        total_length += message_length;         // Data

        /////////////////////////////////////////////////
        // Read the message id
        /////////////////////////////////////////////////

        stream >> messageID;
        total_length += sizeof(QChar) * messageID.length() + (int)sizeof(quint32);    // ID

        if (m_receive_buffer.size() < (total_length + (int)sizeof (quint16)))
        {
            bDebug << "Received data is to small to read data block!";
            return false;
        }

        /////////////////////////////////////////////////
        // Read the data
        /////////////////////////////////////////////////

        data.resize(message_length);
        qint32 bytes = stream.readRawData(data.data(), message_length);

        bAssert (bytes == message_length);
        bAssert (data.size() == message_length);

        quint16 crc_calc = qChecksum (data.constData(), data.size());
        quint16 crc_recv = 0;
        stream >> (quint16)crc_recv;

        total_length += (int)sizeof(quint16);

        m_receive_buffer.remove(0, total_length);

        if (crc_calc != crc_recv)
        {
            bWarning << "Message CRC error!";
            data.clear();
            return false;
        }

        return true;
    }

} // namespace BlackMisc
