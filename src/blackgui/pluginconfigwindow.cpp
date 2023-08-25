// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/pluginconfigwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QStyle>
#include <Qt>
#include <QtGlobal>

namespace BlackGui
{
    CPluginConfigWindow::CPluginConfigWindow(QWidget *parent) : COverlayMessagesFrame(parent, Qt::Window)
    {
        this->setWindowModality(Qt::WindowModal);
    }

    void CPluginConfigWindow::showEvent(QShowEvent *event)
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
