// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "ui_loginadvcomponent.h"
#include "loginadvcomponent.h"
#include "blackgui/components/serverlistselector.h"
#include "blackgui/editors/serverform.h"
#include "blackgui/editors/pilotform.h"
#include "blackgui/guiapplication.h"
#include "blackgui/loginmodebuttons.h"
#include "blackgui/ticklabel.h"
#include "blackgui/uppercasevalidator.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/webdataservices.h"
#include "blackcore/simulator.h"
#include "blackmisc/simulation/simulatorinternals.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/crashhandler.h"
#include "blackconfig/buildconfig.h"

#include <QDialogButtonBox>
#include <QMessageBox>
#include <QGroupBox>
#include <QIntValidator>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QTabWidget>
#include <QTimer>
#include <QToolButton>
#include <QStringBuilder>
#include <QtGlobal>
#include <QPointer>
#include <QPair>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackCore;
using namespace BlackCore::Data;
using namespace BlackCore::Context;
using namespace BlackGui;

namespace BlackGui::Components
{
    const QStringList &CLoginAdvComponent::getLogCategories()
    {
        static const QStringList cats { CLogCategories::guiComponent() };
        return cats;
    }

    CLoginAdvComponent::CLoginAdvComponent(QWidget *parent) : COverlayMessagesFrame(parent),
                                                              ui(new Ui::CLoginAdvComponent)
    {
        ui->setupUi(this);

        connect(ui->pb_Cancel, &QPushButton::clicked, this, &CLoginAdvComponent::loginCancelled, Qt::QueuedConnection);
        connect(ui->pb_Connect, &QPushButton::clicked, this, &CLoginAdvComponent::toggleNetworkConnection, Qt::QueuedConnection);
        connect(ui->comp_NetworkDetails, &CNetworkDetailsComponent::overridePilot, this, &CLoginAdvComponent::overrideCredentialsToPilot, Qt::QueuedConnection);
        connect(ui->comp_NetworkDetails, &CNetworkDetailsComponent::requestNetworkSettings, this, &CLoginAdvComponent::requestNetworkSettings, Qt::QueuedConnection);

        // overlay
        this->setOverlaySizeFactors(0.8, 0.5);
        this->setReducedInfo(true);
        this->setForceSmall(true);
        this->showKillButton(false);

        // override details
        ui->comp_NetworkDetails->setAlwaysAllowOverride(true);

        // Stored data
        this->loadRememberedUserData();

        // signals
        if (sGui && sGui->getIContextSimulator())
        {
            connect(sGui->getIContextSimulator(), &IContextSimulator::vitalityLost, this, &CLoginAdvComponent::autoLogoffDetection, Qt::QueuedConnection);
            connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this, &CLoginAdvComponent::onSimulatorStatusChanged, Qt::QueuedConnection);
            connect(sGui->getIContextSimulator(), &IContextSimulator::insufficientFrameRateDetected, this, &CLoginAdvComponent::autoLogoffFrameRate, Qt::QueuedConnection);
        }

