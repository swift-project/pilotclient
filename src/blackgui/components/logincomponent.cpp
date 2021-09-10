/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "ui_logincomponent.h"
#include "logincomponent.h"
#include "serverlistselector.h"
#include "dbquickmappingwizard.h"
#include "blackgui/editors/serverform.h"
#include "blackgui/editors/pilotform.h"
#include "blackgui/guiapplication.h"
#include "blackgui/loginmodebuttons.h"
#include "blackgui/ticklabel.h"
#include "blackgui/uppercasevalidator.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/webdataservices.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/simulator.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/connectionstatus.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/simulation/simulatorinternals.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"
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

namespace BlackGui
{
    namespace Components
    {
        const QStringList &CLoginComponent::getLogCategories()
        {
            static const QStringList cats { CLogCategories::guiComponent() };
            return cats;
        }

        CLoginComponent::CLoginComponent(QWidget *parent) :
            COverlayMessagesFrame(parent),
            ui(new Ui::CLoginComponent)
        {
            ui->setupUi(this);
            ui->tw_Details->setCurrentWidget(ui->tb_LoginMode);
            m_logoffCountdownTimer.setObjectName("CLoginComponent:m_logoffCountdownTimer");

            ui->tw_Network->setCurrentIndex(0);
            ui->selector_AircraftIcao->displayWithIcaoDescription(false);
            ui->selector_AirlineIcao->displayWithIcaoDescription(false);
            ui->selector_AircraftIcao->displayMode(CDbAircraftIcaoSelectorComponent::DisplayIcaoAndId);
            ui->selector_AirlineIcao->displayMode(CDbAirlineIcaoSelectorComponent::DisplayVDesignatorAndId);

            this->setLogoffCountdown();
            connect(&m_logoffCountdownTimer, &QTimer::timeout, this, &CLoginComponent::logoffCountdown);
            connect(ui->comp_OtherServers,   &CServerListSelector::serverChanged, this, &CLoginComponent::onSelectedServerChanged);
            connect(ui->comp_VatsimServers,  &CServerListSelector::serverChanged, this, &CLoginComponent::onSelectedServerChanged);
            connect(ui->pb_RefreshOtherServers, &QToolButton::clicked, this, &CLoginComponent::reloadOtherServersSetup);
            connect(ui->tw_Network, &QTabWidget::currentChanged,       this, &CLoginComponent::onServerTabWidgetChanged);
            connect(ui->pb_Cancel,  &QPushButton::clicked, this, &CLoginComponent::loginCancelled,          Qt::QueuedConnection);
            connect(ui->pb_Ok,      &QPushButton::clicked, this, &CLoginComponent::toggleNetworkConnection, Qt::QueuedConnection);
            connect(ui->pb_OtherServersGotoSettings, &QPushButton::pressed, this, &CLoginComponent::requestNetworkSettings);
            connect(ui->pb_MappingWizard, &QToolButton::clicked,   this, &CLoginComponent::mappingWizard,           Qt::QueuedConnection);
            connect(&m_networkSetup, &CNetworkSetup::setupChanged, this, &CLoginComponent::reloadOtherServersSetup, Qt::QueuedConnection);

            ui->form_FsdDetails->showEnableInfo(true);
            ui->form_FsdDetails->setFsdSetupEnabled(false);
            ui->form_FsdDetails->setAlwaysAllowOverride(true);

            ui->form_Voice->showEnableInfo(true);
            ui->form_Voice->setVoiceSetupEnabled(false);
            ui->form_Voice->setAlwaysAllowOverride(true);

            ui->lblp_AircraftCombinedType->setToolTips("ok", "wrong");
            ui->lblp_AirlineIcao->setToolTips("ok", "wrong");
            ui->lblp_AircraftIcao->setToolTips("ok", "wrong");
            ui->lblp_Callsign->setToolTips("ok", "wrong");

            // overlay
            this->setOverlaySizeFactors(0.8, 0.5);
            this->setReducedInfo(true);
            this->setForceSmall(true);
            this->showKillButton(false);

            // Stored data
            this->loadRememberedUserData();

            // Remark: The validators affect the signals such as returnPressed, editingFinished
            // So I use no ranges in the CUpperCaseValidators, as this disables the signals for invalid values

            // own aircraft
            constexpr int MaxLength = 10;
            constexpr int MinLength = 0;
            CUpperCaseValidator *ucv = new CUpperCaseValidator(MinLength, MaxLength, ui->le_Callsign);
            // ucv->setAllowedCharacters09AZ();
            ui->le_Callsign->setMaxLength(MaxLength);
            ui->le_Callsign->setValidator(ucv);
            connect(ui->le_Callsign, &QLineEdit::editingFinished, this, &CLoginComponent::validateAircraftValues);

            ui->le_AircraftCombinedType->setMaxLength(3);
            ui->le_AircraftCombinedType->setValidator(new CUpperCaseValidator(this));
            connect(ui->le_AircraftCombinedType, &QLineEdit::editingFinished, this, &CLoginComponent::validateAircraftValues);
            connect(ui->selector_AircraftIcao,   &CDbAircraftIcaoSelectorComponent::changedAircraftIcao, this, &CLoginComponent::onChangedAircraftIcao, Qt::QueuedConnection);
            connect(ui->selector_AirlineIcao,    &CDbAirlineIcaoSelectorComponent::changedAirlineIcao, this, &CLoginComponent::onChangedAirlineIcao, Qt::QueuedConnection);
            connect(ui->pb_SimulatorLookup,      &QToolButton::clicked, this, &CLoginComponent::lookupOwnAircraftModel);
            connect(ui->tw_Details,              &QTabWidget::currentChanged, this, &CLoginComponent::onDetailsTabChanged);

            if (sGui && sGui->getIContextSimulator())
            {
                connect(sGui->getIContextSimulator(), &IContextSimulator::ownAircraftModelChanged, this, &CLoginComponent::onSimulatorModelChanged, Qt::QueuedConnection);
                connect(sGui->getIContextSimulator(), &IContextSimulator::vitalityLost, this, &CLoginComponent::autoLogoffDetection, Qt::QueuedConnection);
                connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this, &CLoginComponent::onSimulatorStatusChanged, Qt::QueuedConnection);
                connect(sGui->getIContextSimulator(), &IContextSimulator::insufficientFrameRateDetected, this, &CLoginComponent::autoLogoffFrameRate, Qt::QueuedConnection);
            }

