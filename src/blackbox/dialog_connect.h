//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef DIALOG_CONNECT_H
#define DIALOG_CONNECT_H

#include <QDialog>
#include "blackmisc/com_client.h"

namespace Ui {
class CDialogConnect;
}

class CDialogConnect : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDialogConnect(QWidget *parent = 0);
    ~CDialogConnect();

protected slots:
    void onFSDConnect();
    void onError(QAbstractSocket::SocketError, QString);
    void onClientConnected();
    
private:
    Ui::CDialogConnect *ui;
    BlackMisc::CComClient comclient;

    bool m_hasComConnection;
};

#endif // DIALOG_CONNECT_H
