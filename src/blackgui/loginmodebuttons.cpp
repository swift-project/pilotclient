/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/loginmodebuttons.h"
#include "ui_loginmodebuttons.h"

#include <QRadioButton>

using namespace BlackCore;

namespace BlackGui
{
    CLoginModeButtons::CLoginModeButtons(QWidget *parent) :
        QGroupBox(parent),
        ui(new Ui::CLoginModeButtons)
    {
        ui->setupUi(this);
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
}
