// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/editors/serverform.h"

#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

#include "ui_serverform.h"

#include "misc/network/user.h"

using namespace swift::misc;
using namespace swift::misc::audio;
using namespace swift::misc::network;

namespace swift::gui::editors
{
    CServerForm::CServerForm(QWidget *parent) : CForm(parent), ui(new Ui::CNetworkServerForm)
    {
        ui->setupUi(this);
        ui->le_Port->setValidator(new QIntValidator(1, 65535, this));
        this->initServerTypes();

        connect(ui->cbp_Ecosystem, &CEcosystemComboBox::currentTextChanged, this, &CServerForm::onChangedEcoSystem);
        connect(ui->cb_ServerType, &QComboBox::currentTextChanged, this, &CServerForm::onChangedServerType);
        connect(ui->tb_Unhide, &QToolButton::clicked, this, &CServerForm::tempUnhidePassword);
    }

    CServerForm::~CServerForm() {}

    void CServerForm::setServer(const CServer &server)
    {
        const CUser user = server.getUser();
        ui->le_NetworkId->setText(user.getId());
        ui->le_RealName->setText(user.getRealName());
        ui->le_Name->setText(server.getName());
        ui->cb_ServerType->setCurrentText(server.getServerTypeAsString());
        ui->cbp_Ecosystem->setCurrentEcosystem(server.getEcosystem());
        ui->le_Password->setText(user.getPassword());
        ui->le_Description->setText(server.getDescription());
        ui->le_Address->setText(server.getAddress());
        ui->le_Port->setText(QString::number(server.getPort()));
        ui->form_ServerFsd->setValue(server.getFsdSetup());
    }

    CServer CServerForm::getServer() const
    {
        const CUser user(ui->le_NetworkId->text().trimmed(), ui->le_RealName->text().trimmed().simplified(), QString(),
                         ui->le_Password->text().trimmed());
        const CFsdSetup fsdSetup(ui->form_ServerFsd->getValue());
        const CServer server(ui->le_Name->text().trimmed().simplified(),
                             ui->le_Description->text().trimmed().simplified(), ui->le_Address->text().trimmed(),
                             ui->le_Port->text().trimmed().toInt(), user, fsdSetup,
                             ui->cbp_Ecosystem->getSelectedEcosystem(), this->getServerType(), true);
        return server;
    }

    CServer::ServerType CServerForm::getServerType() const
    {
        return ui->cb_ServerType->currentData().value<CServer::ServerType>();
    }

    void CServerForm::resetToFirstTab() { ui->tw_ServerForm->setCurrentIndex(0); }

    void CServerForm::setReadOnly(bool readOnly)
    {
        ui->form_ServerFsd->setReadOnly(readOnly);

        ui->le_NetworkId->setReadOnly(readOnly);
        ui->le_RealName->setReadOnly(readOnly);
        ui->le_Name->setReadOnly(readOnly);
        ui->le_Description->setReadOnly(readOnly);
        ui->le_Address->setReadOnly(readOnly);
        ui->le_Port->setReadOnly(readOnly);
        ui->le_Password->setReadOnly(readOnly);
        ui->cb_ServerType->setEnabled(!readOnly);
        ui->cbp_Ecosystem->setEnabled(!readOnly);
        ui->tb_Unhide->setVisible(!readOnly);
        this->forceStyleSheetUpdate();
    }

    void CServerForm::showPasswordField(bool show)
    {
        if (ui->wi_Password->isVisible() == show) { return; }
        if (m_passwordNameLabel.isEmpty()) { m_passwordNameLabel = ui->lbl_IdPassword->text(); }
        ui->lbl_IdPassword->setText(show ? m_passwordNameLabel : "Id");
        ui->wi_Password->setVisible(show);
    }

    void CServerForm::initServerTypes()
    {
        // init all server type values
        int c = 0;
        ui->cb_ServerType->clear();
        for (const int type : CServer::allServerTypes())
        {
            const CServer::ServerType st = static_cast<CServer::ServerType>(type);
            ui->cb_ServerType->insertItem(c++, CServer::serverTypeToString(st), QVariant::fromValue(type));
        }
    }

    void CServerForm::onChangedServerType(const QString &text)
    {
        Q_UNUSED(text);
        const CServer::ServerType t = this->getServerType();
        const CServer dummy(t);
        const CEcosystem es = dummy.getEcosystem();
        if (es.isUnspecified()) { return; }
        if (es.isSystem(CEcosystem::NoSystem)) { return; }
        ui->cbp_Ecosystem->setCurrentEcosystem(es);
    }

    void CServerForm::onChangedEcoSystem(const QString &text)
    {
        Q_UNUSED(text);
        const CEcosystem es = ui->cbp_Ecosystem->getSelectedEcosystem();
        const CServer dummy(es);
        if (dummy.hasUnspecifiedServerType()) { return; }
        ui->cb_ServerType->setCurrentText(dummy.getServerTypeAsString());
    }

    void CServerForm::tempUnhidePassword() { CGuiUtility::tempUnhidePassword(ui->le_Password); }

    CStatusMessageList CServerForm::validate(bool nested) const
    {
        Q_UNUSED(nested);
        const CServer server = this->getServer();
        return server.validate();
    }
} // namespace swift::gui::editors