            if (sGui && sGui->getIContextNetwork())
            {
                connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CLoginComponent::onNetworkStatusChanged, Qt::QueuedConnection);
            }

            // server and UI elements when in disconnect state
            ui->frp_CurrentServer->setReadOnly(true);
            ui->frp_CurrentServer->showPasswordField(false);
            ui->tb_Timeout->setIcon(m_iconPause);
            connect(ui->tb_Timeout, &QToolButton::clicked, this, &CLoginComponent::toggleTimeout);

            // web service data
            if (sGui && sGui->getWebDataServices())
            {
                connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CLoginComponent::onWebServiceDataRead, Qt::QueuedConnection);
            }

            // inital setup, if data already available
            this->validateAircraftValues();
            ui->form_Pilot->validate();
            ui->cb_AutoLogoff->setChecked(m_networkSetup.useAutoLogoff());
            this->onWebServiceDataRead(CEntityFlags::VatsimDataFile, CEntityFlags::ReadFinished, -1, {});
            this->reloadOtherServersSetup();

            connect(ui->pb_OverrideCredentialsVatsim, &QPushButton::clicked, this, &CLoginComponent::overrideCredentialsToPilot);
            connect(ui->pb_OverrideCredentialsOtherServers, &QPushButton::clicked, this, &CLoginComponent::overrideCredentialsToPilot);

            this->setUiLoginState(false);

            const int tab = m_networkSetup.wasLastUsedWithOtherServer() ? LoginOthers : LoginVATSIM;
            ui->tw_Network->setCurrentIndex(tab);

            QPointer<CLoginComponent> myself(this);
            QTimer::singleShot(5000, this, [ = ]
            {
                if (!myself) { return; }
                this->updateGui();
            });
        }

        CLoginComponent::~CLoginComponent()
        { }

        void CLoginComponent::mainInfoAreaChanged(const QWidget *currentWidget)
        {
            if (!sGui || sGui->isShuttingDown()) { return; }
            if (currentWidget != this && currentWidget != this->parentWidget())
            {
                //m_logoffCountdownTimer.stop();
            }
            else
            {
                this->setOwnModelAndIcaoValues();
                const bool isConnected = sGui->getIContextNetwork()->isConnected();
                this->setUiLoginState(isConnected);
            }

            // we decided to make it difficult for users to disable it
            if (!CBuildConfig::isLocalDeveloperDebugBuild())
            {
                ui->cb_AutoLogoff->setChecked(true);
            }
        }

        void CLoginComponent::setLogoffCountdown(int timeoutSeconds)
        {
            if (timeoutSeconds < 0) { timeoutSeconds = LogoffIntervalSeconds; }

            ui->pb_LogoffTimeout->setMaximum(timeoutSeconds);
            ui->pb_LogoffTimeout->setValue(timeoutSeconds);
            m_logoffIntervalSeconds = timeoutSeconds;
        }

        void CLoginComponent::loginCancelled()
        {
            m_logoffCountdownTimer.stop();
            this->setLogoffCountdown(); // reset time
            this->closeOverlay();
            emit this->loginOrLogoffCancelled();
        }

        void CLoginComponent::toggleNetworkConnection()
        {
            if (!sGui || sGui->isShuttingDown()) { return; }
            if (!sGui->getIContextNetwork() || !sGui->getIContextAudio()) { return; }

            const bool isConnected = sGui && sGui->getIContextNetwork()->isConnected();
            const bool vatsimLogin = this->isVatsimNetworkTabSelected();
            m_networkSetup.setAutoLogoff(ui->cb_AutoLogoff->isChecked());

            ui->form_Pilot->setVatsimValidation(vatsimLogin);
            this->setUiLoginState(isConnected);

            // reset time
            this->setLogoffCountdown();

            CServer currentServer; // used for login
            CSimulatedAircraft ownAircraft; // used own aircraft
            CStatusMessage msg;
            if (!isConnected)
            {
                if (!this->validateAircraftValues())
                {
                    this->showOverlayHTMLMessage(CStatusMessage(this).validationWarning(u"Invalid aircraft data, login not possible"), OverlayMessageMs);
                    return;
                }

                const CStatusMessageList pilotMsgs = ui->form_Pilot->validate();
                if (pilotMsgs.isFailure())
                {
                    this->showOverlayHTMLMessage(CStatusMessage(this).validationWarning(u"Invalid pilot data, login not possible"), OverlayMessageMs);
                    return;
                }

                // sync values with GUI values
                this->updateOwnAircraftCallsignAndPilotFromGuiValues();
                this->updateOwnAircaftIcaoValuesFromGuiValues();

                // Login mode
                const CLoginMode mode = ui->frp_LoginMode->getLoginMode();
                if (mode.isObserver()) { CLogMessage(this).info(u"Login in observer mode"); }

                // Server
                currentServer = this->getCurrentServer();
                const CUser user = this->getUserFromPilotGuiValues();
                currentServer.setUser(user);

                // FSD setup, then override
                if (ui->form_FsdDetails->isFsdSetupEnabled())
                {
                    const CFsdSetup fsd = ui->form_FsdDetails->getValue();
                    currentServer.setFsdSetup(fsd);
                }

                if (ui->form_Voice->isVoiceSetupEnabled())
                {
                    const CVoiceSetup voice = ui->form_Voice->getValue();
                    currentServer.setVoiceSetup(voice);
                }

                ui->frp_CurrentServer->setServer(currentServer);
                sGui->getIContextOwnAircraft()->updateOwnAircraftPilot(currentServer.getUser());

                // set own aircraft from all values
                ownAircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();

                // Login
                if (sGui->getCContextAudioBase())
                {
                    sGui->getCContextAudioBase()->setVoiceSetup(currentServer.getVoiceSetup());
                }

                msg = sGui->getIContextNetwork()->connectToNetwork(currentServer, {}, true, {}, true, {}, mode);
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

        void CLoginComponent::onWebServiceDataRead(CEntityFlags::Entity entity, CEntityFlags::ReadState state, int number, const QUrl &url)
        {
            if (!CEntityFlags::isFinishedReadState(state)) { return; }
            if (!sGui || !sGui->getIContextNetwork() || sGui->isShuttingDown()) { return; }

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

        void CLoginComponent::loadRememberedUserData()
        {
            const CServer lastServer = m_networkSetup.getLastServer();
            if (!lastServer.isNull())
            {
                ui->tw_Network->setCurrentWidget(
                    lastServer.getServerType() == CServer::FSDServerVatsim ?
                    ui->tb_NetworkVatsim : ui->tb_OtherServers);
            }

            const CUser lastUser = lastServer.getUser();
            ui->form_Pilot->setUser(lastUser);
            if (lastUser.hasCallsign())
            {
                ui->le_Callsign->setText(lastUser.getCallsign().asString());
            }
            else if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                ui->le_Callsign->setText("SWIFT");
            }
        }

        void CLoginComponent::overrideCredentialsToPilot()
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
            ui->form_Pilot->setUser(server.getUser(), true);
        }

        void CLoginComponent::onSelectedServerChanged(const CServer &server)
        {
            if (!m_updatePilotOnServerChanges) { return; }
            const bool vatsim = this->isVatsimNetworkTabSelected();
            const CUser user = vatsim ? this->getCurrentVatsimServer().getUser() : server.getUser();
            ui->form_Pilot->setUser(user, true);
        }

        void CLoginComponent::onSimulatorStatusChanged(int status)
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

        void CLoginComponent::onNetworkStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
        {
            Q_UNUSED(from)
            if (to != CConnectionStatus::Connected) { return; }

            this->setUiLoginState(true);
            this->updateGui();
        }

        void CLoginComponent::onServerTabWidgetChanged(int index)
        {
            Q_UNUSED(index)
            if (!m_updatePilotOnServerChanges) { return; }
            const bool vatsim = this->isVatsimNetworkTabSelected();
            const CServer server = vatsim ? this->getCurrentVatsimServer() : this->getCurrentOtherServer();
            ui->form_Pilot->setUser(server.getUser(), true);
        }

        bool CLoginComponent::hasValidContexts() const
        {
            if (!sGui || !sGui->supportsContexts()) { return false; }
            if (sGui->isShuttingDown())          { return false; }
            if (!sGui->getIContextSimulator())   { return false; }
            if (!sGui->getIContextNetwork())     { return false; }
            if (!sGui->getIContextOwnAircraft()) { return false; }
            return true;
        }

        void CLoginComponent::setServerButtonsVisible(bool visible)
        {
            ui->wi_OtherServersButtons->setVisible(visible);
            ui->wi_VatsimButtons->setVisible(visible);
        }

        void CLoginComponent::onDetailsTabChanged(int index)
        {
            Q_UNUSED(index)
            const bool showNetwork = (ui->tw_Details->currentWidget() != ui->tb_FsdDetails);

            const CServer server = this->getCurrentServer();

            // only override if not yet enabled
            if (!ui->form_FsdDetails->isFsdSetupEnabled()) { ui->form_FsdDetails->setValue(server.getFsdSetup()); }
            if (!ui->form_Voice->isVoiceSetupEnabled())    { ui->form_Voice->setValue(server.getVoiceSetup()); }

            ui->tw_Network->setVisible(showNetwork);
            ui->tw_Details->setMinimumHeight(showNetwork ? 0 : 125);
        }

        CLoginComponent::CGuiAircraftValues CLoginComponent::getAircraftValuesFromGui() const
        {
            CGuiAircraftValues values;
            values.ownCallsign = CCallsign(ui->le_Callsign->text().trimmed().toUpper());
            values.ownAircraftIcao = ui->selector_AircraftIcao->getAircraftIcao();
            values.ownAirlineIcao  = ui->selector_AirlineIcao->getAirlineIcao();
            values.ownAircraftCombinedType   = ui->le_AircraftCombinedType->text().trimmed().toUpper();
            values.ownAircraftSimulatorModel = ui->le_SimulatorModel->text().trimmed().toUpper();
            return values;
        }

        CUser CLoginComponent::getUserFromPilotGuiValues() const
        {
            CUser user = ui->form_Pilot->getUser();
            user.setCallsign(this->getCallsignFromGui());
            return user;
        }

        CCallsign CLoginComponent::getCallsignFromGui() const
        {
            const CCallsign cs(ui->le_Callsign->text().trimmed().toUpper());
            return cs;
        }

        CServer CLoginComponent::getCurrentVatsimServer() const
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

        CServer CLoginComponent::getCurrentOtherServer() const
        {
            return ui->comp_OtherServers->currentServer();
        }

        CServer CLoginComponent::getCurrentServer() const
        {
            return this->isVatsimNetworkTabSelected() ? this->getCurrentVatsimServer() : this->getCurrentOtherServer();
        }

        void CLoginComponent::startLogoffTimerCountdown()
        {
            ui->pb_LogoffTimeout->setValue(m_logoffIntervalSeconds);
            m_logoffCountdownTimer.setInterval(1000);
            m_logoffCountdownTimer.start();
        }

        void CLoginComponent::setOwnModelAndIcaoValues(const CAircraftModel &ownModel)
        {
            if (!this->hasValidContexts()) { return; }
            CAircraftModel model = ownModel;
            const bool simulating = sGui->getIContextSimulator() &&
                                    (sGui->getIContextSimulator()->getSimulatorStatus() & ISimulator::Simulating);

            // fill simulator related values
            if (simulating)
            {
                if (!model.hasModelString())
                {
                    model = sGui->getIContextOwnAircraft()->getOwnAircraft().getModel();
                }
                const QString modelAndKey(model.getModelStringAndDbKey());
                ui->le_SimulatorModel->setText(modelAndKey);
                ui->le_SimulatorModel->home(false);
                this->highlightModelField(model);

                const CSimulatorInfo sim = sGui->getIContextSimulator()->getSimulatorPluginInfo().getSimulator();
                const CSimulatorInternals simulatorInternals = sGui->getIContextSimulator()->getSimulatorInternals();
                const QString simStr = sim.toQString() + QStringLiteral(" ") + simulatorInternals.getSimulatorVersion();
                CCrashHandler::instance()->crashAndLogInfoSimulator(simStr);
            }
            else
            {
                ui->le_SimulatorModel->clear();
                this->highlightModelField();
            }
            ui->le_SimulatorModel->setToolTip(model.asHtmlSummary());

            // reset the model
            bool changedOwnAircraftIcaoValues = false;

            if (model.isLoadedFromDb() || (model.getAircraftIcaoCode().isLoadedFromDb() && model.getLivery().isLoadedFromDb()))
            {
                // full model from DB, take all values
                this->setGuiIcaoValues(model, false);
            }
            else
            {
                // we have a model, which is not from DB
                model = this->getPrefillModel(); // manually entered values
                if (model.getLivery().hasValidDbKey() && model.getLivery().isColorLivery())
                {
                    // special case for color liveries/NO airline
                    ui->selector_AirlineIcao->clear();
                }

                this->setGuiIcaoValues(model, true);
                changedOwnAircraftIcaoValues = this->updateOwnAircaftIcaoValuesFromGuiValues();
            }

            const bool changedOwnAircraftCallsignPilot = this->updateOwnAircraftCallsignAndPilotFromGuiValues();
            if (changedOwnAircraftIcaoValues || changedOwnAircraftCallsignPilot)
            {
                m_changedLoginDataDigestSignal.inputSignal();
            }
        }

        bool CLoginComponent::setGuiIcaoValues(const CAircraftModel &model, bool onlyIfEmpty)
        {
            bool changed = false;
            if (!onlyIfEmpty || !ui->selector_AircraftIcao->isSet())
            {
                changed = ui->selector_AircraftIcao->setAircraftIcao(model.getAircraftIcaoCode());
            }
            if (!onlyIfEmpty || !ui->selector_AirlineIcao->isSet())
            {
                const bool c = ui->selector_AirlineIcao->setAirlineIcao(model.getAirlineIcaoCode());
                changed |= c;
            }
            if (!onlyIfEmpty || ui->le_AircraftCombinedType->text().trimmed().isEmpty())
            {
                const QString combined(model.getAircraftIcaoCode().getCombinedType());
                if (ui->le_AircraftCombinedType->text() != combined)
                {
                    ui->le_AircraftCombinedType->setText(combined);
                    changed = true;
                }
            }
            const bool valid = this->validateAircraftValues();
            return valid ? changed : false;
        }

        void CLoginComponent::setGuiLoginAsValues(const CSimulatedAircraft &ownAircraft)
        {
            const QString ac(
                ownAircraft.getAircraftIcaoCodeDesignator() %
                (ownAircraft.hasAirlineDesignator() ? (u' ' % ownAircraft.getAirlineIcaoCodeDesignator()) : QString()) %
                (ownAircraft.hasModelString() ? (u' ' % ownAircraft.getModelString()) : QString())
            );
            const QString cs = ownAircraft.getCallsignAsString();
            ui->le_LoginSince->setText(QDateTime::currentDateTimeUtc().toString());
            ui->le_LoginAsAircaft->setText(ac);
            ui->le_LoginAsAircaft->home(false);
            ui->le_LoginCallsign->setText(cs);
            if (!cs.isEmpty()) { ui->le_Callsign->setText(cs); }
        }

        bool CLoginComponent::validateAircraftValues()
        {
            CGuiAircraftValues values = this->getAircraftValuesFromGui();

            // fill in combined type if empty
            if (ui->le_AircraftCombinedType->text().isEmpty() && values.ownAircraftIcao.isLoadedFromDb())
            {
                ui->le_AircraftCombinedType->setText(values.ownAircraftIcao.getCombinedType());
                values.ownAircraftCombinedType = values.ownAircraftIcao.getCombinedType();
            }

            const bool validCombinedType = CAircraftIcaoCode::isValidCombinedType(values.ownAircraftCombinedType);
            ui->lblp_AircraftCombinedType->setTicked(validCombinedType);

            // airline is optional, e.g. C172 has no airline
            const bool validAirlineDesignator = values.ownAirlineIcao.hasValidDesignator() || values.ownAirlineIcao.getDesignator().isEmpty();
            ui->lblp_AirlineIcao->setTicked(validAirlineDesignator);

            const bool validAircraftDesignator = values.ownAircraftIcao.hasValidDesignator();
            ui->lblp_AircraftIcao->setTicked(validAircraftDesignator);

            const bool validCallsign = CCallsign::isValidAircraftCallsign(values.ownCallsign);
            ui->lblp_Callsign->setTicked(validCallsign);

            // model intentionally ignored
            return validCombinedType && validAirlineDesignator && validAircraftDesignator && validCallsign;
        }

        void CLoginComponent::onChangedAircraftIcao(const CAircraftIcaoCode &icao)
        {
            if (icao.isLoadedFromDb())
            {
                ui->le_AircraftCombinedType->setText(icao.getCombinedType());
            }
            this->validateAircraftValues();
        }

        void CLoginComponent::onChangedAirlineIcao(const CAirlineIcaoCode &icao)
        {
            Q_UNUSED(icao)
            this->validateAircraftValues();
        }

        void CLoginComponent::reloadOtherServersSetup()
        {
            const CServerList otherServers(m_networkSetup.getOtherServersPlusPredefinedServers());
            ui->comp_OtherServers->setServers(otherServers);
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
            if (!ui->cb_AutoLogoff->isChecked()) { return; }
            if (!this->hasValidContexts()) { return; }
            if (!sGui->getIContextNetwork()->isConnected()) { return; } // nothing to logoff

            const CStatusMessage m = CStatusMessage(this, CStatusMessage::SeverityInfo, u"Auto logoff in progress (could be simulator shutdown, crash, closing simulator)");
            const int delaySecs = 20;
            this->showOverlayHTMLMessage(m, qRound(1000 * delaySecs * 0.8));
            this->setLogoffCountdown(delaySecs);
            this->startLogoffTimerCountdown();

            emit this->requestLoginPage();
        }

        void CLoginComponent::autoLogoffFrameRate(bool fatal)
        {
            //! \fixme code duplication with function above
            if (!ui->cb_AutoLogoff->isChecked()) { return; }
            if (!this->hasValidContexts()) { return; }
            if (!sGui->getIContextNetwork()->isConnected()) { return; }

            const auto msg = fatal
                ? CStatusMessage(this, CStatusMessage::SeverityError, u"Sim frame rate too low to maintain constant simulation rate. Disconnecting to avoid disrupting the network.")
                : CStatusMessage(this, CStatusMessage::SeverityWarning, u"Sim frame rate too low to maintain constant simulation rate. Reduce graphics quality to avoid disconnection.");
            const int delaySecs = 20;
            this->showOverlayHTMLMessage(msg, qRound(1000 * delaySecs * 0.8));
            if (fatal)
            {
                this->setLogoffCountdown(delaySecs);
                this->startLogoffTimerCountdown();
            }

            emit this->requestLoginPage();
        }

        void CLoginComponent::lookupOwnAircraftModel()
        {
            if (!this->hasValidContexts()) { return; }
            if (!sGui->getIContextSimulator()->isSimulatorAvailable()) { return; }
            const CAircraftModel model(sGui->getIContextOwnAircraft()->getOwnAircraft().getModel());
            this->onSimulatorModelChanged(model);
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
                CLogMessage(this).validationInfo(u"Invalid lookup for '%1' successful: %2") << modelStr << reverseModel.toQString();
                CLogMessage(this).validationInfo(u"Hint: Are you using the emulated driver? Set a model if so!");
                return;
            }
            this->setOwnModelAndIcaoValues(reverseModel);

            // open dialog for model mapping
            if (m_autoPopupWizard && !reverseModel.isLoadedFromDb())
            {
                this->mappingWizard();
            }

            // check state of own aircraft
            this->updateOwnAircraftCallsignAndPilotFromGuiValues();

            // let others know data changed
            m_changedLoginDataDigestSignal.inputSignal();
        }

        void CLoginComponent::mappingWizard()
        {
            if (!sGui || !sGui->getIContextOwnAircraft() || sGui->isShuttingDown()) { return; }
            if (!sGui->hasMinimumMappingVersion()) { return; }

            if (!m_mappingWizard)
            {
                m_mappingWizard.reset(new CDbQuickMappingWizard(this));
            }

            if (sGui->getIContextSimulator()->isSimulatorAvailable())
            {
                // preset on model
                const CAircraftModel model(sGui->getIContextOwnAircraft()->getOwnAircraft().getModel());
                m_mappingWizard->presetModel(model);
            }
            else
            {
                // preset on GUI values only
                const CAircraftIcaoCode icao(ui->selector_AircraftIcao->getAircraftIcao());
                m_mappingWizard->presetAircraftIcao(icao);
            }
            m_mappingWizard->show();
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

        void CLoginComponent::setUiLoginState(bool connected)
        {
            ui->fr_LoginDisconnected->setVisible(!connected);
            ui->fr_LogoffConfirmationConnected->setVisible(connected);

            const QString s = connected ? QStringLiteral("disconnect") : QStringLiteral("connect");
            ui->pb_Ok->setText(s);
        }

        void CLoginComponent::highlightModelField(const CAircraftModel &model)
        {
            if (!model.hasModelString())      { ui->le_SimulatorModel->setProperty("validation", "error"); }
            else if (!model.isLoadedFromDb()) { ui->le_SimulatorModel->setProperty("validation", "warning"); }
            else                              { ui->le_SimulatorModel->setProperty("validation", "ok"); }
            ui->le_SimulatorModel->setStyleSheet(""); // force update
        }

        bool CLoginComponent::isVatsimNetworkTabSelected() const
        {
            return (ui->tw_Network->currentWidget() == ui->tb_NetworkVatsim);
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
            const QString cs(ui->le_Callsign->text().trimmed().toUpper());
            bool changedCallsign = false;
            if (!cs.isEmpty() && ownAircraft.getCallsignAsString() != cs)
            {
                const CCallsign callsign(cs, CCallsign::Aircraft);
                sGui->getIContextOwnAircraft()->updateOwnCallsign(callsign);
                ownAircraft.setCallsign(callsign); // also update
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

        bool CLoginComponent::updateOwnAircaftIcaoValuesFromGuiValues()
        {
            if (!this->hasValidContexts()) { return false; }
            const CSimulatedAircraft ownAircraft(sGui->getIContextOwnAircraft()->getOwnAircraft());
            const CGuiAircraftValues aircraftValues = this->getAircraftValuesFromGui();

            CAircraftIcaoCode aircraftCode(ownAircraft.getAircraftIcaoCode());
            CAirlineIcaoCode airlineCode(ownAircraft.getAirlineIcaoCode());

            bool changedIcaoCodes = false;
            if (aircraftValues.ownAircraftIcao.hasValidDesignator() && aircraftValues.ownAircraftIcao != aircraftCode)
            {
                aircraftCode = aircraftValues.ownAircraftIcao;
                changedIcaoCodes = true;
            }
            if (aircraftValues.ownAirlineIcao.hasValidDesignator() && aircraftValues.ownAirlineIcao != airlineCode)
            {
                airlineCode = aircraftValues.ownAirlineIcao;
                changedIcaoCodes = true;
            }

            if (changedIcaoCodes)
            {
                sGui->getIContextOwnAircraft()->updateOwnIcaoCodes(aircraftCode, airlineCode);
            }

            return changedIcaoCodes;
        }

        void CLoginComponent::updateGui()
        {
            if (!this->hasValidContexts())   { return; }
            if (!sGui->getIContextNetwork()) { return; }
            const IContextNetwork *nwc = sGui->getIContextNetwork();
            const bool connected = nwc->isConnected();
            if (!connected) { return; }
            const CSimulatedAircraft ownAircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();
            this->setGuiLoginAsValues(ownAircraft);
            this->setUiLoginState(connected);
            this->setOwnModelAndIcaoValues();
            const CServer server = nwc->getConnectedServer();
            ui->le_LoginHomeBase->setText(server.getUser().getHomeBase().asString());
            ui->frp_CurrentServer->setServer(server);
            ui->frp_LoginMode->setLoginMode(nwc->getLoginMode());
        }
    } // namespace
} // namespace
