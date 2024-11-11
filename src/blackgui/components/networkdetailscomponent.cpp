// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "networkdetailscomponent.h"
#include "ui_networkdetailscomponent.h"
#include "blackgui/uppercasevalidator.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/webdataservices.h"

#include <QToolButton>

using namespace swift::misc::network;
using namespace swift::misc::audio;
using namespace swift::misc::aviation;
using namespace BlackCore;
using namespace BlackCore::Data;

namespace BlackGui::Components
{
    CNetworkDetailsComponent::CNetworkDetailsComponent(QWidget *parent) : QFrame(parent),
                                                                          ui(new Ui::CNetworkDetailsComponent)
    {
        ui->setupUi(this);

        connect(ui->comp_OtherServers, &CServerListSelector::serverChanged, this, &CNetworkDetailsComponent::onSelectedServerChanged);
        connect(ui->comp_VatsimServers, &CServerListSelector::serverChanged, this, &CNetworkDetailsComponent::onSelectedServerChanged);
        connect(ui->tw_Network, &QTabWidget::currentChanged, this, &CNetworkDetailsComponent::onServerTabWidgetChanged);
        connect(ui->pb_OtherServersGotoSettings, &QPushButton::pressed, this, &CNetworkDetailsComponent::requestNetworkSettings);
        connect(&m_networkSetup, &CNetworkSetup::setupChanged, this, &CNetworkDetailsComponent::reloadOtherServersSetup, Qt::QueuedConnection);

        // web service data
        if (sGui && sGui->getWebDataServices())
        {
            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CNetworkDetailsComponent::onWebServiceDataRead, Qt::QueuedConnection);
        }

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

    bool CNetworkDetailsComponent::isVatsimServerSelected() const
    {
        const bool vatsim = ui->tw_Network->currentWidget() == ui->tb_NetworkVatsim;
        return vatsim;
    }

    bool CNetworkDetailsComponent::isOtherServerSelected() const
    {
        return ui->tw_Network->currentWidget() == ui->tb_OtherServers;
    }

    void CNetworkDetailsComponent::setServerButtonsVisible(bool visible)
    {
        ui->wi_OtherServersButtons->setVisible(visible);
        ui->wi_VatsimButtons->setVisible(visible);
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
            ui->comp_VatsimServers->setServers(vatsimFsdServers);
            ui->comp_VatsimServers->preSelect(currentServer.getName());
        }
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
