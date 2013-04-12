//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef COM_HANDLER_H
#define COM_HANDLER_H

#include <QObject>

class QTcpSocket;

namespace BlackMisc
{

const qint32 Sync_Marker = 0x1ACFFC1D;


//! IComHandler Interface.
/*!
  This interface implements the basic class for every InterCommunikation
  objects. It creates the frames in which the data is packed and
  deframes it, when it receives something from the TCP socket.
  \sa CComClient CComServer
*/
class IComHandler : public QObject
{
    Q_OBJECT

public:
    //! Constructor
    /*!
      \param parent Pointer to the parent QObject
    */
    explicit IComHandler(QObject *parent = 0);

    //! Virtual destructor
    virtual ~IComHandler() {}

protected:

    //! Creates a sendable frame containing some data.
    /*!
      \param messageID QString with the unique messageID
      \param message The actual data
      \sa IMessage
    */
    void createFrame(const QString &messageID, const QByteArray &data);

    //! Parses a new frame and constructs messageID and data out of it
    /*!
      \param messageID Reference to the QString messageID
      \param message Reference where the data will be stored in.
      \sa IMessage
    */
    bool parseFrame(QString &messageID, QByteArray &data);

    //! Receive Buffer
    /*!
      Data received from the TCP socket, will stored in here.
    */
    QByteArray      m_receive_buffer;

    //! Sender Buffer
    /*!
      Sending data via the TCP socket, should be stored in here.
    */
    QByteArray      m_sender_buffer;

};

} // namespace BlackMisc

#endif // COM_HANDLER_H
