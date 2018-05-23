/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/statusmessage.h"
#include "legalinfocomponent.h"
#include "ui_legalinfocomponent.h"

using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {
        CLegalInfoComponent::CLegalInfoComponent(QWidget *parent) :
            COverlayMessagesFrame(parent),
            ui(new Ui::CLegalInfoComponent)
        {
            ui->setupUi(this);
        }

        CLegalInfoComponent::~CLegalInfoComponent()
        { }

        bool CLegalInfoComponent::isAgreedTo() const
        {
            return ui->cb_Agree->isChecked();
        }

        bool CLegalInfoComponent::validateAgreement()
        {
            if (this->isAgreedTo()) { return true; }
            static const CStatusMessage m = CStatusMessage(this).validationError("You need to agree with the swift license");
            this->showOverlayMessage(m);
            return false;
        }

        bool CLegalInfoWizardPage::validatePage()
        {
            return m_legalInfo && m_legalInfo->validateAgreement();
        }
    } // ns
} // ns
