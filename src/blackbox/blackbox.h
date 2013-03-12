//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef BLACKBOX_H
#define BLACKBOX_H

#include <QDialog>

class CDialogConnect;
class CDialogChat;

namespace Ui {
class BlackBox;
}

class BlackBox : public QDialog
{
    Q_OBJECT
    
public:
    explicit BlackBox(QWidget *parent = 0);
    ~BlackBox();

protected slots:
    void onConnect();
    void onButtonChat();
    
private:
    Ui::BlackBox *ui;

    CDialogConnect *m_dlg_connect;
    CDialogChat     *m_dlg_chat;
};

#endif // BLACKBOX_H
