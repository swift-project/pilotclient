#include <blackmisc/context.h>
#include <blackmisc/debug.h>

#include <QStringList>
#include <QTextStream>

#include <blackcore/fsd_client.h>
#include <blackcore/fsd_messages.h>

namespace FSD
{

	using namespace BlackMisc;


	CFSDClient::CFSDClient()
		:   m_tcp_socket(NULL), m_port(0), m_simType(FSD::SIM_UNKNOWN)
	{
		init();
	}

	void CFSDClient::connectTo(const QString &host, quint16 port)
	{
		m_host = host;
		m_port = port;

		if ( isConnected() )
			return;

		bDebug << "Connecting to FSD server: " << m_host << ":" << m_port;

		m_tcp_socket->connectToHost(m_host, m_port);
	}

	void CFSDClient::disconnectFrom()
	{
		bDebug << "Disconnecting from host.";

		m_tcp_socket->disconnectFromHost();
		m_port = 0;
		m_host.clear();
	}

	void CFSDClient::reconnect()
	{

	}

	void CFSDClient::updateClientInfo(TClientInfo &clientInfo )
	{
		m_host = clientInfo.m_host;
		m_port = clientInfo.m_port;
		m_password = clientInfo.m_password;
		m_callsign = clientInfo.m_callsign;
		m_userid = clientInfo.m_userid;
		m_simType = clientInfo.m_simType;
		m_realName = clientInfo.m_realName;
	}

	bool CFSDClient::isConnected()
	{
		return m_tcp_socket->state() == QAbstractSocket::ConnectedState;
	}

	QString CFSDClient::getErrorMessage(QAbstractSocket::SocketError error)
	{
		return QString("Unknown");
	}

	void CFSDClient::onConnected()
	{
		bDebug << "Connected successfully to remote host.";
		bDebug << "Sending #AP now...";

		QString line;

		FSD_MSG_AddPilot addPilot;
		addPilot.setSource(m_callsign);
		addPilot.setDest("SERVER");
		addPilot.setUserID(m_userid);
		addPilot.setPassword(m_password);
		addPilot.setSimulator((quint16)m_simType);
		addPilot.setRealName(m_realName);

		QTextStream stream(&line);
		addPilot >> stream;

		sendMessage(line);

		emit doConnected();
	}

	void CFSDClient::onDisconnected()
	{
		bDebug << "Disconnected successfully from remote host.";
		emit doDisconnected();
	}

	void CFSDClient::onError(QAbstractSocket::SocketError error)
	{

		if ( error != 0 )
		{
			bError << "Received socket error: " << error << " - Disconnecting...";
		}

		disconnectFrom();
		emit doError(error, getErrorMessage(error) );
	}

	void CFSDClient::onReceivingData()
	{
		QTextStream input(m_tcp_socket);
		QString line;

		do
		{
			line = input.readLine();
			if (!line.isNull())
				processLine(line);
		} while(!line.isNull());
	}

	bool CFSDClient::sendMessage(const QString &message)
	{
		if (!isConnected())
		{
			bError << "Cannot send data in disconnected state!";
			return false;
		}

		qint64 message_size = message.size();

		qint64 bytes = m_tcp_socket->write(message.toAscii());
		if (bytes < 0 || bytes != message_size)
		{
			bWarning << "Error writing to socket!";
			return false;
		}

		return true;
	}

	void CFSDClient::init()
	{
		m_tcp_socket = new QTcpSocket(this);
		bAssert(m_tcp_socket);

		bAssert ( QObject::connect( m_tcp_socket, SIGNAL( connected() ), this, SLOT( onConnected() ) ) );
		bAssert ( QObject::connect( m_tcp_socket, SIGNAL( disconnected() ), this, SLOT( onDisconnected() ) ) );
		bAssert ( QObject::connect( m_tcp_socket, SIGNAL( error(QAbstractSocket::SocketError) ), this, SLOT( onError(QAbstractSocket::SocketError) ) ) );

		bAssert ( QObject::connect( m_tcp_socket, SIGNAL( readyRead() ), this, SLOT( onReceivingData() ) ) );

		registerMessages();
	}

	void CFSDClient::registerMessages()
	{
		REGISTER_MESSAGE(FSD::FSD_MSG_TextMessage, #TM);
	}

	void CFSDClient::processLine(QString &line)
	{
		QString header;
		for (int ii = 0; ii < MAX_FSD_HEADERS; ii++)
		{
			QString head = FSD::Headers[ii];
			if(line.startsWith( head ))
			{
			   header = FSD::Headers[ii];
			   line.remove(0, FSD::Headers[ii].length());
			}
		}

		if (header.isEmpty())
			return;

		QTextStream stream(&line);
		IMessage* fsd_message =  CMessageFactory::getInstance().create(header);
		bAssert(fsd_message);
		*fsd_message << stream;

		CMessageDispatcher::getInstance().append(fsd_message);
	}

	

} // namespace FSD
