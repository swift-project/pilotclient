/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "serverform.h"
#include "ui_serverform.h"

using namespace BlackMisc::Network;

namespace BlackGui
{

    CServerForm::CServerForm(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CNetworkServerForm)
    {
        ui->setupUi(this);
        this->ui->le_Port->setValidator(new QIntValidator(1, 65535, this));
    }

    CServerForm::~CServerForm()
    { }

    void CServerForm::setServer(const CServer &server)
    {
        CUser user = server.getUser();
        this->ui->le_NetworkId->setText(user.getId());
        this->ui->le_RealName->setText(user.getRealName());
        this->ui->le_Name->setText(server.getName());
        this->ui->le_Password->setText(user.getPassword());
        this->ui->le_Description->setText(server.getDescription());
        this->ui->le_Address->setText(server.getAddress());
        this->ui->le_Port->setText(QString::number(server.getPort()));
    }

    BlackMisc::Network::CServer CServerForm::getServer() const
    {
        CUser user(
            this->ui->le_NetworkId->text().trimmed(),
            this->ui->le_RealName->text().trimmed().simplified(),
            "",
            this->ui->le_Password->text().trimmed()
        );
        CServer server(
            this->ui->le_Name->text().trimmed().simplified(),
            this->ui->le_Description->text().trimmed().simplified(),
            this->ui->le_Address->text().trimmed(),
            this->ui->le_Port->text().trimmed().toInt(),
            user
        );
        return server;
    }

    void CServerForm::setReadOnly(bool readOnly)
    {
        this->ui->le_NetworkId->setReadOnly(readOnly);
        this->ui->le_RealName->setReadOnly(readOnly);
        this->ui->le_Name->setReadOnly(readOnly);
        this->ui->le_Description->setReadOnly(readOnly);
        this->ui->le_Address->setReadOnly(readOnly);
        this->ui->le_Port->setReadOnly(readOnly);
        this->ui->le_Password->setReadOnly(readOnly);
    }

    void CServerForm::showPasswordField(bool show)
    {
        if (this->ui->le_Password->isVisible() == show) { return; }
        if (m_passwordNameLabel.isEmpty()) { m_passwordNameLabel = ui->lbl_IdPassword->text(); }
        if (show)
        {
            ui->lbl_IdPassword->setText(m_passwordNameLabel);
        }
        else
        {
            ui->lbl_IdPassword->setText("Id");
        }
        this->ui->le_Password->setVisible(show);
    }

    BlackMisc::CStatusMessageList CServerForm::validate() const
    {
        CServer server = getServer();
        return server.validate();
    }

} // namespace
