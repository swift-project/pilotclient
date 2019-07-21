/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "logindialog.h"
#include "ui_logindialog.h"

namespace BlackGui
{
    namespace Components
    {
        CLoginDialog::CLoginDialog(QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CLoginDialog)
        {
            ui->setupUi(this);
            this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

            connect(ui->comp_LoginComponent, &CLoginAdvComponent::loginOrLogoffSuccessful, this, &CLoginDialog::onLoginOrLogoffSuccessful);
            connect(ui->comp_LoginComponent, &CLoginAdvComponent::loginOrLogoffCancelled,  this, &CLoginDialog::onLoginOrLogoffCancelled);
            connect(ui->comp_LoginComponent, &CLoginAdvComponent::requestNetworkSettings,  this, &CLoginDialog::onRequestNetworkSettings);
        }

        CLoginDialog::~CLoginDialog()
        { }

        void CLoginDialog::setAutoLogoff(bool logoff)
        {
            ui->comp_LoginComponent->setAutoLogoff(logoff);
        }

        void CLoginDialog::onLoginOrLogoffCancelled()
        {
            this->reject();
        }

        void CLoginDialog::onLoginOrLogoffSuccessful()
        {
            this->accept();
        }

        void CLoginDialog::onRequestNetworkSettings()
        {
            emit this->requestNetworkSettings();
            this->close();
        }
    } // ns
}// ns
