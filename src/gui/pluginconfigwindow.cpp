// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/pluginconfigwindow.h"

#include <QApplication>
#include <QStyle>
#include <Qt>
#include <QtGlobal>

namespace swift::gui
{
    CPluginConfigWindow::CPluginConfigWindow(QWidget *parent) : COverlayMessagesFrame(parent, Qt::Window)
    {
        this->setWindowModality(Qt::WindowModal);
    }
}