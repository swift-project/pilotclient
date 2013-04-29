//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef COM_CLIENT_BUFFER_H
#define COM_CLIENT_BUFFER_H

#include "blackmisc/com_handler.h"
#include <QObject>

class QTcpSocket;

namespace BlackMisc
{

class IContext;

class CComClientBuffer : public IComHandler
{
    Q_OBJECT

public:
    CComClientBuffer(IContext &context, uint clientID, QTcpSocket *socket, QObject *parent = 0);

    virtual ~CComClientBuffer();

    //! Sends a message to the remote host.
    /*!
      \param data Reference to the raw byte data to be sent.
      \return Returns true if sending was successfull, otherwise false.
    */
    bool sendMessage(const QString &id, const QByteArray &message);

signals:

    void doReceivedMessage(uint clientID, QString &messageID, QByteArray &message);
    void doDisconnected(uint clientID);

protected slots:
    void onReceivingData();
    void onClientDisconnected();

protected:
    IContext       &m_context;
    QTcpSocket     *m_tcp_socket;
    uint            m_client_id;
};

} // namespace BlackMisc

#endif // COM_CLIENT_BUFFER_H
