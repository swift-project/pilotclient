/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/editors/serverform.h"
#include "blackmisc/network/user.h"
#include "ui_serverform.h"

#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>

using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Editors
    {
        CServerForm::CServerForm(QWidget *parent) :
            CForm(parent),
            ui(new Ui::CNetworkServerForm)
        {
            ui->setupUi(this);
            ui->le_Port->setValidator(new QIntValidator(1, 65535, this));
        }

        CServerForm::~CServerForm()
        { }

        void CServerForm::setServer(const CServer &server)
        {
            CUser user = server.getUser();
            ui->le_NetworkId->setText(user.getId());
            ui->le_RealName->setText(user.getRealName());
            ui->le_Name->setText(server.getName());
            ui->le_Password->setText(user.getPassword());
            ui->le_Description->setText(server.getDescription());
            ui->le_Address->setText(server.getAddress());
            ui->le_Port->setText(QString::number(server.getPort()));
            ui->form_ServerFsd->setValue(server.getFsdSetup());
        }

        BlackMisc::Network::CServer CServerForm::getServer() const
        {
            CUser user(
                ui->le_NetworkId->text().trimmed(),
                ui->le_RealName->text().trimmed().simplified(),
                "",
                ui->le_Password->text().trimmed()
            );
            CServer server(
                ui->le_Name->text().trimmed().simplified(),
                ui->le_Description->text().trimmed().simplified(),
                ui->le_Address->text().trimmed(),
                ui->le_Port->text().trimmed().toInt(),
                user
            );
            CFsdSetup setup(ui->form_ServerFsd->getValue());
            server.setFsdSetup(setup);
            return server;
        }

        void CServerForm::setReadOnly(bool readOnly)
        {
            ui->le_NetworkId->setReadOnly(readOnly);
            ui->le_RealName->setReadOnly(readOnly);
            ui->le_Name->setReadOnly(readOnly);
            ui->le_Description->setReadOnly(readOnly);
            ui->le_Address->setReadOnly(readOnly);
            ui->le_Port->setReadOnly(readOnly);
            ui->le_Password->setReadOnly(readOnly);
            ui->form_ServerFsd->setReadOnly(readOnly);
        }

        void CServerForm::showPasswordField(bool show)
        {
            if (ui->le_Password->isVisible() == show) { return; }
            if (m_passwordNameLabel.isEmpty()) { m_passwordNameLabel = ui->lbl_IdPassword->text(); }
            if (show)
            {
                ui->lbl_IdPassword->setText(m_passwordNameLabel);
            }
            else
            {
                ui->lbl_IdPassword->setText("Id");
            }
            ui->le_Password->setVisible(show);
        }

        BlackMisc::CStatusMessageList CServerForm::validate(bool nested) const
        {
            Q_UNUSED(nested);
            const CServer server = getServer();
            return server.validate();
        }
    } // ns
} // ns
