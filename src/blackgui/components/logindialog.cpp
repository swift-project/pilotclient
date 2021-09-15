/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "logindialog.h"
#include "ui_logindialog.h"

#include "blackgui/guiapplication.h"
#include "blackcore/context/contextnetwork.h"

using namespace BlackMisc::Network;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui::Components
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

        connect(ui->comp_LoginOverviewComponent, &CLoginOverviewComponent::closeOverview,           this, &CLoginDialog::close);
        connect(ui->comp_LoginOverviewComponent, &CLoginOverviewComponent::loginOrLogoffSuccessful, this, &CLoginDialog::onLoginOrLogoffCancelled);

        if (sGui && sGui->getIContextNetwork())
        {
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CLoginDialog::onNetworkStatusChanged, Qt::QueuedConnection);
        }
    }

    CLoginDialog::~CLoginDialog()
    { }

    void CLoginDialog::setAutoLogoff(bool logoff)
    {
        ui->comp_LoginComponent->setAutoLogoff(logoff);
        ui->comp_LoginOverviewComponent->setAutoLogoff(logoff);
    }

    void CLoginDialog::show()
    {
        this->init();
        QDialog::show();
    }

    void CLoginDialog::init()
    {
        bool connected = false;
        if (sGui && sGui->getIContextNetwork())
        {
            connected = sGui->getIContextNetwork()->isConnected();
            if (connected)
            {
                ui->comp_LoginOverviewComponent->showCurrentValues();
            }
            else
            {
                ui->comp_LoginComponent->resetState();
            }
        }

        ui->comp_LoginComponent->setVisible(!connected);
        ui->comp_LoginOverviewComponent->setVisible(connected);
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

    void CLoginDialog::onNetworkStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
    {
        Q_UNUSED(from);
        if (to.isDisconnected() || to.isConnected())
        {
            this->init();
        }
    }
}// ns
