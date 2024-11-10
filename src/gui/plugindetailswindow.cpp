// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/plugindetailswindow.h"
#include "ui_plugindetailswindow.h"

#include <QApplication>
#include <QFont>
#include <QLabel>
#include <QPushButton>
#include <QStyle>
#include <Qt>
#include <QtGlobal>

namespace swift::gui
{
    CPluginDetailsWindow::CPluginDetailsWindow(QWidget *parent) : QWidget(parent, Qt::Dialog),
                                                                  ui(new Ui::CPluginDetailsWindow)
    {
        ui->setupUi(this);
        this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
        setWindowModality(Qt::WindowModal);

        QFont font = ui->lbl_PluginName->font();
        font.setPointSize(font.pointSize() + 2);
        ui->lbl_PluginName->setFont(font);

        connect(ui->pb_Close, &QPushButton::clicked, this, &CPluginDetailsWindow::close);
    }

    CPluginDetailsWindow::~CPluginDetailsWindow()
    {}

    void CPluginDetailsWindow::setPluginName(const QString &name)
    {
        ui->lbl_PluginName->setText(name);
        this->setWindowTitle(name);
    }

    void CPluginDetailsWindow::setPluginIdentifier(const QString &id)
    {
        ui->lbl_PluginIdentifier->setText(id);
    }

    void CPluginDetailsWindow::setPluginDescription(const QString &description)
    {
        ui->lbl_Description->setText(description);
    }

    void CPluginDetailsWindow::setPluginAuthors(const QString &authors)
    {
        ui->lbl_Authors->setText(authors);
    }
}
