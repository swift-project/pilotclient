//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef COM_CLIENT_H
#define COM_CLIENT_H

#include "blackmisc/com_handler.h"
#include <QString>
#include <QAbstractSocket>

class QTcpSocket;

namespace BlackMisc
{

    class IContext;

class CComClient : public IComHandler
{
    Q_OBJECT

public:
	
	//! Constructor
	/*!
	  \param parent Pointer to the parent QObject
	*/
    CComClient(IContext &context, QObject *parent = 0);

	//! Destructor
    ~CComClient();

	//! This method initializes the client
    virtual bool init();

    //! Connects to the host address.
    /*!
      \param host Host address of the remote host.
      \param port Port number of the remote host.
      \return Returns true if connecting was successfull, otherwise false.
    */
    void connectTo (const QString& hostName, quint16 port);

    //! Returns the connection status
    /*!
      \return Returns true if connected, otherwise false
    */
    bool isConnected() const;

    //! Disconnects from the current server
    void disconnectFrom();

    //! Sends a message to the remote host.
    /*!
      \param data Reference to the raw byte data to be sent.
      \return Returns true if sending was successfull, otherwise false.
    */
    bool sendMessage (const QString &messageID, const QByteArray &message);

    //! Returns a human readable description of the last error that occurred.
    QString getErrorMessage(QAbstractSocket::SocketError error);

signals:

    //! emitted when new data were received
    void doReceivedMessage(QString &messageID, QByteArray &message);

    //! emitted when cliebt connected
    void doConnected();

    //! emitted when client disconnected
    void doDisconnected();

    //! emitted when an error has occured
    void doError(QAbstractSocket::SocketError error, const QString& error_message);

protected slots:

    //! Call this slot, when connected succesfully
    void onConnected();
	
	//! Call this slot, when disconnected succesfully
    void onDisconnected();
	
	//! Call this slot, when an error appeared
    void onError(QAbstractSocket::SocketError error);
	
	//! Call this slot, when data has been received
    void onReceivingData();

private:
    IContext&       m_context;

protected:
	
	//! TCP Socket
	/*!
	  Pointer to the tcp socket.
	*/
    QTcpSocket*     m_tcp_socket;

	//! Remote hostname
    QString         m_hostName;

	//! Remote host port
    quint16         m_port;

	//! This variable holds the last appeared error
    QString         m_last_error;

private:

    CComClient( const CComClient& other);
    const CComClient& operator = ( const CComClient& other);
};

} // namespace BlackMisc

#endif // COM_CLIENT_H
