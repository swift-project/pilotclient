// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "networkdetailscomponent.h"
#include "ui_networkdetailscomponent.h"
#include "blackgui/uppercasevalidator.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/webdataservices.h"

#include <QToolButton>

using namespace BlackMisc::Network;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;
using namespace BlackCore;
using namespace BlackCore::Data;

namespace BlackGui::Components
{
    CNetworkDetailsComponent::CNetworkDetailsComponent(QWidget *parent) : QFrame(parent),
                                                                          ui(new Ui::CNetworkDetailsComponent)
    {
        ui->setupUi(this);
        ui->tw_Details->setCurrentIndex(0);
        ui->sw_NetworkServerDetails->setCurrentIndex(PageServer);

        connect(ui->comp_OtherServers, &CServerListSelector::serverChanged, this, &CNetworkDetailsComponent::onSelectedServerChanged);
        connect(ui->comp_VatsimServers, &CServerListSelector::serverChanged, this, &CNetworkDetailsComponent::onSelectedServerChanged);
        connect(ui->tw_Network, &QTabWidget::currentChanged, this, &CNetworkDetailsComponent::onServerTabWidgetChanged);
        connect(ui->tw_Details, &QTabWidget::currentChanged, this, &CNetworkDetailsComponent::onDetailsTabChanged);
        connect(ui->pb_OtherServersGotoSettings, &QPushButton::pressed, this, &CNetworkDetailsComponent::requestNetworkSettings);
        connect(ui->pb_OverrideCredentialsVatsim, &QPushButton::clicked, this, &CNetworkDetailsComponent::onOverrideCredentialsToPilot);
        connect(ui->pb_OverrideCredentialsOtherServers, &QPushButton::clicked, this, &CNetworkDetailsComponent::onOverrideCredentialsToPilot);
        connect(ui->pb_DetailsVatsim, &QPushButton::clicked, this, &CNetworkDetailsComponent::onChangePage);
        connect(ui->pb_DetailsOtherServers, &QPushButton::clicked, this, &CNetworkDetailsComponent::onChangePage);
        connect(ui->pb_BackToServer, &QPushButton::clicked, this, &CNetworkDetailsComponent::onChangePage);
        connect(&m_networkSetup, &CNetworkSetup::setupChanged, this, &CNetworkDetailsComponent::reloadOtherServersSetup, Qt::QueuedConnection);

        // web service data
        if (sGui && sGui->getWebDataServices())
        {
            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CNetworkDetailsComponent::onWebServiceDataRead, Qt::QueuedConnection);
        }

        ui->form_FsdDetails->showEnableInfo(true);
        ui->form_FsdDetails->setFsdSetupEnabled(false);
        ui->form_FsdDetails->setReadOnly(false);

        constexpr int MaxLength = 10;
        constexpr int MinLength = 0;
        CUpperCaseValidator *ucv = new CUpperCaseValidator(MinLength, MaxLength, ui->le_PartnerCallsign);
        ucv->setAllowedCharacters09AZ();
        ui->le_PartnerCallsign->setMaxLength(MaxLength);
        ui->le_PartnerCallsign->setValidator(ucv);

        const int tab = m_networkSetup.wasLastUsedWithOtherServer() ? LoginOthers : LoginVATSIM;
        ui->tw_Network->setCurrentIndex(tab);

