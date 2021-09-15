/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "networksetupform.h"
#include "ui_networksetupform.h"
#include "guiutility.h"

using namespace BlackMisc;

namespace BlackGui::Editors
{
    CNetworkSetupForm::CNetworkSetupForm(QWidget *parent) :
        CForm(parent),
        ui(new Ui::CNetworkSetupForm)
    {
        ui->setupUi(this);
    }

    CNetworkSetupForm::~CNetworkSetupForm()
    { }

    void CNetworkSetupForm::setReadOnly(bool readonly)
    {
        CGuiUtility::checkBoxReadOnly(ui->cb_DynamicOffsetTimes, readonly);
    }
} // ns
