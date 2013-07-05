//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "dialog_connect.h"
#include "dialog_chat.h"

#include "blackbox.h"
#include "ui_blackbox.h"

BlackBox::BlackBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BlackBox)
{
    ui->setupUi(this);

    m_dlg_connect = new CDialogConnect();
    m_dlg_connect->hide();

    m_dlg_chat = new CDialogChat();
    m_dlg_chat->hide();

    connect(ui->bt_Connect, &QPushButton::clicked,
            this,           &BlackBox::onConnect);
    connect(ui->bt_Chat,    &QPushButton::clicked,
            this,           &BlackBox::onButtonChat);
}

BlackBox::~BlackBox()
{
    delete ui;
}

void BlackBox::onConnect()
{
    if (m_dlg_connect->isHidden())
    {
        m_dlg_connect->show();
    }

}

void BlackBox::onButtonChat()
{
    if (m_dlg_chat->isHidden())
    {
        m_dlg_chat->show();
    }
}
