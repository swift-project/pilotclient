// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "networksetupform.h"
#include "ui_networksetupform.h"
#include "blackgui/guiutility.h"

using namespace BlackMisc;

namespace BlackGui::Editors
{
    CNetworkSetupForm::CNetworkSetupForm(QWidget *parent) : CForm(parent),
                                                            ui(new Ui::CNetworkSetupForm)
    {
        ui->setupUi(this);
    }

    CNetworkSetupForm::~CNetworkSetupForm()
    {}

    void CNetworkSetupForm::setReadOnly(bool readonly)
    {
        CGuiUtility::checkBoxReadOnly(ui->cb_DynamicOffsetTimes, readonly);
    }
} // ns
