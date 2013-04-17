//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <QHash>
#include <QHostAddress>
#include <QTcpSocket>

class QTcpServer;

namespace BlackMisc
{

    class CComClientBuffer;
    class IContext;

	class CComServer : public QObject
	{
		Q_OBJECT

	public:
		
		//! Constructor
		/*!
          \param context
		  \param parent Pointer to the parent QObject
		*/
		CComServer(IContext &context, QObject *parent = 0);

		//! Destructor
		~CComServer();

		//! This method initializes the server
		/*!
		  \return Returns true, if successfull, otherwise false
		*/
		bool    init ();

		//! Call this method to start hosting
		void    Host ( const QHostAddress & address, const quint16 port);

		//! Hosting status
		/*!
		  \return Returns true if hosting, otherwise false
		*/
		bool    isHosting () const;

		//! Closes the opened hosting session
		void    close();

		//! Sends data to a client
		/*!
		  \param clientID ID of the target client
		  \param messageID ID of the message, to be sent to the client
		  \param data The actual data/message
		*/
		void    sendToClient(const quint32 clientID, const QString &messageID, const QByteArray& data);

		//! Sends data to all clients
		/*!
		  \param messageID ID of the message, to be sent to the client
		  \param data The actual data/message
		*/
		void    sendToAll(const QString &messageID, const QByteArray& data);

		//! Call this method to get last error
		/*!
		  \return Returns a string representation of the last error.
		*/
		QString getErrorMessage( const QAbstractSocket::SocketError error);

	signals:

		void            doHostClosed();
		void            doHosting();
		void            doClientConnected();
		void            doClientDisconnected();
		void            doMessageReceived(QString &, QByteArray &);
		void            doMessageReceived();

	protected slots:

		void onIncomingConnection();

		void onClientDisconnected(uint clientID);

		void onClientMessageReceived(uint clientHash, QString &messageID, QByteArray &message);

	protected:

        IContext&       m_context;

		QTcpServer*     m_tcp_server;

		QHostAddress    m_address;
		quint16         m_port;

		typedef QHash<uint, CComClientBuffer*> TClientBufferHash;
		TClientBufferHash m_client_buffers;
	};

} // namespace BlackMisc

#endif // TCP_SERVER_H
