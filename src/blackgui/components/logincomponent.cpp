/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "logincomponent.h"
#include "ui_logincomponent.h"
#include "blackgui/uppercasevalidator.h"
#include "blackgui/guiapplication.h"
#include "blackcore/contextnetwork.h"
#include "blackcore/contextownaircraft.h"
#include "blackcore/contextaudio.h"
#include "blackcore/contextsimulator.h"
#include "blackcore/network.h"
#include "blackcore/simulator.h"
#include "blackcore/setupreader.h"
#include "blackmisc/logmessage.h"
#include <QIntValidator>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackCore;
using namespace BlackCore::Data;
using namespace BlackGui;

namespace BlackGui
{
    namespace Components
    {
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

            // Settings loaded
            this->loadFromSettings();

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

            this->ui->le_AircraftIcaoAirline->setMaxLength(5);
            this->ui->le_AircraftIcaoAirline->setValidator(new CUpperCaseValidator(this));
            connect(ui->le_AircraftIcaoAirline, &QLineEdit::editingFinished, this, &CLoginComponent::ps_validateAircraftValues);

            this->ui->le_AircraftIcaoDesignator->setMaxLength(5);
            this->ui->le_AircraftIcaoDesignator->setValidator(new CUpperCaseValidator(this));
            connect(ui->le_AircraftIcaoDesignator, &QLineEdit::editingFinished, this, &CLoginComponent::ps_validateAircraftValues);
            connect(ui->tb_SimulatorIcaoReverseLookup, &QToolButton::clicked, this, &CLoginComponent::ps_reverseLookupModel);

            // server GUI element
            this->ui->frp_CurrentServer->setReadOnly(true);
            this->ui->frp_CurrentServer->showPasswordField(false);

            connect(sGui->getIContextNetwork(), &IContextNetwork::webServiceDataRead, this, &CLoginComponent::ps_onWebServiceDataRead);

            // inital setup, if data already available
            ps_validateAircraftValues();
            ps_validateVatsimValues();
            ps_onWebServiceDataRead(CEntityFlags::VatsimDataFile, CEntityFlags::ReadFinished, -1);
            CServerList otherServers(this->m_otherTrafficNetworkServers.get());

            // add a testserver when no servers can be loaded
            if (otherServers.isEmpty() && CProject::isRunningInBetaOrDeveloperEnvironment())
            {
                otherServers.push_back(m_setup.get().fsdTestServersPlusHardcodedServers());
                CLogMessage(this).info("Added servers for testing");
            }
            this->ui->cbp_OtherServers->setServers(otherServers);
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
            bool isConnected = sGui->getIContextNetwork()->isConnected();
            CStatusMessage msg;
            if (!isConnected)
            {

                bool vatsimLogin = (this->ui->tw_Network->currentWidget() == this->ui->pg_NetworkVatsim);
                if (!this->ps_validateAircraftValues())
                {
                    CLogMessage(this).warning("Invalid aircraft data, login not possible");
                    return;
                }

                if (vatsimLogin && !this->ps_validateVatsimValues())
                {
                    CLogMessage(this).warning("Invalid VATSIM data, login not possible");
                    return;
                }

                // sync values with GUI values
                CGuiAircraftValues aircraftValues = this->getAircraftValuesFromGui();
                CSimulatedAircraft ownAircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();
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
                CServer currentServer;
                if (vatsimLogin)
                {
                    currentServer = this->getCurrentVatsimServer();
                    CUser vatsimUser = this->getUserFromVatsimGuiValues();
                    currentServer.setUser(vatsimUser);
                }
                else
                {
                    currentServer = this->getCurrentOtherServer();
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
            }
            else
            {
                emit loginOrLogoffCancelled();
            }
        }

