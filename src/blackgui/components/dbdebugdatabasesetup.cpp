/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/dbdebugdatabasesetup.h"
#include "blackgui/guiapplication.h"
#include "ui_dbdebugdatabasesetup.h"

#include <QCheckBox>

using namespace BlackCore::Data;
using namespace BlackMisc;

namespace BlackGui::Components
{
    CDbDebugDatabaseSetup::CDbDebugDatabaseSetup(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CDbDebugDatabaseSetup)
    {
        ui->setupUi(this);
        const bool enabled = sGui->isDeveloperFlagSet();
        this->setEnabled(enabled);
        if (!enabled)
        {
            this->setToolTip("Disabled, cannot be set!");
        }
        else
        {
            connect(ui->cb_EnableServerDebugging, &QCheckBox::toggled, this, &CDbDebugDatabaseSetup::onDebugChanged);
        }
    }

    CDbDebugDatabaseSetup::~CDbDebugDatabaseSetup()
    { }

    void CDbDebugDatabaseSetup::onDebugChanged(bool set)
    {
        CGlobalSetup gs(m_setup.getThreadLocal());
        gs.setServerDebugFlag(set);
        m_setup.set(gs);
    }

} // ns
