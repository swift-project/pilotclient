/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/webdataservices.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/network.h"
#include "blackcore/simulator.h"
#include "blackgui/components/logincomponent.h"
#include "blackgui/editors/serverform.h"
#include "blackgui/components/serverlistselector.h"
#include "blackgui/guiapplication.h"
#include "blackgui/loginmodebuttons.h"
#include "blackgui/ticklabel.h"
#include "blackgui/uppercasevalidator.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/statusmessage.h"
#include "ui_logincomponent.h"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QIntValidator>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QTabWidget>
#include <QTimer>
#include <QToolButton>
#include <QCompleter>
#include <QStyledItemDelegate>
#include <QtGlobal>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
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
        const CLogCategoryList &CLoginComponent::getLogCategories()
        {
            static const BlackMisc::CLogCategoryList cats { BlackMisc::CLogCategory::guiComponent() };
            return cats;
        }

        CLoginComponent::CLoginComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CLoginComponent)
        {
            ui->setupUi(this);
            this->m_logoffCountdownTimer = new QTimer(this);
            this->m_logoffCountdownTimer->setObjectName("CLoginComponent:m_logoffCountdownTimer");
            this->ui->pb_LogoffTimeout->setMaximum(LogoffIntervalSeconds);
            this->ui->pb_LogoffTimeout->setValue(LogoffIntervalSeconds);
            connect(this->m_logoffCountdownTimer, &QTimer::timeout, this, &CLoginComponent::ps_logoffCountdown);

            setOkButtonString(false);
            connect(this->ui->bb_OkCancel, &QDialogButtonBox::rejected, this, &CLoginComponent::ps_loginCancelled);
            connect(this->ui->bb_OkCancel, &QDialogButtonBox::accepted, this, &CLoginComponent::ps_toggleNetworkConnection);
            connect(this->ui->pb_OtherServersGotoSettings, &QPushButton::pressed, this, &CLoginComponent::requestNetworkSettings);

            this->ui->comp_FsdDetails->showEnableInfo(true);
            this->ui->comp_FsdDetails->setFsdSetupEnabled(false);

            this->ui->lblp_SimulatorModel->setToolTips("available", "unavailable");
            this->ui->lblp_SimulatorModel->setPixmapUnticked(CIcons::empty());
            this->ui->lblp_AircraftCombinedType->setToolTips("ok", "wrong");
            this->ui->lblp_AircraftIcaoAirline->setToolTips("ok", "wrong");
            this->ui->lblp_AircraftIcaoDesignator->setToolTips("ok", "wrong");
            this->ui->lblp_Callsign->setToolTips("ok", "wrong");
            this->ui->lblp_VatsimHomeAirport->setToolTips("ok", "wrong");
            this->ui->lblp_VatsimId->setToolTips("ok", "wrong");
            this->ui->lblp_VatsimPassword->setToolTips("ok", "wrong");
            this->ui->lblp_VatsimRealName->setToolTips("ok", "wrong");

            // Stored data
            this->loadRememberedVatsimData();

            // Remark: The validators affect the signals such as returnPressed, editingFinished
            // So I use no ranges in the CUpperCaseValidators, as this disables the signals for invalid values

            // VATSIM
            this->ui->le_VatsimId->setValidator(new QIntValidator(100000, 9999999, this));
            connect(ui->le_VatsimId, &QLineEdit::editingFinished, this, &CLoginComponent::ps_validateVatsimValues);

            this->ui->le_VatsimHomeAirport->setValidator(new CUpperCaseValidator(this));
            connect(ui->le_VatsimHomeAirport, &QLineEdit::editingFinished, this, &CLoginComponent::ps_validateVatsimValues);

            connect(ui->le_VatsimPassword, &QLineEdit::editingFinished, this, &CLoginComponent::ps_validateVatsimValues);
            connect(ui->le_VatsimRealName, &QLineEdit::editingFinished, this, &CLoginComponent::ps_validateVatsimValues);

            // own aircraft
            this->ui->le_Callsign->setMaxLength(LogoffIntervalSeconds);
            this->ui->le_Callsign->setValidator(new CUpperCaseValidator(this));
            connect(ui->le_Callsign, &QLineEdit::editingFinished, this, &CLoginComponent::ps_validateAircraftValues);

            this->ui->le_AircraftCombinedType->setMaxLength(3);
            this->ui->le_AircraftCombinedType->setValidator(new CUpperCaseValidator(this));
            connect(ui->le_AircraftCombinedType, &QLineEdit::editingFinished, this, &CLoginComponent::ps_validateAircraftValues);

            this->ui->le_AirlineIcaoDesignator->setMaxLength(5);
            this->ui->le_AirlineIcaoDesignator->setValidator(new CUpperCaseValidator(this));
            connect(ui->le_AirlineIcaoDesignator, &QLineEdit::editingFinished, this, &CLoginComponent::ps_validateAircraftValues);

            this->ui->le_AircraftIcaoDesignator->setMaxLength(5);
            this->ui->le_AircraftIcaoDesignator->setValidator(new CUpperCaseValidator(this));
            connect(ui->le_AircraftIcaoDesignator, &QLineEdit::editingFinished, this, &CLoginComponent::ps_validateAircraftValues);
            connect(ui->tb_SimulatorIcaoReverseLookup, &QToolButton::clicked, this, &CLoginComponent::ps_reverseLookupModel);

            // server GUI element
            this->ui->frp_CurrentServer->setReadOnly(true);
            this->ui->frp_CurrentServer->showPasswordField(false);

            connect(sGui->getIContextNetwork(), &IContextNetwork::webServiceDataRead, this, &CLoginComponent::ps_onWebServiceDataRead);

            // inital setup, if data already available
            this->ps_validateAircraftValues();
            this->ps_validateVatsimValues();
            this->ps_onWebServiceDataRead(CEntityFlags::VatsimDataFile, CEntityFlags::ReadFinished, -1);
            CServerList otherServers(this->m_otherTrafficNetworkServers.getThreadLocal());

            // add a testserver when no servers can be loaded
            if (otherServers.isEmpty() && (sGui->isRunningInDeveloperEnvironment() || CBuildConfig::isBetaTest()))
            {
                otherServers.push_back(sGui->getGlobalSetup().getFsdTestServersPlusHardcodedServers());
                CLogMessage(this).info("Added servers for testing");
            }
            this->ui->comp_OtherServers->setServers(otherServers);
        }

        CLoginComponent::~CLoginComponent()
        { }

        void CLoginComponent::mainInfoAreaChanged(const QWidget *currentWidget)
        {
            this->m_logoffCountdownTimer->stop(); // in any case stop the timer
            if (currentWidget != this && currentWidget != this->parentWidget())
            {
                this->m_visible = false;
                this->m_logoffCountdownTimer->stop();
            }
            else
            {
                this->setOwnModel();
                if (this->m_visible)
                {
                    // already visible:
                    // re-trigger! treat as same as OK
                    this->ps_toggleNetworkConnection();
                }
                else
                {
                    this->m_visible = true;
                    bool isConnected = sGui->getIContextNetwork()->isConnected();
                    this->setGuiVisibility(isConnected);
                    this->setOkButtonString(isConnected);
                    if (isConnected) { this->startLogoffTimerCountdown(); }
                }
            }
        }

        void CLoginComponent::ps_loginCancelled()
        {
            this->m_logoffCountdownTimer->stop();
            this->ui->pb_LogoffTimeout->setValue(LogoffIntervalSeconds);
            emit loginOrLogoffCancelled();
        }

        void CLoginComponent::ps_toggleNetworkConnection()
        {
            if (this->ui->tw_Network->currentWidget() == this->ui->pg_FsdDetails)
            {
                CLogMessage(this).validationError("No login possible from this very tab, use VATSIM or other servers");
                return;
            }
            const bool isConnected = sGui->getIContextNetwork()->isConnected();
            const bool vatsimLogin = (this->ui->tw_Network->currentWidget() == this->ui->pg_NetworkVatsim);
            CServer currentServer; // used for login
            CSimulatedAircraft ownAircraft; // used own aircraft
            CStatusMessage msg;
            if (!isConnected)
            {

                if (!this->ps_validateAircraftValues())
                {
                    CLogMessage(this).validationWarning("Invalid aircraft data, login not possible");
                    return;
                }

                if (vatsimLogin && !this->ps_validateVatsimValues())
                {
                    CLogMessage(this).validationWarning("Invalid VATSIM data, login not possible");
                    return;
                }

                // sync values with GUI values
                CGuiAircraftValues aircraftValues = this->getAircraftValuesFromGui();
                ownAircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();
                CAircraftIcaoCode aircraftCode(ownAircraft.getAircraftIcaoCode());
                CAirlineIcaoCode airlineCode(ownAircraft.getAirlineIcaoCode());

                bool setIcaoCodes = false;
                if (!ownAircraft.hasAircraftDesignator() && !aircraftValues.ownAircraftIcaoTypeDesignator.isEmpty())
                {
                    aircraftCode = CAircraftIcaoCode(aircraftValues.ownAircraftIcaoTypeDesignator, aircraftValues.ownAircraftCombinedType);
                    setIcaoCodes = true;
                }
                if (!ownAircraft.hasAircraftDesignator() && !aircraftValues.ownAircraftIcaoAirline.isEmpty())
                {
                    airlineCode = CAirlineIcaoCode(aircraftValues.ownAircraftIcaoAirline);
                    setIcaoCodes = true;
                }

                if (ownAircraft.getCallsign().asString() != aircraftValues.ownCallsign)
                {
                    ownAircraft.setCallsign(aircraftValues.ownCallsign);
                    sGui->getIContextOwnAircraft()->updateOwnCallsign(ownAircraft.getCallsign());
                }

                if (setIcaoCodes)
                {
                    ownAircraft.setIcaoCodes(aircraftCode, airlineCode);
                    sGui->getIContextOwnAircraft()->updateOwnIcaoCodes(ownAircraft.getAircraftIcaoCode(), ownAircraft.getAirlineIcaoCode());
                }

                // Login mode
                INetwork::LoginMode mode = ui->gbp_LoginMode->getLoginMode();
                switch (mode)
                {
                case INetwork::LoginStealth:
                    CLogMessage(this).info("login in stealth mode");
                    break;
                case INetwork::LoginAsObserver:
                    CLogMessage(this).info("login in observer mode");
                    break;
                case INetwork::LoginNormal:
                default:
                    break;
                }

                // Server
                if (vatsimLogin)
                {
                    currentServer = this->getCurrentVatsimServer();
                    const CUser vatsimUser = this->getUserFromVatsimGuiValues();
                    currentServer.setUser(vatsimUser);
                }
                else
                {
                    currentServer = this->getCurrentOtherServer();
                }

                // FSD setup, then override
                if (ui->comp_FsdDetails->isFsdSetuoEnabled())
                {
                    const CFsdSetup fsd = ui->comp_FsdDetails->getValue();
                    currentServer.setFsdSetup(fsd);
                }

                this->ui->frp_CurrentServer->setServer(currentServer);
                sGui->getIContextOwnAircraft()->updateOwnAircraftPilot(currentServer.getUser());

                // Login
                msg = sGui->getIContextNetwork()->connectToNetwork(currentServer, mode);
            }
            else
            {
                // disconnect from network
                sGui->getIContextAudio()->leaveAllVoiceRooms();
                msg = sGui->getIContextNetwork()->disconnectFromNetwork();
            }

            // log message and trigger events
            msg.addCategories(this);
            CLogMessage::preformatted(msg);
            if (msg.isSeverityInfoOrLess())
            {
                emit loginOrLogoffSuccessful();
                if (vatsimLogin)
                {
                    this->m_currentVatsimServer.set(currentServer);
                    this->m_currentAircraftModel.setAndSave(ownAircraft.getModel());
                }
            }
            else
            {
                emit loginOrLogoffCancelled();
            }
        }

        void CLoginComponent::ps_onWebServiceDataRead(int entityInt, int stateInt, int number)
        {
            const CEntityFlags::EntityFlag entity = static_cast<CEntityFlags::EntityFlag>(entityInt);
            const CEntityFlags::ReadState state = static_cast<CEntityFlags::ReadState>(stateInt);
            if (state != CEntityFlags::ReadFinished) { return; }
            Q_UNUSED(number);

            if (entity == CEntityFlags::VatsimDataFile)
            {
                CServerList vatsimFsdServers = sGui->getIContextNetwork()->getVatsimFsdServers();
                if (vatsimFsdServers.isEmpty()) { return; }
                vatsimFsdServers.sortBy(&CServer::getName);
                const CServer currentServer = this->m_currentVatsimServer.get();
                this->ui->comp_VatsimServer->setServers(vatsimFsdServers, true);
                this->ui->comp_VatsimServer->preSelect(currentServer.getName());
            }
            else
            {
                this->initCompleters(entity);
            }
        }

        void CLoginComponent::setGuiValuesFromAircraft(const CSimulatedAircraft &ownAircraft)
        {
            const CAircraftIcaoCode aircraftIcao = ownAircraft.getAircraftIcaoCode();
            this->ui->le_Callsign->setText(ownAircraft.getCallsignAsString());
            this->ui->le_AircraftIcaoDesignator->setText(aircraftIcao.getDesignator());
            this->ui->le_AirlineIcaoDesignator->setText(ownAircraft.getAirlineIcaoCodeDesignator());
            this->ui->le_AircraftCombinedType->setText(aircraftIcao.getCombinedType());
        }

        void CLoginComponent::loadRememberedVatsimData()
        {
            const CServer lastServer = this->m_currentVatsimServer.get();
            const CUser lastUser = lastServer.getUser();
            if (lastUser.hasValidCallsign())
            {
                this->ui->le_Callsign->setText(lastUser.getCallsign().asString());
                this->ui->le_VatsimId->setText(lastUser.getId());
                this->ui->le_VatsimPassword->setText(lastUser.getPassword());
                this->ui->le_VatsimHomeAirport->setText(lastUser.getHomebase().asString());
                this->ui->le_VatsimRealName->setText(lastUser.getRealName());
            }
            else
            {
                this->ui->le_Callsign->setText("SWIFT");
                this->ui->le_VatsimId->setText("1288459");
                this->ui->le_VatsimPassword->setText("4769");
                this->ui->le_VatsimHomeAirport->setText("LOWI");
                this->ui->le_VatsimRealName->setText("Black Swift");
            }
            this->ui->comp_OtherServers->preSelect(lastServer.getName());
        }

        CLoginComponent::CGuiAircraftValues CLoginComponent::getAircraftValuesFromGui() const
        {
            CGuiAircraftValues values;
            values.ownCallsign = this->ui->le_Callsign->text().trimmed().toUpper();
            values.ownAircraftIcaoTypeDesignator = CAircraftIcaoCode::normalizeDesignator(this->ui->le_AircraftIcaoDesignator->text());
            values.ownAircraftIcaoAirline = CAirlineIcaoCode::normalizeDesignator(this->ui->le_AirlineIcaoDesignator->text());
            values.ownAircraftCombinedType = this->ui->le_AircraftCombinedType->text().trimmed().toUpper();
            values.ownAircraftSimulatorModel = this->ui->le_SimulatorModel->text().trimmed().toUpper();
            return values;
        }

        CLoginComponent::CVatsimValues CLoginComponent::getVatsimValuesFromGui() const
        {
            CVatsimValues values;
            values.vatsimHomeAirport = this->ui->le_VatsimHomeAirport->text().trimmed().toUpper();
            values.vatsimId = this->ui->le_VatsimId->text().trimmed();
            values.vatsimPassword = this->ui->le_VatsimPassword->text().trimmed();
            values.vatsimRealName = this->ui->le_VatsimRealName->text().simplified().trimmed();
            return values;
        }

        CUser CLoginComponent::getUserFromVatsimGuiValues() const
        {
            CVatsimValues values = getVatsimValuesFromGui();
            CUser user(values.vatsimId, values.vatsimRealName, "", values.vatsimPassword, getCallsignFromGui());
            user.setHomebase(values.vatsimHomeAirport);
            return user;
        }

        CCallsign CLoginComponent::getCallsignFromGui() const
        {
            CCallsign cs(this->ui->le_Callsign->text().trimmed().toUpper());
            return cs;
        }

        CServer CLoginComponent::getCurrentVatsimServer() const
        {
            return this->ui->comp_VatsimServer->currentServer();
        }

        CServer CLoginComponent::getCurrentOtherServer() const
        {
            return this->ui->comp_OtherServers->currentServer();
        }

        void CLoginComponent::setOkButtonString(bool connected)
        {
            QString s = connected ? "Disconnect" : "Connect";
            this->ui->bb_OkCancel->button(QDialogButtonBox::Ok)->setText(s);
        }

        void CLoginComponent::setGuiVisibility(bool connected)
        {
            this->ui->gbp_LoginMode->setVisible(!connected);
            this->ui->gb_OwnAircraft->setVisible(!connected);
            this->ui->gb_Network->setVisible(!connected);
            this->ui->fr_LogoffConfirmation->setVisible(connected);
        }

        void CLoginComponent::startLogoffTimerCountdown()
        {
            this->ui->pb_LogoffTimeout->setValue(LogoffIntervalSeconds);
            this->m_logoffCountdownTimer->setInterval(1000);
            this->m_logoffCountdownTimer->start();
        }

        void CLoginComponent::setOwnModel()
        {
            Q_ASSERT(sGui->getIContextOwnAircraft());
            Q_ASSERT(sGui->getIContextSimulator());

            CAircraftModel model;
            const bool simulating = sGui->getIContextSimulator() &&
                                    (sGui->getIContextSimulator()->getSimulatorStatus() & ISimulator::Simulating);
            if (simulating)
            {
                model = sGui->getIContextOwnAircraft()->getOwnAircraft().getModel();
                this->ui->le_SimulatorModel->setText(model.getModelString());
            }
            else
            {
                static const CAircraftModel defaultModel(
                    "", CAircraftModel::TypeOwnSimulatorModel, "default model",
                    CAircraftIcaoCode("C172", "L1P", "Cessna", "172", "L", true, false, false, 0));
                model = this->m_currentAircraftModel.get();
                if (!model.hasAircraftDesignator()) { model = defaultModel; }
                this->ui->gbp_LoginMode->setLoginMode(INetwork::LoginNormal); //! \todo Set observer mode without simulator, currently not working in OBS mode
                this->ui->le_SimulatorModel->setText("No simulator");
            }

            if (model.hasAircraftDesignator())
            {
                this->setGuiIcaoValues(model, false);
            }
        }

        void CLoginComponent::setGuiIcaoValues(const CAircraftModel &model, bool onlyIfEmpty)
        {
            if (!onlyIfEmpty || this->ui->le_AircraftIcaoDesignator->text().trimmed().isEmpty())
            {
                this->ui->le_AircraftIcaoDesignator->setText(model.getAircraftIcaoCode().getDesignator());
            }
            if (!onlyIfEmpty || this->ui->le_AirlineIcaoDesignator->text().trimmed().isEmpty())
            {
                this->ui->le_AirlineIcaoDesignator->setText(model.getAirlineIcaoCode().getDesignator());
            }
            if (!onlyIfEmpty || this->ui->le_AircraftCombinedType->text().trimmed().isEmpty())
            {
                this->ui->le_AircraftCombinedType->setText(model.getAircraftIcaoCode().getCombinedType());
            }
            this->ps_validateAircraftValues();
        }

        bool CLoginComponent::ps_validateAircraftValues()
        {
            CGuiAircraftValues values = getAircraftValuesFromGui();

            bool validCombinedType = CAircraftIcaoCode::isValidCombinedType(values.ownAircraftCombinedType);
            this->ui->lblp_AircraftCombinedType->setTicked(validCombinedType);

            bool validAirlineDesignator = values.ownAircraftIcaoAirline.isEmpty() || CAircraftIcaoCode::isValidDesignator(values.ownAircraftIcaoAirline);
            this->ui->lblp_AircraftIcaoAirline->setTicked(validAirlineDesignator);

            bool validIcaoDesignator = CAircraftIcaoCode::isValidDesignator(values.ownAircraftIcaoTypeDesignator);
            this->ui->lblp_AircraftIcaoDesignator->setTicked(validIcaoDesignator);

            bool validCallsign = CCallsign::isValidAircraftCallsign(values.ownCallsign);
            this->ui->lblp_Callsign->setTicked(validCallsign);

            bool validSimulatorModel = !values.ownAircraftSimulatorModel.isEmpty();
            this->ui->lblp_SimulatorModel->setTicked(validSimulatorModel);

            // model intentionally ignored
            return validCombinedType && validAirlineDesignator && validIcaoDesignator && validCallsign;
        }

        bool CLoginComponent::ps_validateVatsimValues()
        {
            CVatsimValues values = getVatsimValuesFromGui();

            bool validVatsimId = CUser::isValidVatsimId(values.vatsimId);
            this->ui->lblp_VatsimId->setTicked(validVatsimId);

            bool validHomeAirport = values.vatsimHomeAirport.isEmpty() || CAirportIcaoCode::isValidIcaoDesignator(values.vatsimHomeAirport);
            this->ui->lblp_VatsimHomeAirport->setTicked(validHomeAirport);

            bool validVatsimPassword = !values.vatsimPassword.isEmpty();
            this->ui->lblp_VatsimPassword->setTicked(validVatsimPassword);

            bool validRealUserName = !values.vatsimRealName.isEmpty();
            this->ui->lblp_VatsimRealName->setTicked(validRealUserName);

            return validVatsimId && validHomeAirport && validVatsimPassword && validRealUserName;
        }

        void CLoginComponent::ps_reloadSettings()
        {
            CServerList otherServers(this->m_otherTrafficNetworkServers.getThreadLocal());
            this->ui->comp_OtherServers->setServers(otherServers);
        }

        void CLoginComponent::ps_logoffCountdown()
        {
            int v = this->ui->pb_LogoffTimeout->value();
            v -= 1;
            if (v < 0) { v = 0; }
            this->ui->pb_LogoffTimeout->setValue(v);
            if (v <= 0)
            {
                this->m_logoffCountdownTimer->stop();
                this->ps_toggleNetworkConnection();
            }
        }

        void CLoginComponent::ps_reverseLookupModel()
        {
            if (!sGui->getIContextSimulator()->isSimulatorAvailable()) { return; }
            CAircraftModel model(sGui->getIContextOwnAircraft()->getOwnAircraft().getModel());
            QString modelStr(model.hasModelString() ? model.getModelString() : "<unknown>");
            if (model.getAircraftIcaoCode().hasDesignator())
            {
                CLogMessage(this).validationInfo("Reverse lookup for '%1'") << modelStr;

                // update GUI
                this->setGuiIcaoValues(model, false);
            }
            else
            {
                CLogMessage(this).validationInfo("Reverse lookup for '%1'' failed, set data manually") << modelStr;
            }
        }

        void CLoginComponent::initCompleters(CEntityFlags::Entity entity)
        {
            // completers where possible
            if (sGui && sGui->getWebDataServices())
            {
                //! \todo fill in when airports are ready

                if (entity.testFlag(CEntityFlags::AircraftIcaoEntity) && !ui->le_AircraftIcaoDesignator->completer())
                {
                    const QStringList aircraftDesignators = sGui->getWebDataServices()->getAircraftIcaoCodes().toCompleterStrings();
                    if (!aircraftDesignators.isEmpty())
                    {
                        QCompleter *completer = new QCompleter(aircraftDesignators, this);
                        QStyledItemDelegate *itemDelegate = new QStyledItemDelegate(completer);
                        completer->popup()->setItemDelegate(itemDelegate);
                        ui->le_AircraftIcaoDesignator->setCompleter(completer);
                        completer->popup()->setObjectName("AircraftCompleter");
                        completer->popup()->setMinimumWidth(175);
                    }
                }

                if (entity.testFlag(CEntityFlags::AirlineIcaoEntity) && !ui->le_AirlineIcaoDesignator->completer())
                {
                    const QStringList airlineDesignators = sGui->getWebDataServices()->getAirlineIcaoCodes().toIcaoDesignatorNameCountryCompleterStrings();
                    if (!airlineDesignators.isEmpty())
                    {
                        QCompleter *completer = new QCompleter(airlineDesignators, this);
                        QStyledItemDelegate *itemDelegate = new QStyledItemDelegate(completer);
                        completer->popup()->setItemDelegate(itemDelegate);
                        ui->le_AirlineIcaoDesignator->setCompleter(completer);
                        completer->popup()->setObjectName("AirlineCompleter");
                        completer->popup()->setMinimumWidth(175);
                    }
                }
            }
        }
    } // namespace
} // namespace
