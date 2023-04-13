/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/plugindetailswindow.h"
#include "ui_plugindetailswindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QFont>
#include <QLabel>
#include <QPushButton>
#include <QStyle>
#include <Qt>
#include <QtGlobal>

namespace BlackGui
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

    void CPluginDetailsWindow::showEvent(QShowEvent *event)
    {
        this->setGeometry(
            QStyle::alignedRect(
                Qt::LeftToRight,
                Qt::AlignCenter,
                this->size(),
                QDesktopWidget().screenGeometry(qApp->activeWindow())));
        Q_UNUSED(event);
    }
}
