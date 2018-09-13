/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackgui/loginmodebuttons.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "ui_loginmodebuttons.h"

#include <QRadioButton>

using namespace BlackConfig;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui
{
    CLoginModeButtons::CLoginModeButtons(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CLoginModeButtons)
    {
        ui->setupUi(this);
        configureLoginModes();
        connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this, &CLoginModeButtons::configureLoginModes, Qt::QueuedConnection);
    }

    CLoginModeButtons::~CLoginModeButtons()
    { }

    BlackCore::INetwork::LoginMode BlackGui::CLoginModeButtons::getLoginMode() const
    {
        INetwork::LoginMode mode = INetwork::LoginNormal;
        if (ui->rb_LoginStealth->isChecked())
        {
            mode = INetwork::LoginStealth;
        }
        else if (ui->rb_LoginObserver->isChecked())
        {
            mode = INetwork::LoginAsObserver;
        }
        return mode;
    }

    void CLoginModeButtons::setLoginMode(INetwork::LoginMode mode)
    {
        switch (mode)
        {
        case INetwork::LoginAsObserver:
            ui->rb_LoginObserver->setChecked(true);
            break;
        case INetwork::LoginStealth:
            ui->rb_LoginStealth->setChecked(true);
            break;
        default:
        case INetwork::LoginNormal:
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
        if (!sGui->isDeveloperFlagSet() && !sGui->getIContextSimulator()->isSimulatorSimulating())
        {
            // Disable pilot login modes
            ui->rb_LoginNormal->setEnabled(false);
            ui->rb_LoginStealth->setEnabled(false);
            ui->rb_LoginNormal->setToolTip("No simulator available");
            ui->rb_LoginStealth->setToolTip("No simulator available");
            ui->rb_LoginObserver->setChecked(true);
        }
        else
        {
            ui->rb_LoginNormal->setEnabled(true);
            ui->rb_LoginStealth->setEnabled(true);
            ui->rb_LoginNormal->setToolTip({});
            ui->rb_LoginStealth->setToolTip({});
            ui->rb_LoginNormal->setChecked(true);
        }
    }
} // ns
