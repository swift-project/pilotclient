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
#include "blackcore/context_network.h"
#include "blackcore/context_settings.h"
#include "blackcore/context_ownaircraft.h"
#include "blackcore/context_audio.h"
#include "blackcore/network.h"
#include "blackmisc/logmessage.h"
#include "../uppercasevalidator.h"
#include <QIntValidator>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackCore;
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
            this->ui->le_Callsign->setMaxLength(10);
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

            // server GUI element
            this->ui->frp_CurrentServer->setReadOnly(true);
            this->ui->frp_CurrentServer->showPasswordField(false);
        }

        CLoginComponent::~CLoginComponent()
        { }

        void CLoginComponent::mainInfoAreaChanged(const QWidget *currentWidget)
        {
            if (currentWidget != this && currentWidget != this->parentWidget())
            {
                this->m_visible = false;
            }
            else
            {

                if (this->m_visible)
                {
                    // re-trigger! treat as same as OK
                    this->ps_toggleNetworkConnection();
                }
                else
                {

                    this->m_visible = true;
                    bool isConnected = this->getIContextNetwork()->isConnected();
                    this->setGuiVisibility(isConnected);
                    this->setOkButtonString(isConnected);
                }
            }
        }

        void CLoginComponent::runtimeHasBeenSet()
        {
            Q_ASSERT(getIContextNetwork());
            Q_ASSERT(getIContextSettings());
            connect(getIContextNetwork(), &IContextNetwork::vatsimDataFileRead, this, &CLoginComponent::ps_onVatsimDataFileLoaded);
            connect(getIContextSettings(), &IContextSettings::changedSettings, this, &CLoginComponent::ps_onSettingsChanged);

            // inital setup, if data already available
            ps_validateAircraftValues();
            ps_validateVatsimValues();
            ps_onVatsimDataFileLoaded();
            CServerList otherServers = this->getIContextSettings()->getNetworkSettings().getTrafficNetworkServers();
            this->ui->cbp_OtherServers->setServers(otherServers);
        }

        void CLoginComponent::ps_loginCancelled()
        {
            emit loginOrLogoffCancelled();
        }

        void CLoginComponent::ps_toggleNetworkConnection()
        {
            Q_ASSERT(this->getIContextNetwork());
            Q_ASSERT(this->getIContextOwnAircraft());
            Q_ASSERT(this->getIContextAudio());

            bool isConnected = this->getIContextNetwork()->isConnected();
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

                CGuiAircraftValues aircraftValues = this->getAircraftValuesFromGui();
                CAircraft ownAircraft = this->getIContextOwnAircraft()->getOwnAircraft();
                CAircraftIcao icao = ownAircraft.getIcaoInfo();
                icao.setAircraftDesignator(aircraftValues.ownAircraftIcaoTypeDesignator);
                icao.setAirlineDesignator(aircraftValues.ownAircraftIcaoAirline);
                icao.setAircraftCombinedType(aircraftValues.ownAircraftCombinedType);
                ownAircraft.setIcaoInfo(icao);
                ownAircraft.setCallsign(aircraftValues.ownCallsign);

                // set latest ICAO, callsign
                this->getIContextOwnAircraft()->updateIcaoData(ownAircraft.getIcaoInfo(), loginOriginator());
                this->getIContextOwnAircraft()->updateCallsign(ownAircraft.getCallsign(), loginOriginator());

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

                // Login
                msg = this->getIContextNetwork()->connectToNetwork(currentServer, static_cast<uint>(mode));
            }
            else
            {
                // disconnect from network
                this->getIContextAudio()->leaveAllVoiceRooms();
                msg = this->getIContextNetwork()->disconnectFromNetwork();
            }

            // log message and trigger events
            CLogMessage(this).statusMessage(msg);
            if (msg.isSeverityInfoOrLess())
            {
                emit loginOrLogoffSuccessful();
            }
            else
            {
                emit loginOrLogoffCancelled();
            }

        }

        void CLoginComponent::ps_onVatsimDataFileLoaded()
        {
            Q_ASSERT(getIContextNetwork());
            Q_ASSERT(getIContextSettings());
            CServerList vatsimFsdServers = this->getIContextNetwork()->getVatsimFsdServers();
            if (vatsimFsdServers.isEmpty()) { return; }
            this->ui->cbp_VatsimServer->setServers(vatsimFsdServers);
        }

        void CLoginComponent::setGuiValuesFromAircraft(const CAircraft &ownAircraft)
        {
            CAircraftIcao icao = ownAircraft.getIcaoInfo();
            this->ui->le_Callsign->setText(ownAircraft.getCallsignAsString());
            this->ui->le_AircraftIcaoDesignator->setText(icao.getAircraftDesignator());
            this->ui->le_AircraftIcaoAirline->setText(icao.getAirlineDesignator());
            this->ui->le_AircraftCombinedType->setText(icao.getAircraftCombinedType());
        }

        void CLoginComponent::loadFromSettings()
        {
            //! \todo replace with loading from settings when completed
            this->ui->le_Callsign->setText("BLACK");
            this->ui->le_AircraftIcaoDesignator->setText("C172");
            this->ui->le_AircraftIcaoAirline->setText("GA");
            this->ui->le_AircraftCombinedType->setText("L1P");
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
            values.vatsimPassword = this->ui->le_VatsimPassword->text().trimmed().toUpper();
            values.vatsimRealName = this->ui->le_VatsimRealName->text().simplified().trimmed().toUpper();
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
            this->ui->gb_CurrentServer->setVisible(connected);
        }

        bool CLoginComponent::ps_validateAircraftValues()
        {
            CGuiAircraftValues values = getAircraftValuesFromGui();

            bool validCombinedType = CAircraftIcao::isValidCombinedType(values.ownAircraftCombinedType);
            this->ui->lblp_AircraftCombinedType->setTicked(validCombinedType);

            bool validAirlineDesignator = values.ownAircraftIcaoAirline.isEmpty() || CAircraftIcao::isValidAirlineDesignator(values.ownAircraftIcaoAirline);
            this->ui->lblp_AircraftIcaoAirline->setTicked(validAirlineDesignator);

            bool validIcaoDesignator = CAircraftIcao::isValidDesignator(values.ownAircraftIcaoTypeDesignator);
            this->ui->lblp_AircraftIcaoDesignator->setTicked(validIcaoDesignator);

            bool validCallsign = CCallsign::isValidCallsign(values.ownCallsign);
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

            bool validHomeAirport = values.vatsimHomeAirport.isEmpty() || CAirportIcao::isValidIcaoDesignator(values.vatsimHomeAirport);
            this->ui->lblp_VatsimHomeAirport->setTicked(validHomeAirport);

            bool validVatsimPassword = !values.vatsimPassword.isEmpty();
            this->ui->lblp_VatsimPassword->setTicked(validVatsimPassword);

            bool validRealUserName = !values.vatsimRealName.isEmpty();
            this->ui->lblp_VatsimRealName->setTicked(validRealUserName);

            return validVatsimId && validHomeAirport && validVatsimPassword && validRealUserName;
        }

        void CLoginComponent::ps_onSettingsChanged(uint settingsType)
        {
            if (settingsType != static_cast<uint>(IContextSettings::SettingsNetwork)) { return; }
            CServerList otherServers = this->getIContextSettings()->getNetworkSettings().getTrafficNetworkServers();
            this->ui->cbp_OtherServers->setServers(otherServers);
        }

        const QString &CLoginComponent::loginOriginator()
        {
            // string is generated once, the timestamp allows to use multiple
            // components (as long as they are not generated at the same ms)
            static const QString o = QString("LOGINCOMCOMPONENT:").append(QString::number(QDateTime::currentMSecsSinceEpoch()));
            return o;
        }

    } // namespace
} // namespace
