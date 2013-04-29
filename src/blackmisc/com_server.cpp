//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "blackmisc/com_server.h"
#include "blackmisc/com_client_buffer.h"
#include "blackmisc/debug.h"
#include "blackmisc/context.h"
#include <QTcpServer>
#include <QTcpSocket>

namespace BlackMisc
{

CComServer::CComServer(IContext &context, QObject *parent)
    : QObject(parent), m_context(context), m_tcp_server(NULL), m_port(0)
{
    init();
}

CComServer::~CComServer()
{
    //QObject::disconnect(m_tcp_server, SIGNAL(newConnection()), this, SLOT(onIncomingConnection()) );

    TClientBufferHash::iterator it = m_client_buffers.begin();
    while (it != m_client_buffers.end())
    {
        it.value()->deleteLater();
        ++it;
    }

    m_tcp_server->close();
    m_tcp_server->deleteLater();
}

bool CComServer::init()
{
    m_tcp_server = new QTcpServer(this);
    Q_ASSERT(m_tcp_server);
    Q_ASSERT(QObject::connect(m_tcp_server, SIGNAL(newConnection()), this, SLOT(onIncomingConnection())));

    return true;
}

void CComServer::Host(const QHostAddress &address, const quint16 port)
{
    if (isHosting()) return;

    Q_ASSERT(! address.isNull());
    Q_ASSERT(port > 0);

    if (!m_tcp_server->listen(address, port))
    {
        bError(m_context) << "Hosting failed";
        emit doHostClosed();
    }
    else
    {
        bDebug(m_context) << "Hosting successfull";
        emit doHosting();
    }
}

bool CComServer::isHosting() const
{
    return m_tcp_server->isListening();
}

void CComServer::close()
{
    m_tcp_server->close();
}

void CComServer::sendToClient(const uint clientID, const QString &messageID, const QByteArray &data)
{
    if (!m_client_buffers.contains(clientID))
    {
        bWarning(m_context) << "Cannot send data to client - unknown client!";
        return;
    }
    m_client_buffers.value(clientID)->sendMessage(messageID, data);
}

void CComServer::sendToAll(const QString &messageID, const QByteArray &data)
{
    TClientBufferHash::const_iterator it = m_client_buffers.constBegin();
    while (it != m_client_buffers.constEnd())
    {
        it.value()->sendMessage(messageID, data);
        ++it;
    }
}

QString CComServer::getErrorMessage(const QAbstractSocket::SocketError error)
{
    return QString();
}

void CComServer::onIncomingConnection()
{
    while (m_tcp_server->hasPendingConnections())
    {
        QTcpSocket *socket = m_tcp_server->nextPendingConnection();
        uint clientID = qHash(socket);

        // Create new ClientBuffer object. This new object gets the owner of the socket
        CComClientBuffer *clientbuf = new CComClientBuffer(m_context, clientID, socket, this);
        Q_ASSERT(clientbuf);

        connect(clientbuf, SIGNAL(doDisconnected(uint)), this, SLOT(onClientDisconnected(uint)));
        connect(clientbuf, SIGNAL(doReceivedMessage(uint, QString &, QByteArray &)), this, SLOT(onClientMessageReceived(uint, QString &, QByteArray &)));

        m_client_buffers.insert(clientID, clientbuf);

        emit doClientConnected();
    }
}

void CComServer::onClientDisconnected(uint clientID)
{
    if (!m_client_buffers.contains(clientID))
    {
        bWarning(m_context) << "Disconnected unknown client!";
        return;
    }

    m_client_buffers.take(clientID)->deleteLater();
}

void CComServer::onClientMessageReceived(uint clientID, QString &messageID, QByteArray &message)
{
    emit doMessageReceived(messageID, message);
}

} // namespace BlackMisc