        this->reloadOtherServersSetup();
        this->onWebServiceDataRead(CEntityFlags::VatsimDataFile, CEntityFlags::ReadFinished, -1, {});
    }

    CNetworkDetailsComponent::~CNetworkDetailsComponent()
    {}

    CLoginMode CNetworkDetailsComponent::getLoginMode() const
    {
        return ui->frp_LoginMode->getLoginMode();
    }

    void CNetworkDetailsComponent::setLoginMode(CLoginMode mode)
    {
        ui->frp_LoginMode->setLoginMode(mode);
    }

    void CNetworkDetailsComponent::resetState()
    {
        ui->sw_NetworkServerDetails->setCurrentIndex(PageServer);
        this->setBackTabName();
    }

    bool CNetworkDetailsComponent::isVatsimServerSelected() const
    {
        const bool vatsim = ui->tw_Network->currentWidget() == ui->tb_NetworkVatsim;
        return vatsim;
    }

    bool CNetworkDetailsComponent::isOtherServerSelected() const
    {
        return ui->tw_Details->currentWidget() == ui->tb_OtherServers;
    }

    CFsdSetup CNetworkDetailsComponent::getFsdSetup() const
    {
        return ui->form_FsdDetails->getValue();
    }

    void CNetworkDetailsComponent::setAlwaysAllowOverride(bool allow)
    {
        ui->form_FsdDetails->setAlwaysAllowOverride(allow);
    }

    bool CNetworkDetailsComponent::isFsdSetupOverrideEnabled() const
    {
        return ui->form_FsdDetails->isFsdSetupEnabled();
    }

    void CNetworkDetailsComponent::setServerButtonsVisible(bool visible)
    {
        ui->wi_OtherServersButtons->setVisible(visible);
        ui->wi_VatsimButtons->setVisible(visible);
    }

    void CNetworkDetailsComponent::onDetailsTabChanged(int index)
    {
        if (index == DetailsBack)
        {
            ui->sw_NetworkServerDetails->setCurrentIndex(PageServer);
            return;
        }

        Q_UNUSED(index)
        const CServer server = this->getCurrentServer();

        // only override if not yet enabled
        if (!ui->form_FsdDetails->isFsdSetupEnabled()) { ui->form_FsdDetails->setValue(server.getFsdSetup()); }
    }

    void CNetworkDetailsComponent::onOverrideCredentialsToPilot()
    {
        CServer server;
        const QObject *s = QObject::sender();
        if (s == ui->pb_OverrideCredentialsOtherServers)
        {
            server = this->getCurrentOtherServer();
        }
        else if (s == ui->pb_OverrideCredentialsVatsim)
        {
            // the VATSIM server selected has no valid user credentials
            server = m_networkSetup.getLastVatsimServer();
        }
        else { return; }
        emit this->overridePilot(server.getUser());
    }

    void CNetworkDetailsComponent::onServerTabWidgetChanged(int index)
    {
        Q_UNUSED(index)
        if (!m_updatePilotOnServerChanges) { return; }
        const bool vatsim = this->isVatsimServerSelected();
        const CServer server = vatsim ? this->getCurrentVatsimServer() : this->getCurrentOtherServer();
        emit this->overridePilot(server.getUser());
    }

    void CNetworkDetailsComponent::onSelectedServerChanged(const CServer &server)
    {
        if (!m_updatePilotOnServerChanges) { return; }
        emit this->overridePilot(server.getUser());
    }

    void CNetworkDetailsComponent::onWebServiceDataRead(CEntityFlags::Entity entity, CEntityFlags::ReadState state, int number, const QUrl &url)
    {
        if (!CEntityFlags::isFinishedReadState(state)) { return; }
        Q_UNUSED(number)
        Q_UNUSED(url)

        if (entity == CEntityFlags::VatsimDataFile)
        {
            CServerList vatsimFsdServers = sGui->getIContextNetwork()->getVatsimFsdServers();
            if (vatsimFsdServers.isEmpty()) { return; }
            vatsimFsdServers.sortBy(&CServer::getName);
            const CServer currentServer = m_networkSetup.getLastVatsimServer();
            ui->comp_VatsimServers->setServers(vatsimFsdServers, true);
            ui->comp_VatsimServers->preSelect(currentServer.getName());
        }
    }

    void CNetworkDetailsComponent::onChangePage()
    {
        const QObject *s = QObject::sender();
        if (s == ui->pb_DetailsVatsim || s == ui->pb_DetailsOtherServers)
        {
            ui->sw_NetworkServerDetails->setCurrentIndex(PageDetails);
            ui->tw_Details->setCurrentIndex(DetailsServer);
        }
        else
        {
            ui->sw_NetworkServerDetails->setCurrentIndex(PageServer);
        }
        this->setBackTabName();
    }

    void CNetworkDetailsComponent::setBackTabName()
    {
        const QString name = this->getCurrentServer().getName();
        const int index = ui->tw_Details->indexOf(ui->tb_BackToServer);
        ui->tw_Details->setTabText(index, QStringLiteral("Back to server '%1'").arg(name));
    }

    CServer CNetworkDetailsComponent::getCurrentVatsimServer() const
    {
        CServer server = ui->comp_VatsimServers->currentServer();
        if (!server.getUser().hasValidVatsimId())
        {
            // normally VATSIM server have no valid user associated
            const CUser user = m_networkSetup.getLastVatsimServer().getUser();
            server.setUser(user);
        }
        return server;
    }

    CServer CNetworkDetailsComponent::getCurrentOtherServer() const
    {
        return ui->comp_OtherServers->currentServer();
    }

    CServer CNetworkDetailsComponent::getCurrentServer() const
    {
        return this->isVatsimServerSelected() ? this->getCurrentVatsimServer() : this->getCurrentOtherServer();
    }

    bool CNetworkDetailsComponent::hasPartnerCallsign() const
    {
        return !ui->le_PartnerCallsign->text().isEmpty();
    }

    CCallsign CNetworkDetailsComponent::getPartnerCallsign() const
    {
        if (ui->le_PartnerCallsign->text().isEmpty()) { return {}; }
        return CCallsign(ui->le_PartnerCallsign->text(), CCallsign::Aircraft);
    }

    void CNetworkDetailsComponent::reloadOtherServersSetup()
    {
        const CServerList otherServers(m_networkSetup.getOtherServersPlusPredefinedServers());
        ui->comp_OtherServers->setServers(otherServers);
    }

} // ns
