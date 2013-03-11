//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include <QTcpSocket>
#include <QDataStream>

#include "blackmisc/debug.h"
#include "blackmisc/com_client.h"

namespace BlackMisc
{

    CComClient::CComClient(QObject *parent)
        :  IComHandler(parent), m_tcp_socket(NULL), m_port(0)
    {
        init();
    }

    CComClient::~CComClient()
    {
        if ( isConnected() )
            disconnect();

        if ( m_tcp_socket != NULL )
            delete m_tcp_socket;

        m_tcp_socket = NULL;
    }

    bool CComClient::init()
    {
        m_tcp_socket = new QTcpSocket(this);
        bAssert(m_tcp_socket);

        bAssert ( QObject::connect( m_tcp_socket, SIGNAL( connected() ), this, SLOT( onConnected() ) ) );
        bAssert ( QObject::connect( m_tcp_socket, SIGNAL( disconnected() ), this, SLOT( onDisconnected() ) ) );
        bAssert ( QObject::connect( m_tcp_socket, SIGNAL( error(QAbstractSocket::SocketError) ), this, SLOT( onError(QAbstractSocket::SocketError) ) ) );

        bAssert ( QObject::connect( m_tcp_socket, SIGNAL( readyRead() ), this, SLOT( onReceivingData() ) ) );

        return true;
    }

    void CComClient::connectTo( const QString& hostName, quint16 port)
    {
        bAssert ( ! hostName.isEmpty() );
        bAssert ( port > 0 );

        m_hostName = hostName;
        m_port = port;

        if ( isConnected() )
            return;

        bDebug << "Connecting to host: " << hostName << ":" << port;

        m_receive_buffer.clear();

        m_tcp_socket->connectToHost(hostName, port);

    }

    bool CComClient::isConnected() const
    {
        return m_tcp_socket->state() == QAbstractSocket::ConnectedState;
    }

    void CComClient::disconnectFrom()
    {
        bDebug << "Disconnecting from host.";

        m_tcp_socket->disconnectFromHost();
        m_port = 0;
        m_hostName.clear();
    }

    bool CComClient::sendMessage(const QString& messageID, const QByteArray &message)
    {
        if (!isConnected())
        {
            bError << "Cannot send data in disconnected state!";
            return false;
        }

        createFrame(messageID, message);

        qint64 sender_buffer_size = m_sender_buffer.size();

        qint64 bytes = m_tcp_socket->write(m_sender_buffer);
        if (bytes < 0 || bytes != sender_buffer_size)
        {
            bWarning << "Error writing to socket!";
            return false;
        }

        return true;
    }

    QString CComClient::getErrorMessage(QAbstractSocket::SocketError error)
    {
        QString message;

        /*
        ConnectionRefusedError,
        RemoteHostClosedError,
        HostNotFoundError,
        SocketAccessError,
        SocketResourceError,
        SocketTimeoutError,
        DatagramTooLargeError,
        NetworkError,
        AddressInUseError,
        SocketAddressNotAvailableError,
        UnsupportedSocketOperationError,
        UnfinishedSocketOperationError,
        ProxyAuthenticationRequiredError,
        SslHandshakeFailedError,
        ProxyConnectionRefusedError,
        ProxyConnectionClosedError,
        ProxyConnectionTimeoutError,
        ProxyNotFoundError,
        ProxyProtocolError,
        UnknownSocketError = -1 */

        switch (error)
        {
        case QAbstractSocket::ConnectionRefusedError:
            message = "Connection refused by host!";
            break;

        case QAbstractSocket::RemoteHostClosedError:
            message = "Connection closed by host!";
            break;

        case QAbstractSocket::HostNotFoundError:
            message = "Host not found!";
            break;

        default:
        case QAbstractSocket::UnknownSocketError:
            message = "Unknown error!";
            break;
        }

        return message;
    }

    void CComClient::onConnected()
    {
        bDebug << "Connected successfully to remote host.";
        emit doConnected();
    }

    void CComClient::onDisconnected()
    {
        bDebug << "Disconnected successfully from remote host.";
        emit doDisconnected();
    }

    void CComClient::onError(QAbstractSocket::SocketError error)
    {
        if ( error != 0 )
        {
            bError << "Received socket error: " << error << " - Disconnecting...";
        }

        disconnect();
        emit doError(error, getErrorMessage(error) );
    }

    void CComClient::onReceivingData()
    {
        QString messageID;
        QByteArray message;
        m_receive_buffer.append(m_tcp_socket->readAll());
        while (parseFrame(messageID, message))
        {
            emit doReceivedMessage(messageID, message);
        }
    }

} // namespace BlackMisc


