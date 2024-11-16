// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "singleapplicationui.h"

#include <QWidget>

#include "gui/guiapplication.h"

using namespace swift::misc;

namespace swift::gui
{
    CSingleApplicationUi::CSingleApplicationUi(QWidget *ui) : m_ui(ui)
    {
        QObject::connect(&m_timer, &QTimer::timeout, [=] { this->evaluate(); });
        m_timer.setInterval(5000);
        m_timer.start();
    }

    void CSingleApplicationUi::evaluate()
    {
        if (!sGui) { return; }
        if (!m_ui) { return; }
        const CApplicationInfoList apps = sGui->getRunningApplications();
        const bool enable = apps.size() < 2;
        m_ui->setEnabled(enable);
    }
} // namespace swift::gui
