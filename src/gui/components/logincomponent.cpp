// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "logincomponent.h"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QIntValidator>
#include <QLineEdit>
#include <QMessageBox>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QStringBuilder>
#include <QTabWidget>
#include <QTimer>
#include <QToolButton>
#include <QtGlobal>

#include "ui_logincomponent.h"

#include "config/buildconfig.h"
#include "core/context/contextaudio.h"
#include "core/context/contextnetwork.h"
#include "core/context/contextownaircraft.h"
#include "core/context/contextsimulator.h"
#include "core/data/globalsetup.h"
#include "core/simulator.h"
#include "core/webdataservices.h"
#include "gui/components/serverlistselector.h"
#include "gui/editors/pilotform.h"
#include "gui/editors/serverform.h"
#include "gui/guiapplication.h"
#include "gui/ticklabel.h"
#include "gui/uppercasevalidator.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/airporticaocode.h"
#include "misc/crashhandler.h"
#include "misc/icons.h"
#include "misc/logmessage.h"
#include "misc/network/connectionstatus.h"
#include "misc/network/entityflags.h"
#include "misc/network/server.h"
#include "misc/network/serverlist.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/simulatorinternals.h"
#include "misc/statusmessage.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::audio;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::core;
using namespace swift::core::data;
using namespace swift::core::context;
using namespace swift::gui;

namespace swift::gui::components
{
    const QStringList &CLoginComponent::getLogCategories()
    {
        static const QStringList cats { CLogCategories::guiComponent() };
        return cats;
    }

    CLoginComponent::CLoginComponent(QWidget *parent) : COverlayMessagesFrame(parent), ui(new Ui::CLoginComponent)
    {
        ui->setupUi(this);
        m_logoffCountdownTimer.setObjectName("CLoginComponent:m_logoffCountdownTimer");

        this->setLogoffCountdown();
        connect(&m_logoffCountdownTimer, &QTimer::timeout, this, &CLoginComponent::logoffCountdown);
        connect(ui->pb_Cancel, &QPushButton::clicked, this, &CLoginComponent::loginCancelled, Qt::QueuedConnection);
        connect(ui->pb_Ok, &QPushButton::clicked, this, &CLoginComponent::toggleNetworkConnection,
                Qt::QueuedConnection);
        connect(ui->comp_NetworkDetails, &CNetworkDetailsComponent::requestNetworkSettings, this,
                &CLoginComponent::requestNetworkSettings, Qt::QueuedConnection);
        connect(ui->comp_NetworkDetails, &CNetworkDetailsComponent::currentServerChanged, this,
                &CLoginComponent::onSelectedServerChanged, Qt::QueuedConnection);

        // overlay
        this->setOverlaySizeFactors(0.8, 0.5);
        this->setReducedInfo(true);
        this->setForceSmall(true);

        // Stored data
        this->loadRememberedUserData();

        // Remark: The validators affect the signals such as returnPressed, editingFinished
        // So I use no ranges in the CUpperCaseValidators, as this disables the signals for invalid values

        if (sGui && sGui->getIContextSimulator())
        {
            connect(sGui->getIContextSimulator(), &IContextSimulator::ownAircraftModelChanged, this,
                    &CLoginComponent::onSimulatorModelChanged, Qt::QueuedConnection);
            connect(sGui->getIContextSimulator(), &IContextSimulator::vitalityLost, this,
                    &CLoginComponent::autoLogoffDetection, Qt::QueuedConnection);
            connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this,
                    &CLoginComponent::onSimulatorStatusChanged, Qt::QueuedConnection);
            connect(sGui->getIContextSimulator(), &IContextSimulator::insufficientFrameRateDetected, this,
                    &CLoginComponent::autoLogoffFrameRate, Qt::QueuedConnection);
        }