        // inital setup, if data already available
        ui->form_Pilot->validate();
    }

    CLoginAdvComponent::~CLoginAdvComponent()
    {}

    void CLoginAdvComponent::loginCancelled()
    {
        this->closeOverlay();
        emit this->loginOrLogoffCancelled();
    }

    void CLoginAdvComponent::toggleNetworkConnection()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (!sGui->getIContextNetwork() || !sGui->getIContextAudio()) { return; }

        const bool isConnected = sGui && sGui->getIContextNetwork()->isConnected();
        const bool vatsimLogin = ui->comp_NetworkDetails->isVatsimServerSelected();

        ui->form_Pilot->setVatsimValidation(vatsimLogin);

        CServer currentServer; // used for login
        CSimulatedAircraft ownAircraft; // used own aircraft
        CStatusMessage msg;
        if (!isConnected)
        {
            const CStatusMessageList aircraftMsgs = ui->comp_OwnAircraft->validate();
            if (aircraftMsgs.isFailure())
            {
                this->showOverlayHTMLMessage(CStatusMessage(this).validationWarning(u"Invalid aircraft data, login not possible"), OverlayMessageMs);
                return;
            }

            const CStatusMessageList pilotMsgs = ui->form_Pilot->validate();
            if (pilotMsgs.isFailure())
            {
                // this->showOverlayHTMLMessage(CStatusMessage(this).validationWarning(u"Invalid pilot data, login not possible"), OverlayMessageMs);
                this->showOverlayMessagesOrHTMLMessage(pilotMsgs, false, OverlayMessageMs);
                return;
            }

            // sync values with GUI values
            const COwnAircraftComponent::CGuiAircraftValues values = ui->comp_OwnAircraft->getAircraftValuesFromGui();
            this->updateOwnAircraftCallsignAndPilotFromGuiValues();
            ui->comp_OwnAircraft->updateOwnAircaftIcaoValuesFromGuiValues();

            // Login mode
            const CLoginMode mode = ui->comp_NetworkDetails->getLoginMode();
            switch (mode.getLoginMode())
            {
            case CLoginMode::Observer: CLogMessage(this).info(u"Login in observer mode"); break;
            default: break; // INetwork::LoginNormal
            }

            // Server
            currentServer = this->getCurrentServer();
            const CUser user = this->getUserFromPilotGuiValues();
            currentServer.setUser(user);

            // FSD setup, then override
            if (ui->comp_NetworkDetails->isFsdSetupOverrideEnabled())
            {
                const CFsdSetup fsd = ui->comp_NetworkDetails->getFsdSetup();
                currentServer.setFsdSetup(fsd);
            }

            // update for own aircraft context
            sGui->getIContextOwnAircraft()->updateOwnAircraftPilot(currentServer.getUser());

            // set own aircraft from all values
            ownAircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();

            // check the copilot stuff
            CCallsign partnerCs;
            if (ui->comp_NetworkDetails->hasPartnerCallsign())
            {
                partnerCs = ui->comp_NetworkDetails->getPartnerCallsign();
                if (partnerCs == ownAircraft.getCallsign())
                {
                    this->showOverlayHTMLMessage("Your callsign and the pilot/copilot callsign must be NOT the same", OverlayMessageMs);
                    return;
                }

                const bool ok = (partnerCs.asString().startsWith(ownAircraft.getCallsignAsString(), Qt::CaseInsensitive) || ownAircraft.getCallsignAsString().startsWith(partnerCs.asString(), Qt::CaseInsensitive));
                if (!ok)
                {
                    this->showOverlayHTMLMessage("Callsign and the pilot/copilot callsign appear not to be synchronized", OverlayMessageMs);
                    return;
                }
            }

            // Login
            msg = sGui->getIContextNetwork()->connectToNetwork(currentServer, values.ownLiverySend, values.useLivery, values.ownAircraftModelStringSend, values.useModelString, partnerCs, mode);
            if (msg.isSuccess())
            {
                Q_ASSERT_X(currentServer.isValidForLogin(), Q_FUNC_INFO, "invalid server");
                sGui->setExtraWindowTitle(QStringLiteral("[%1]").arg(ownAircraft.getCallsignAsString()));
                CCrashHandler::instance()->crashAndLogInfoUserName(currentServer.getUser().getRealNameAndId());
                CCrashHandler::instance()->crashAndLogInfoFlightNetwork(currentServer.getEcosystem().toQString(true));
                CCrashHandler::instance()->crashAndLogAppendInfo(currentServer.getServerSessionId(false));
                m_networkSetup.setLastServer(currentServer);
                if (vatsimLogin) { m_networkSetup.setLastVatsimServer(currentServer); }
            }
            else
            {
                sGui->setExtraWindowTitle("");
            }
        }
        else
        {
            // disconnect from network
            sGui->setExtraWindowTitle("");
            msg = sGui->getIContextNetwork()->disconnectFromNetwork();
        }

        // log message and trigger events
        msg.addCategories(this);
        CLogMessage::preformatted(msg);
        if (msg.isSuccess())
        {
            this->setGuiLoginAsValues(ownAircraft);
            emit this->loginOrLogoffSuccessful();
        }
        else
        {
            emit this->loginOrLogoffCancelled();
        }
    }

    void CLoginAdvComponent::resetState()
    {
        ui->comp_NetworkDetails->resetState();
    }

    void CLoginAdvComponent::loadRememberedUserData()
    {
        const CServer lastServer = m_networkSetup.getLastServer();
        const CUser lastUser = lastServer.getUser();
        ui->form_Pilot->setUser(lastUser);
        ui->comp_OwnAircraft->setUser(lastUser);
    }

    void CLoginAdvComponent::overrideCredentialsToPilot(const CUser &user)
    {
        ui->form_Pilot->setUser(user, true);
    }

    void CLoginAdvComponent::onSimulatorStatusChanged(int status)
    {
        ISimulator::SimulatorStatus s = static_cast<ISimulator::SimulatorStatus>(status);
        if (!this->hasValidContexts()) { return; }
        if (sGui->getIContextNetwork()->isConnected())
        {
            if (!s.testFlag(ISimulator::Connected))
            {
                // sim NOT connected but network connected
                this->autoLogoffDetection();
            }
        }
    }

    bool CLoginAdvComponent::hasValidContexts() const
    {
        if (!sGui || !sGui->supportsContexts()) { return false; }
        if (sGui->isShuttingDown()) { return false; }
        if (!sGui->getIContextSimulator()) { return false; }
        if (!sGui->getIContextNetwork()) { return false; }
        if (!sGui->getIContextOwnAircraft()) { return false; }
        return true;
    }

    CUser CLoginAdvComponent::getUserFromPilotGuiValues() const
    {
        CUser user = ui->form_Pilot->getUser();
        user.setCallsign(ui->comp_OwnAircraft->getCallsignFromGui());
        return user;
    }

    void CLoginAdvComponent::setGuiLoginAsValues(const CSimulatedAircraft &ownAircraft)
    {
        Q_UNUSED(ownAircraft)
    }

    CServer CLoginAdvComponent::getCurrentVatsimServer() const
    {
        return ui->comp_NetworkDetails->getCurrentVatsimServer();
    }

    CServer CLoginAdvComponent::getCurrentOtherServer() const
    {
        return ui->comp_NetworkDetails->getCurrentOtherServer();
    }

    CServer CLoginAdvComponent::getCurrentServer() const
    {
        return ui->comp_NetworkDetails->getCurrentServer();
    }

    void CLoginAdvComponent::autoLogoffDetection()
    {
        if (!this->hasValidContexts()) { return; }
        if (!sGui->getIContextNetwork()->isConnected()) { return; } // nothing to logoff

        const CStatusMessage m = CStatusMessage(this, CStatusMessage::SeverityInfo, u"Auto logoff in progress (could be simulator shutdown, crash, closing simulator)");
        const int delaySecs = 30;
        this->showOverlayHTMLMessage(m, qRound(1000 * delaySecs * 0.8));

        emit this->requestLoginPage();
    }

    void CLoginAdvComponent::autoLogoffFrameRate(bool fatal)
    {
        //! \fixme code duplication with function above
        if (!this->hasValidContexts()) { return; }
        if (!sGui->getIContextNetwork()->isConnected()) { return; }

        const auto msg = fatal ? CStatusMessage(this, CStatusMessage::SeverityError, u"Sim frame rate too low to maintain constant simulation rate. Disconnecting to avoid disrupting the network.") : CStatusMessage(this, CStatusMessage::SeverityWarning, u"Sim frame rate too low to maintain constant simulation rate. Reduce graphics quality to avoid disconnection.");
        const int delaySecs = 30;
        this->showOverlayHTMLMessage(msg, qRound(1000 * delaySecs * 0.8));

        emit this->requestLoginPage();
    }

    bool CLoginAdvComponent::updateOwnAircraftCallsignAndPilotFromGuiValues()
    {
        if (!this->hasValidContexts()) { return false; }
        CSimulatedAircraft ownAircraft(sGui->getIContextOwnAircraft()->getOwnAircraft());
        const CCallsign cs = ui->comp_OwnAircraft->getCallsignFromGui();
        bool changedCallsign = false;
        if (!cs.isEmpty() && ownAircraft.getCallsign() != cs)
        {
            sGui->getIContextOwnAircraft()->updateOwnCallsign(cs);
            ownAircraft.setCallsign(cs); // also update
            changedCallsign = true;
        }
        CUser pilot = ownAircraft.getPilot();
        const CUser uiUser = ui->form_Pilot->getUser();
        pilot.setRealName(uiUser.getRealName());
        pilot.setHomeBase(uiUser.getHomeBase());
        pilot.setId(uiUser.getId());
        pilot.setCallsign(cs);
        bool changedPilot = false;
        if (ownAircraft.getPilot() != pilot)
        {
            // it can be that the callsign was changed and this results in unchanged here
            changedPilot = sGui->getIContextOwnAircraft()->updateOwnAircraftPilot(pilot);
        }
        return changedCallsign || changedPilot;
    }

    void CLoginAdvComponent::updateGui()
    {
        if (!this->hasValidContexts()) { return; }
        if (!sGui->getIContextNetwork()) { return; }
        const IContextNetwork *nwc = sGui->getIContextNetwork();
        const bool connected = nwc->isConnected();
        if (!connected) { return; }

        // in any case override if connected
        ui->comp_OwnAircraft->setOwnModelAndIcaoValues();
        ui->comp_NetworkDetails->setLoginMode(nwc->getLoginMode());
        const CSimulatedAircraft ownAircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();
        this->setGuiLoginAsValues(ownAircraft);
    }
} // namespace
