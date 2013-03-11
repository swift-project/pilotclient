//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef FSD_CLIENT_H
#define FSD_CLIENT_H

#include <QObject>
#include <QList>
#include <QString>
#include <QTcpSocket>

#include <blackcore/fsd_protocol.h>


namespace FSD
{

typedef struct {
    QString                 m_host;
    quint16                 m_port;
    QString                 m_callsign;
    QString                 m_userid;
    QString                 m_password;
    FSD::SimulatorType      m_simType;
    QString                 m_realName;
} TClientInfo;

class CFSDClient  : public QObject
{
    Q_OBJECT

public:
	CFSDClient();

    void connectTo(const QString &host, quint16 port);
    void disconnectFrom();
    void reconnect();

    void updateClientInfo(TClientInfo &clientInfo );

    TClientInfo& clientInfo();

    bool isConnected();

    //! Returns a human readable description of the last error that occurred.
    QString getErrorMessage(QAbstractSocket::SocketError error);

    bool sendMessage(const QString &message);

    //////////////////////////////////
    // FSD specific senders functions
    //////////////////////////////////

    void    sendText                ( QString &message);

    void    sendTextonRadios        ( QList<qint16> &frequencies, QString &message);

    void    sendClientQuery         ( TQueryType type, QString &callsign);

    void    sendClientQueryReponse  ( TQueryType type, QString &data);

    void    sendPilotPosition       ( TPositionMessage *posMessage );

    void    sendFlightPlan          ();

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

    //! Call this slot .
    /*!
      \param data Reference to the raw byte data to be sent.
      \return Returns true if sending was successfull, otherwise false.
    */
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void onReceivingData();

protected:

	void sendAddPilot();



private:

    CFSDClient( const CFSDClient& other);
    const CFSDClient& operator = ( const CFSDClient& other);

    void init();
    void registerMessages();

    void processLine(QString &line);

    QTcpSocket*             m_tcp_socket;

    QString                 m_host;
    quint16                 m_port;
    QString                 m_callsign;
    QString                 m_userid;
    QString                 m_password;
    FSD::SimulatorType      m_simType;
    QString                 m_realName;

    QString                 m_last_error;

};

} // namespace FSD

#endif // FSD_CLIENT_H
