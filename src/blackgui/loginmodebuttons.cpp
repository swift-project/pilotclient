// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "ui_loginmodebuttons.h"

#include "config/buildconfig.h"
#include "blackgui/loginmodebuttons.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "blackmisc/verify.h"

#include <QRadioButton>

using namespace swift::config;
using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackMisc::Network;

namespace BlackGui
{
    CLoginModeButtons::CLoginModeButtons(QWidget *parent) : QFrame(parent),
                                                            ui(new Ui::CLoginModeButtons)
    {
        ui->setupUi(this);
        ui->lbl_NoSimulator->setVisible(false);
        this->configureLoginModes();
        connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this, &CLoginModeButtons::configureLoginModes, Qt::QueuedConnection);
    }

    CLoginModeButtons::~CLoginModeButtons()
    {}

    CLoginMode BlackGui::CLoginModeButtons::getLoginMode() const
    {
        CLoginMode mode = CLoginMode::Pilot;
        if (ui->rb_LoginObserver->isChecked())
        {
            mode.setLoginMode(CLoginMode::Observer);
        }
        return mode;
    }

    void CLoginModeButtons::setLoginMode(CLoginMode mode)
    {
        switch (mode.getLoginMode())
        {
        case CLoginMode::Observer:
            ui->rb_LoginObserver->setChecked(true);
            break;
        default:
        case CLoginMode::Pilot:
            ui->rb_LoginNormal->setChecked(true);
            break;
        }
    }

    void CLoginModeButtons::setReadOnly(bool readonly)
    {
        ui->rb_LoginNormal->setEnabled(!readonly);
        ui->rb_LoginObserver->setEnabled(!readonly);
        ui->rb_LoginStealth->setEnabled(!readonly);
    }

    void CLoginModeButtons::configureLoginModes()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }

        // we have no idea how we can get here without the context existing Ref T389
        if (CBuildConfig::isLocalDeveloperDebugBuild() && !sGui->getIContextSimulator())
        {
            // how is this possible? In debug builds I do crosscheck
            Q_ASSERT_X(false, Q_FUNC_INFO, "No context or sGUI");
        }

        if (!sGui->getIContextSimulator() || (!sGui->isDeveloperFlagSet() && !sGui->getIContextSimulator()->isSimulatorSimulating()))
        {
            // Disable pilot login modes, only observer
            ui->rb_LoginNormal->setEnabled(false);
            ui->rb_LoginStealth->setEnabled(false);
            ui->rb_LoginNormal->setToolTip("No simulator available");
            ui->rb_LoginStealth->setToolTip("No simulator available");
            ui->rb_LoginObserver->setChecked(true);
            ui->lbl_NoSimulator->setVisible(true);
        }
        else
        {
            ui->rb_LoginNormal->setEnabled(true);
            ui->rb_LoginStealth->setEnabled(true);
            ui->rb_LoginNormal->setToolTip({});
            ui->rb_LoginStealth->setToolTip({});
            ui->rb_LoginNormal->setChecked(true);
            ui->lbl_NoSimulator->setVisible(false);
        }

        ui->rb_LoginStealth->setVisible(false); // 2019-01 hide as based on discussion with RR
    }
} // ns
