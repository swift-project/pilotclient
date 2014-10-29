/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "mainkeypadareacomponent.h"
#include "ui_mainkeypadareacomponent.h"
#include "blackcore/context_audio.h"

using namespace BlackMisc::Aviation;
using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {

        CMainKeypadAreaComponent::CMainKeypadAreaComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CMainKeypadAreaComponent)
        {
            ui->setupUi(this);

            // Info areas
            connect(this->ui->pb_MainAircrafts, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);
            connect(this->ui->pb_MainAtc, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);
            connect(this->ui->pb_MainCockpit, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);
            connect(this->ui->pb_MainFlightplan, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);
            connect(this->ui->pb_MainLog, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);
            connect(this->ui->pb_MainMappings, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);
            connect(this->ui->pb_MainSettings, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);
            connect(this->ui->pb_MainSimulator, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);
            connect(this->ui->pb_MainTextMessages, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);
            connect(this->ui->pb_MainUsers, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);
            connect(this->ui->pb_MainWeather, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);

            // non info areas
            connect(this->ui->pb_Connect, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);
            connect(this->ui->pb_CockpitIdent, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);
            connect(this->ui->pb_Opacity050, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);
            connect(this->ui->pb_Opacity100, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);
            connect(this->ui->pb_SoundMaxVolume, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);
            connect(this->ui->pb_SoundMute, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonPressed);

            // command line
            this->connect(this->ui->le_CommandLineInput, &QLineEdit::returnPressed, this, &CMainKeypadAreaComponent::ps_commandEntered);
        }

        CMainKeypadAreaComponent::~CMainKeypadAreaComponent()
        { }

        void CMainKeypadAreaComponent::runtimeHasBeenSet()
        {
            Q_ASSERT(this->getIContextOwnAircraft());
            Q_ASSERT(this->getIContextNetwork());
            connect(this->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CMainKeypadAreaComponent::ps_connectionStatusChanged);
            connect(this, &CMainKeypadAreaComponent::commandEntered, this->getIContextNetwork(), &IContextNetwork::parseCommandLine);
            connect(this->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CMainKeypadAreaComponent::ps_ownAircraftCockpitChanged);
            connect(this, &CMainKeypadAreaComponent::commandEntered, this->getIContextOwnAircraft(), &IContextOwnAircraft::parseCommandLine);
        }

        void CMainKeypadAreaComponent::ps_buttonPressed()
        {
            const QObject *sender = QObject::sender();
            CMainInfoAreaComponent::InfoArea ia = buttonToMainInfoArea(sender);
            if (ia != CMainInfoAreaComponent::InfoAreaNone)
            {
                emit selectedMainInfoAreaDockWidget(ia);
                return;
            }
            else if (sender == this->ui->pb_CockpitIdent && this->getIContextOwnAircraft())
            {
                emit identPressed();
            }
            else if (sender == this->ui->pb_Opacity050)
            {
                emit changedOpacity(50);
            }
            else if (sender == this->ui->pb_Opacity100)
            {
                emit changedOpacity(100);
            }
            else if (sender == this->ui->pb_SoundMaxVolume && this->getIContextAudio())
            {
                this->getIContextAudio()->setVolumes(100, 100);
            }
            else if (sender == this->ui->pb_SoundMute && this->getIContextAudio())
            {
                bool mute = this->getIContextAudio()->isMuted();
                this->getIContextAudio()->setMute(!mute);
            }
            else if (sender == this->ui->pb_Connect)
            {
                emit connectPressed();
            }
        }

        void CMainKeypadAreaComponent::ps_connectionStatusChanged(uint from, uint to, const QString &message)
        {
            INetwork::ConnectionStatus statusFrom = static_cast<INetwork::ConnectionStatus>(from);
            INetwork::ConnectionStatus statusTo = static_cast<INetwork::ConnectionStatus>(to);

            Q_UNUSED(statusFrom);
            Q_UNUSED(message);

            // Connected button
            if (statusTo == INetwork::Connected)
            {
                this->ui->pb_Connect->setText("Disconnect");
                this->ui->pb_Connect->setStyleSheet("background-color: green");
            }
            else
            {
                this->ui->pb_Connect->setText("Connect");
                this->ui->pb_Connect->setStyleSheet("background-color: ");
            }
        }

        void CMainKeypadAreaComponent::ps_commandEntered()
        {
            QString c = this->ui->le_CommandLineInput->text().trimmed();
            if (c.isEmpty()) return;
            emit this->commandEntered(c);
            this->ui->le_CommandLineInput->clear();
        }

        void CMainKeypadAreaComponent::ps_ownAircraftCockpitChanged(const CAircraft &aircraft, const QString &originator)
        {
            Q_UNUSED(originator);
            if (aircraft.getTransponder().getTransponderMode() == CTransponder::StateIdent)
            {
                this->ui->pb_CockpitIdent->setStyleSheet("background-color: yellow");
            }
            else
            {
                this->ui->pb_CockpitIdent->setStyleSheet("");
            }
        }

        CMainInfoAreaComponent::InfoArea CMainKeypadAreaComponent::buttonToMainInfoArea(const QObject *button) const
        {
            if (button == ui->pb_MainAircrafts) return CMainInfoAreaComponent::InfoAreaAircrafts;
            if (button == ui->pb_MainAtc) return CMainInfoAreaComponent::InfoAreaAtc;
            if (button == ui->pb_MainCockpit) return CMainInfoAreaComponent::InfoAreaCockpit;
            if (button == ui->pb_MainFlightplan) return CMainInfoAreaComponent::InfoAreaFlightPlan;
            if (button == ui->pb_MainLog) return CMainInfoAreaComponent::InfoAreaLog;
            if (button == ui->pb_MainMappings) return CMainInfoAreaComponent::InfoAreaMappings;
            if (button == ui->pb_MainSettings) return CMainInfoAreaComponent::InfoAreaSettings;
            if (button == ui->pb_MainSimulator) return CMainInfoAreaComponent::InfoAreaSimulator;
            if (button == ui->pb_MainTextMessages) return CMainInfoAreaComponent::InfoAreaTextMessages;
            if (button == ui->pb_MainUsers) return CMainInfoAreaComponent::InfoAreaUsers;
            if (button == ui->pb_MainWeather) return CMainInfoAreaComponent::InfoAreaWeather;
            return CMainInfoAreaComponent::InfoAreaNone;
        }

        Aviation::CAircraft CMainKeypadAreaComponent::getOwnAircraft() const
        {
            if (!this->getIContextOwnAircraft()) { return CAircraft(); }
            return this->getIContextOwnAircraft()->getOwnAircraft();
        }

    } // namespace
} // namespace
