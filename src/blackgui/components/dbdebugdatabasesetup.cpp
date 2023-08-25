// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/dbdebugdatabasesetup.h"
#include "blackgui/guiapplication.h"
#include "ui_dbdebugdatabasesetup.h"

#include <QCheckBox>

using namespace BlackCore::Data;
using namespace BlackMisc;

namespace BlackGui::Components
{
    CDbDebugDatabaseSetup::CDbDebugDatabaseSetup(QWidget *parent) : QFrame(parent),
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
    {}

    void CDbDebugDatabaseSetup::onDebugChanged(bool set)
    {
        CGlobalSetup gs(m_setup.getThreadLocal());
        gs.setServerDebugFlag(set);
        m_setup.set(gs);
    }

} // ns
