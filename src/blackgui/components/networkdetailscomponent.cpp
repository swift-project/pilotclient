/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "networkdetailscomponent.h"
#include "ui_networkdetailscomponent.h"

using namespace BlackMisc::Network;
using namespace BlackMisc::Audio;
using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {
        CNetworkDetailsComponent::CNetworkDetailsComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CNetworkDetailsComponent)
        {
            ui->setupUi(this);
        }

        CNetworkDetailsComponent::~CNetworkDetailsComponent()
        { }

        INetwork::LoginMode CNetworkDetailsComponent::getLoginMode() const
        {
            return ui->frp_LoginMode->getLoginMode();
        }

        CVoiceSetup CNetworkDetailsComponent::getVoiceSetup() const
        {
            return ui->form_Voice->getValue();
        }

        CFsdSetup CNetworkDetailsComponent::getFsdSetup() const
        {
            return ui->form_FsdDetails->getValue();
        }
    } // ns
} // ns
