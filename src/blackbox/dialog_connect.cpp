//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "dialog_connect.h"
#include "ui_dialog_connect.h"

#include <blackmisc/debug.h>
#include <blackmisc/message.h>
#include <blackmisc/gui_messages.h>

using namespace BlackMisc;

CDialogConnect::CDialogConnect(QWidget *parent) :
    QDialog(parent), m_hasComConnection(false),
    ui(new Ui::CDialogConnect)
{
    ui->setupUi(this);

    connect(&comclient, SIGNAL(doError(QAbstractSocket::SocketError,QString)), this, SLOT(onError(QAbstractSocket::SocketError,QString)));
    connect(&comclient, SIGNAL(doConnected()), this, SLOT(onClientConnected()));
	connect(ui->bt_FSDConnect, SIGNAL(clicked()), this, SLOT(onFSDConnect()));

    QString address = "127.0.0.1";
    comclient.connectTo(address, 42000);
}

CDialogConnect::~CDialogConnect()
{
    delete ui;
}

void CDialogConnect::onFSDConnect()
{
    QByteArray message_data;
    QDataStream out(&message_data, QIODevice::WriteOnly);
    MSG_CONNECT_TO_VATSIM* msgConnect = new MSG_CONNECT_TO_VATSIM();

	msgConnect->setHost(ui->m_host->text());
	msgConnect->setCallsign(ui->m_callsign->text());
	msgConnect->setUserID(ui->m_ID->text());
	msgConnect->setPassword(ui->m_password->text());
	msgConnect->setPort(ui->m_port->text().toUInt());
	msgConnect->setRealName(ui->m_name->text());

    *msgConnect >> out;

    comclient.sendMessage(msgConnect->getID(), message_data);
    delete msgConnect;
}

void CDialogConnect::onError(QAbstractSocket::SocketError,QString)
{

}

void CDialogConnect::onClientConnected()
{
    m_hasComConnection = true;
}
