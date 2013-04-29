//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "blackmisc/com_client_buffer.h"
#include "blackmisc/debug.h"
#include "blackmisc/context.h"
#include <QTcpSocket>

namespace BlackMisc
{

CComClientBuffer::CComClientBuffer(IContext &context, uint clientID, QTcpSocket *socket, QObject *parent)
    : IComHandler(context, parent), m_context(context), m_tcp_socket(socket), m_client_id(clientID)
{
    connect(m_tcp_socket, SIGNAL(readyRead()), this, SLOT(onReceivingData()));
    connect(m_tcp_socket, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));
}
CComClientBuffer::~CComClientBuffer()
{
    m_tcp_socket->deleteLater();
}

bool CComClientBuffer::sendMessage(const QString &id, const QByteArray &message)
{
    createFrame(id, message);

    qint64 sender_buffer_size = m_sender_buffer.size();

    qint64 bytes = m_tcp_socket->write(m_sender_buffer);
    if (bytes < 0 || bytes != sender_buffer_size)
    {
        bWarning(m_context) << "Error writing to socket!";
        return false;
    }

    return true;
}

void CComClientBuffer::onReceivingData()
{
    QByteArray message;
    QString messageID;

    m_receive_buffer.append(m_tcp_socket->readAll());
    while (parseFrame(messageID, message))
    {
        emit doReceivedMessage(m_client_id, messageID, message);
    }
}

void CComClientBuffer::onClientDisconnected()
{
    bInfo(m_context) << "Client disconnected!";
    emit doDisconnected(m_client_id);
}

} // namespace BlackMisc
