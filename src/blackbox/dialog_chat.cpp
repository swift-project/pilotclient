//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "dialog_chat.h"
#include "ui_dialog_chat.h"

CDialogChat::CDialogChat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDialogChat)
{
    ui->setupUi(this);
	ui->chatGeneral->textCursor().insertText("Testline");
    ui->chatGeneral->centerCursor();
}

CDialogChat::~CDialogChat()
{
    delete ui;
}
