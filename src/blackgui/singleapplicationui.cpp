/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "singleapplicationui.h"
#include "blackgui/guiapplication.h"
#include <QWidget>

using namespace BlackMisc;

namespace BlackGui
{
    CSingleApplicationUi::CSingleApplicationUi(QWidget *ui) : m_ui(ui)
    {
        QObject::connect(&m_timer, &QTimer::timeout, [ = ] { this->evaluate(); });
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
} // ns
