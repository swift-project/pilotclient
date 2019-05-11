/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "networkdetailsdialog.h"
#include "ui_networkdetailsdialog.h"

using namespace BlackMisc::Network;
using namespace BlackMisc::Audio;
using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {
        CNetworkDetailsDialog::CNetworkDetailsDialog(QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CNetworkDetailsDialog)
        {
            ui->setupUi(this);
        }

        CNetworkDetailsDialog::~CNetworkDetailsDialog()
        { }

        CFsdSetup CNetworkDetailsDialog::getFsdSetup() const
        {
            return ui->comp_NetworkDetails->getFsdSetup();
        }

        CVoiceSetup CNetworkDetailsDialog::getVoiceSetup() const
        {
            return ui->comp_NetworkDetails->getVoiceSetup();
        }

        INetwork::LoginMode CNetworkDetailsDialog::getLoginMode() const
        {
            return ui->comp_NetworkDetails->getLoginMode();
        }
    } // ns
} // ns