        void CLoginComponent::ps_onWebServiceDataRead(int entityI, int stateI, int number)
        // void CLoginComponent::ps_onWebServiceDataRead(CEntityFlags::Entity entity, CEntityFlags::ReadState, int number)
        {
            CEntityFlags::EntityFlag entity = static_cast<CEntityFlags::EntityFlag>(entityI);
            CEntityFlags::ReadState state = static_cast<CEntityFlags::ReadState>(stateI);
            if (entity != CEntityFlags::VatsimDataFile || state != CEntityFlags::ReadFinished) { return; }
            Q_UNUSED(number);
            CServerList vatsimFsdServers = sGui->getIContextNetwork()->getVatsimFsdServers();
            if (vatsimFsdServers.isEmpty()) { return; }
            this->ui->cbp_VatsimServer->setServers(vatsimFsdServers);
        }

        void CLoginComponent::setGuiValuesFromAircraft(const CSimulatedAircraft &ownAircraft)
        {
            CAircraftIcaoCode aircraftIcao = ownAircraft.getAircraftIcaoCode();
            this->ui->le_Callsign->setText(ownAircraft.getCallsignAsString());
            this->ui->le_AircraftIcaoDesignator->setText(aircraftIcao.getDesignator());
            this->ui->le_AircraftIcaoAirline->setText(ownAircraft.getAirlineIcaoCodeDesignator());
            this->ui->le_AircraftCombinedType->setText(aircraftIcao.getCombinedType());
        }

        void CLoginComponent::loadFromSettings()
        {
            //! \todo replace with loading from settings when completed
            this->ui->le_Callsign->setText("SWIFT");
            this->ui->le_VatsimId->setText("1288459");
            this->ui->le_VatsimPassword->setText("4769");
            this->ui->le_VatsimHomeAirport->setText("LOWI");
            this->ui->le_VatsimRealName->setText("Black Swift");
        }

        CLoginComponent::CGuiAircraftValues CLoginComponent::getAircraftValuesFromGui() const
        {
            CGuiAircraftValues values;
            values.ownCallsign = this->ui->le_Callsign->text().trimmed().toUpper();
            values.ownAircraftIcaoTypeDesignator = this->ui->le_AircraftIcaoDesignator->text().trimmed().toUpper();
            values.ownAircraftIcaoAirline = this->ui->le_AircraftIcaoAirline->text().trimmed().toUpper();
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
            return user;
        }

        CCallsign CLoginComponent::getCallsignFromGui() const
        {
            CCallsign cs(this->ui->le_Callsign->text().trimmed().toUpper());
            return cs;
        }

        CServer CLoginComponent::getCurrentVatsimServer() const
        {
            return this->ui->cbp_VatsimServer->currentServer();
        }

        CServer CLoginComponent::getCurrentOtherServer() const
        {
            return this->ui->cbp_OtherServers->currentServer();
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

            static const CAircraftModel defaultModel(
                "", CAircraftModel::TypeOwnSimulatorModel, "default model",
                CAircraftIcaoCode("C172", "L1P", "Cessna", "172", "L", true, false, false, 0));

            CAircraftModel model;
            bool simulating = sGui->getIContextSimulator() &&
                              (sGui->getIContextSimulator()->getSimulatorStatus() & ISimulator::Simulating);
            if (simulating)
            {
                model = sGui->getIContextOwnAircraft()->getOwnAircraft().getModel();
                this->ui->le_SimulatorModel->setText(model.getModelString());
            }
            else
            {
                // Set observer mode without simulator
                //! \todo Currently not working in OBS mode
                model = CAircraftModel(defaultModel);
                this->ui->gbp_LoginMode->setLoginMode(INetwork::LoginNormal);
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
            if (!onlyIfEmpty || this->ui->le_AircraftIcaoAirline->text().trimmed().isEmpty())
            {
                this->ui->le_AircraftIcaoAirline->setText(model.getAirlineIcaoCode().getDesignator());
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
            CServerList otherServers(this->m_otherTrafficNetworkServers.get());
            this->ui->cbp_OtherServers->setServers(otherServers);
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
                CLogMessage(this).validationInfo("Reverse lookup for %1") << modelStr;

                // update GUI
                this->setGuiIcaoValues(model, false);
            }
            else
            {
                CLogMessage(this).validationInfo("Reverse lookup for %1 failed, set data manually") << modelStr;
            }
        }

    } // namespace
} // namespace
