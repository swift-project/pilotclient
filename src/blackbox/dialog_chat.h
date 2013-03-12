//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef DIALOG_CHAT_H
#define DIALOG_CHAT_H

#include <QDialog>

namespace Ui {
class CDialogChat;
}

class CDialogChat : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDialogChat(QWidget *parent = 0);
    ~CDialogChat();
    
private:
    Ui::CDialogChat *ui;
};

#endif // DIALOG_CHAT_H