        if (sGui && sGui->getIContextNetwork())
        {
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this,
                    &CLoginComponent::onNetworkStatusChanged, Qt::QueuedConnection);
        }

        // server and UI elements when in disconnect state
        ui->frp_CurrentServer->setReadOnly(true);
        ui->frp_CurrentServer->showPasswordField(false);
        ui->tb_Timeout->setIcon(m_iconPause);
        connect(ui->tb_Timeout, &QToolButton::clicked, this, &CLoginComponent::toggleTimeout);

        // inital setup, if data already available
        ui->form_Pilot->validate();

        if (sGui && sGui->getIContextSimulator())
        {
            this->onSimulatorStatusChanged(sGui->getIContextSimulator()->getSimulatorStatus());
        }

        this->updateUiConnectState();

        QPointer<CLoginComponent> myself(this);
        QTimer::singleShot(5000, this, [=] {
            if (!myself) { return; }
            this->updateGui();
        });
    }

    CLoginComponent::~CLoginComponent() = default;

    void CLoginComponent::mainInfoAreaChanged(const QWidget *currentWidget)
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (currentWidget != this && currentWidget != this->parentWidget())
        {
            // m_logoffCountdownTimer.stop();
        }
        else
        {
            ui->comp_OwnAircraft->setOwnModelAndIcaoValues();
            m_networkConnected = sGui->getIContextNetwork()->isConnected();
            this->updateUiConnectState();
            this->blinkConnectButton();
        }
    }

    void CLoginComponent::setLogoffCountdown(std::chrono::seconds timeout)
    {
        ui->pb_LogoffTimeout->setMaximum(timeout.count());
        ui->pb_LogoffTimeout->setValue(timeout.count());
        m_logoffIntervalSeconds = timeout;
    }

    void CLoginComponent::loginCancelled()
    {
        m_logoffCountdownTimer.stop();
        ui->fr_TimeoutConnected->hide();
        this->setLogoffCountdown(); // reset time
        this->closeOverlay();
        emit this->loginOrLogoffCancelled();
    }

    void CLoginComponent::toggleNetworkConnection()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (!sGui->getIContextNetwork() || !sGui->getIContextAudio()) { return; }

        m_networkConnected = sGui && sGui->getIContextNetwork()->isConnected();
        const bool vatsimLogin = this->isVatsimNetworkTabSelected();

        ui->form_Pilot->setVatsimValidation(vatsimLogin);
        this->updateUiConnectState();

        // reset time
        this->setLogoffCountdown();

        CServer currentServer; // used for login
        CSimulatedAircraft ownAircraft; // used own aircraft
        CStatusMessage msg;
        if (!m_networkConnected)
        {
            const CStatusMessageList aircraftMsgs = ui->comp_OwnAircraft->validate();
            if (aircraftMsgs.isFailure())
            {
                this->showOverlayHTMLMessage(
                    CStatusMessage(this).validationWarning(u"Invalid aircraft data, login not possible"),
                    OverlayMessageMs);
                return;
            }

            const CStatusMessageList pilotMsgs = ui->form_Pilot->validate();
            if (pilotMsgs.isFailure())
            {
                this->showOverlayHTMLMessage(
                    CStatusMessage(this).validationWarning(u"Invalid pilot data, login not possible"),
                    OverlayMessageMs);
                return;
            }

            // sync values with GUI values
            const COwnAircraftComponent::CGuiAircraftValues values = ui->comp_OwnAircraft->getAircraftValuesFromGui();
            this->updateOwnAircraftCallsignAndPilotFromGuiValues();
            ui->comp_OwnAircraft->updateOwnAircaftIcaoValuesFromGuiValues();

            // Login mode
            const CLoginMode mode = ui->comp_NetworkDetails->getLoginMode();
            if (mode.isObserver()) { CLogMessage(this).info(u"Login in observer mode"); }

            // Server
            currentServer = this->getCurrentServer();
            const CUser user = this->getUserFromPilotGuiValues();
            currentServer.setUser(user);

            ui->frp_CurrentServer->setServer(currentServer);
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
                    this->showOverlayHTMLMessage("Your callsign and the pilot/copilot callsign must be NOT the same",
                                                 OverlayMessageMs);
                    return;
                }

                const bool ok =
                    (partnerCs.asString().startsWith(ownAircraft.getCallsignAsString(), Qt::CaseInsensitive) ||
                     ownAircraft.getCallsignAsString().startsWith(partnerCs.asString(), Qt::CaseInsensitive));
                if (!ok)
                {
                    this->showOverlayHTMLMessage(
                        "Callsign and the pilot/copilot callsign appear not to be synchronized", OverlayMessageMs);
                    return;
                }
            }

            // Login
            msg = sGui->getIContextNetwork()->connectToNetwork(currentServer, values.ownLiverySend, values.useLivery,
                                                               values.ownAircraftModelStringSend, values.useModelString,
                                                               partnerCs, mode);
            if (msg.isSuccess())
            {
                Q_ASSERT_X(currentServer.isValidForLogin(), Q_FUNC_INFO, "invalid server");
                sGui->setExtraWindowTitle(QStringLiteral("[%1]").arg(ownAircraft.getCallsignAsString()));
                CCrashHandler::instance()->crashAndLogInfoUserName(currentServer.getUser().getRealNameAndId());
                CCrashHandler::instance()->crashAndLogInfoFlightNetwork(currentServer.getEcosystem().toQString(true));
                CCrashHandler::instance()->crashAndLogAppendInfo(currentServer.getServerSessionId(false));
                m_networkSetup.setLastServer(currentServer);

                const CAircraftModel ownModel = ownAircraft.getModel();
                m_lastAircraftModel.set(ownModel);
                ui->le_LoginCallsign->setText(ownAircraft.getCallsignAsString());
                ui->le_LoginHomeBase->setText(currentServer.getUser().getHomeBase().asString());
                if (vatsimLogin) { m_networkSetup.setLastVatsimServer(currentServer); }
            }
            else { sGui->setExtraWindowTitle(""); }
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
        else { emit this->loginOrLogoffCancelled(); }
    }

    void CLoginComponent::loadRememberedUserData()
    {
        const CServer lastServer = m_networkSetup.getLastServer();
        const CUser lastUser = lastServer.getUser();
        ui->form_Pilot->setUser(lastUser);
        ui->comp_OwnAircraft->setUser(lastUser);
    }

    void CLoginComponent::onSelectedServerChanged(const CServer &server)
    {
        if (!m_updatePilotOnServerChanges) { return; }
        const bool vatsim = this->isVatsimNetworkTabSelected();
        // const CUser user = vatsim ? this->getCurrentVatsimServer().getUser() : server.getUser();
        const CUser user =
            server.getServerType() != CServer::FSDServer ? this->getCurrentVatsimServer().getUser() : server.getUser();
        if ((vatsim && server.getServerType() != CServer::FSDServer) ||
            (!vatsim && server.getServerType() == CServer::FSDServer))
            ui->form_Pilot->setUser(user);
    }

    void CLoginComponent::onSimulatorStatusChanged(int status)
    {
        auto s = static_cast<ISimulator::SimulatorStatus>(status);
        if (!this->hasValidContexts()) { return; }
        m_simulatorConnected = s.testFlag(ISimulator::Connected);
        this->updateUiConnectState();
        if (sGui->getIContextNetwork()->isConnected())
        {
            if (!m_simulatorConnected)
            {
                // sim NOT connected but network connected
                this->autoLogoffDetection();
            }
        }
    }

    void CLoginComponent::onNetworkStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
    {
        Q_UNUSED(from)
        if (to != CConnectionStatus::Connected) { return; }

        m_networkConnected = true;
        this->updateUiConnectState();
        this->updateGui();
    }

    void CLoginComponent::onServerTabWidgetChanged(int index)
    {
        Q_UNUSED(index)
        if (!m_updatePilotOnServerChanges) { return; }
        const bool vatsim = this->isVatsimNetworkTabSelected();
        const CServer server = vatsim ? this->getCurrentVatsimServer() : this->getCurrentOtherServer();
        ui->form_Pilot->setUser(server.getUser());
    }

    bool CLoginComponent::hasValidContexts() const
    {
        if (!sGui || !sGui->supportsContexts()) { return false; }
        if (sGui->isShuttingDown()) { return false; }
        if (!sGui->getIContextSimulator()) { return false; }
        if (!sGui->getIContextNetwork()) { return false; }
        if (!sGui->getIContextOwnAircraft()) { return false; }
        return true;
    }

    CUser CLoginComponent::getUserFromPilotGuiValues() const
    {
        CUser user = ui->form_Pilot->getUser();
        user.setCallsign(ui->comp_OwnAircraft->getCallsignFromGui());
        return user;
    }

    CServer CLoginComponent::getCurrentVatsimServer() const
    {
        CServer server = ui->comp_NetworkDetails->getCurrentVatsimServer();
        if (!server.getUser().hasValidVatsimId())
        {
            // normally VATSIM server have no valid user associated
            const CUser user = m_networkSetup.getLastVatsimServer().getUser();
            server.setUser(user);
        }
        return server;
    }

    CServer CLoginComponent::getCurrentOtherServer() const { return ui->comp_NetworkDetails->getCurrentOtherServer(); }

    CServer CLoginComponent::getCurrentServer() const
    {
        return this->isVatsimNetworkTabSelected() ? this->getCurrentVatsimServer() : this->getCurrentOtherServer();
    }

    void CLoginComponent::startLogoffTimerCountdown()
    {
        ui->pb_LogoffTimeout->setValue(m_logoffIntervalSeconds.count());
        m_logoffCountdownTimer.setInterval(1000);
        m_logoffCountdownTimer.start();
        ui->fr_TimeoutConnected->show();
    }

    void CLoginComponent::setGuiLoginAsValues(const CSimulatedAircraft &ownAircraft)
    {
        const QString ac(
            ownAircraft.getAircraftIcaoCodeDesignator() %
            (ownAircraft.hasAirlineDesignator() ? (u' ' % ownAircraft.getAirlineIcaoCodeDesignator()) : QString()) %
            (ownAircraft.hasModelString() ? (u' ' % ownAircraft.getModelString()) : QString()));
        const QString cs = ownAircraft.getCallsignAsString();
        ui->le_LoginSince->setText(QDateTime::currentDateTimeUtc().toString());
        ui->le_LoginAsAircaft->setText(ac);
        ui->le_LoginAsAircaft->home(false);
        ui->le_LoginCallsign->setText(cs);
    }

    void CLoginComponent::logoffCountdown()
    {
        int v = ui->pb_LogoffTimeout->value();
        v -= 1;
        if (v < 0) { v = 0; }
        ui->pb_LogoffTimeout->setValue(v);
        if (v <= 0)
        {
            m_logoffCountdownTimer.stop();
            this->toggleNetworkConnection();
        }
    }

    void CLoginComponent::autoLogoffDetection()
    {
        using namespace std::chrono_literals;
        if (!this->hasValidContexts()) { return; }
        if (!sGui->getIContextNetwork()->isConnected()) { return; } // nothing to logoff

        const CStatusMessage m =
            CStatusMessage(this, CStatusMessage::SeverityInfo,
                           u"Auto logoff in progress (could be simulator shutdown, crash, closing simulator)");
        const auto delay = 20s;
        this->showOverlayHTMLMessage(m, 800 * delay);
        this->setLogoffCountdown(delay);
        this->startLogoffTimerCountdown();

        emit this->requestLoginPage();
    }

    void CLoginComponent::autoLogoffFrameRate(bool fatal)
    {
        using namespace std::chrono_literals;
        //! \fixme code duplication with function above
        if (!this->hasValidContexts()) { return; }
        if (!sGui->getIContextNetwork()->isConnected()) { return; }

        const auto msg = fatal ? CStatusMessage(this, CStatusMessage::SeverityError,
                                                u"Sim frame rate too low to maintain constant simulation rate. "
                                                u"Disconnecting to avoid disrupting the network.") :
                                 CStatusMessage(this, CStatusMessage::SeverityWarning,
                                                u"Sim frame rate too low to maintain constant simulation rate. Reduce "
                                                u"graphics quality to avoid disconnection.");
        const auto delay = 20s;
        this->showOverlayHTMLMessage(msg, 800 * delay);
        if (fatal)
        {
            this->setLogoffCountdown(delay);
            this->startLogoffTimerCountdown();
        }

        emit this->requestLoginPage();
    }

    void CLoginComponent::onSimulatorModelChanged(const CAircraftModel &model)
    {
        if (!sGui || !sGui->getIContextNetwork() || sApp->isShuttingDown()) { return; }
        const bool isNetworkConnected = sGui && sGui->getIContextNetwork()->isConnected();
        if (isNetworkConnected) { return; }

        // update with latest DB data
        CAircraftModel reverseModel(model);
        if (sGui->hasWebDataServices())
        {
            reverseModel = sGui->getWebDataServices()->getModelForModelString(model.getModelString());
            if (!reverseModel.isLoadedFromDb()) { reverseModel = model; } // reset if not found
        }

        const QString modelStr(reverseModel.hasModelString() ? reverseModel.getModelString() : "<unknown>");
        if (!reverseModel.hasModelString())
        {
            CLogMessage(this).validationInfo(u"Invalid lookup for '%1' successful: %2")
                << modelStr << reverseModel.toQString();
            CLogMessage(this).validationInfo(u"Hint: Are you using the emulated driver? Set a model if so!");
            return;
        }
        ui->comp_OwnAircraft->setOwnModelAndIcaoValues(reverseModel);

        // check state of own aircraft
        this->updateOwnAircraftCallsignAndPilotFromGuiValues();

        // let others know data changed
        m_changedLoginDataDigestSignal.inputSignal();
    }

    void CLoginComponent::toggleTimeout()
    {
        if (m_logoffCountdownTimer.isActive())
        {
            m_logoffCountdownTimer.stop();
            ui->tb_Timeout->setIcon(m_iconPlay);
        }
        else
        {
            m_logoffCountdownTimer.start();
            ui->tb_Timeout->setIcon(m_iconPause);
        }
    }

    void CLoginComponent::updateUiConnectState()
    {
        ui->fr_LoginDisconnected->setVisible(!m_networkConnected);
        ui->fr_LogoffConfirmationConnected->setVisible(m_networkConnected);

        const QString s = m_networkConnected ? QStringLiteral("disconnect") : QStringLiteral("connect");
        ui->pb_Ok->setText(s);

        if (m_networkConnected) { ui->pb_Ok->setDisabled(false); }
        else { ui->pb_Ok->setDisabled(!m_simulatorConnected); }
    }

    void CLoginComponent::blinkConnectButton()
    {
        if (!ui->pb_Ok->isEnabled()) { return; }
        ui->pb_Ok->setProperty("blinkOn", true);
        static constexpr int blinkLength = 100;
        static constexpr int blinkTimes = 10;

        auto timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this, timer, count = std::make_shared<int>(0)] {
            if (++*count <= blinkTimes) { ui->pb_Ok->setProperty("blinkOn", !ui->pb_Ok->property("blinkOn").toBool()); }
            else
            {
                ui->pb_Ok->setProperty("blinkOn", false);
                timer->stop();
                timer->deleteLater();
            }
            ui->pb_Ok->style()->unpolish(ui->pb_Ok);
            ui->pb_Ok->style()->polish(ui->pb_Ok);
        });
        timer->setObjectName("blinker");
        timer->start(blinkLength);
    }

    bool CLoginComponent::isVatsimNetworkTabSelected() const
    {
        return ui->comp_NetworkDetails->isVatsimServerSelected();
    }

    CAircraftModel CLoginComponent::getPrefillModel() const
    {
        const CAircraftModel model = m_lastAircraftModel.get();
        if (model.hasAircraftDesignator()) { return model; }
        return IContextOwnAircraft::getDefaultOwnAircraftModel();
    }

    bool CLoginComponent::updateOwnAircraftCallsignAndPilotFromGuiValues()
    {
        if (!this->hasValidContexts()) { return false; }
        CSimulatedAircraft ownAircraft(sGui->getIContextOwnAircraft()->getOwnAircraft());
        const CCallsign cs = ui->comp_OwnAircraft->getCallsignFromGui();
        bool changedCallsign = false;
        if (!cs.isEmpty() && ownAircraft.getCallsignAsString() != cs)
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
        pilot.setCallsign(CCallsign(cs));
        bool changedPilot = false;
        if (ownAircraft.getPilot() != pilot)
        {
            // it can be that the callsign was changed and this results in unchanged here
            changedPilot = sGui->getIContextOwnAircraft()->updateOwnAircraftPilot(pilot);
        }
        return changedCallsign || changedPilot;
    }

    void CLoginComponent::updateGui()
    {
        if (!m_networkConnected) { return; }
        if (!this->hasValidContexts()) { return; }
        if (!sGui->getIContextNetwork()) { return; }
        const IContextNetwork *nwc = sGui->getIContextNetwork();
        const CSimulatedAircraft ownAircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();
        this->setGuiLoginAsValues(ownAircraft);
        this->updateUiConnectState();
        ui->comp_OwnAircraft->setOwnModelAndIcaoValues();
        const CServer server = nwc->getConnectedServer();
        ui->le_LoginHomeBase->setText(server.getUser().getHomeBase().asString());
        ui->frp_CurrentServer->setServer(server);
        ui->comp_NetworkDetails->setLoginMode(nwc->getLoginMode());
    }
} // namespace swift::gui::components
