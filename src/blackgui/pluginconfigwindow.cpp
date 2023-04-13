/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
