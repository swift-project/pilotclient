/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/corefacade.h"
#include "blackgui/components/mainkeypadareacomponent.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "ui_mainkeypadareacomponent.h"

#include <QLineEdit>
#include <QPushButton>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackCore;
using namespace BlackCore::Context;

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
            // pressed collides, as this toggles button again
            // using toggle collides, as checking/unchecking toggles again -> infinite loop
            connect(this->ui->pb_MainAircrafts, &QPushButton::released, this, &CMainKeypadAreaComponent::ps_buttonSelected);
            connect(this->ui->pb_MainAtc, &QPushButton::released, this, &CMainKeypadAreaComponent::ps_buttonSelected);
            connect(this->ui->pb_MainCockpit, &QPushButton::released, this, &CMainKeypadAreaComponent::ps_buttonSelected);
            connect(this->ui->pb_MainFlightplan, &QPushButton::released, this, &CMainKeypadAreaComponent::ps_buttonSelected);
            connect(this->ui->pb_MainLog, &QPushButton::released, this, &CMainKeypadAreaComponent::ps_buttonSelected);
            connect(this->ui->pb_MainMappings, &QPushButton::released, this, &CMainKeypadAreaComponent::ps_buttonSelected);
            connect(this->ui->pb_MainSettings, &QPushButton::released, this, &CMainKeypadAreaComponent::ps_buttonSelected);
            connect(this->ui->pb_MainSimulator, &QPushButton::released, this, &CMainKeypadAreaComponent::ps_buttonSelected);
            connect(this->ui->pb_MainTextMessages, &QPushButton::released, this, &CMainKeypadAreaComponent::ps_buttonSelected);
            connect(this->ui->pb_MainUsers, &QPushButton::released, this, &CMainKeypadAreaComponent::ps_buttonSelected);
            connect(this->ui->pb_MainWeather, &QPushButton::released, this, &CMainKeypadAreaComponent::ps_buttonSelected);

            // non info areas
            connect(this->ui->pb_Connect, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonSelected);
            connect(this->ui->pb_CockpitIdent, &QPushButton::released, this, &CMainKeypadAreaComponent::ps_buttonSelected);
            connect(this->ui->pb_Opacity050, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonSelected);
            connect(this->ui->pb_Opacity100, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonSelected);
            connect(this->ui->pb_SoundMaxVolume, &QPushButton::pressed, this, &CMainKeypadAreaComponent::ps_buttonSelected);
            connect(this->ui->pb_SoundMute, &QPushButton::released, this, &CMainKeypadAreaComponent::ps_buttonSelected);

            // command line
            this->connect(this->ui->le_CommandLineInput, &QLineEdit::returnPressed, this, &CMainKeypadAreaComponent::ps_commandEntered);

            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CMainKeypadAreaComponent::ps_connectionStatusChanged);
            connect(sGui->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CMainKeypadAreaComponent::ps_ownAircraftCockpitChanged);
            connect(sGui->getIContextAudio(), &IContextAudio::changedMute, this, &CMainKeypadAreaComponent::ps_muteChanged);
            connect(this, &CMainKeypadAreaComponent::commandEntered, sGui->getCoreFacade(), &CCoreFacade::parseCommandLine);
        }

        CMainKeypadAreaComponent::~CMainKeypadAreaComponent()
        { }

        void CMainKeypadAreaComponent::onMainInfoAreaChanged(int currentTabIndex, QList<int> dockedIndexes, QList<int> floatingIndexes)
        {
            this->unsetInfoAreaButtons();
            if (currentTabIndex >= 0)
            {
                QPushButton *pb = this->mainInfoAreaToButton(static_cast<CMainInfoAreaComponent::InfoArea>(currentTabIndex));
                if (pb)
                {
                    Q_ASSERT(pb->isCheckable());
                    pb->setChecked(true);
                }
            }

            foreach (int floatingIndex, floatingIndexes)
            {
                QPushButton *pb = this->mainInfoAreaToButton(static_cast<CMainInfoAreaComponent::InfoArea>(floatingIndex));
                if (pb) {pb->setChecked(true); }
            }

            Q_UNUSED(dockedIndexes);
        }

        void CMainKeypadAreaComponent::ps_buttonSelected()
        {
            QPushButton *senderButton = static_cast<QPushButton *>(QObject::sender());
            Q_ASSERT(senderButton);
            if (!senderButton) { return; }
            CMainInfoAreaComponent::InfoArea ia = buttonToMainInfoArea(senderButton);
            if (ia != CMainInfoAreaComponent::InfoAreaNone)
            {
                Q_ASSERT(senderButton->isCheckable());
                emit selectedMainInfoAreaDockWidget(ia);
                senderButton->setChecked(true); // re-check if got unchecked, we use checked buttons like normal buttons
                return;
            }
            else if (senderButton == this->ui->pb_CockpitIdent && sGui->getIContextOwnAircraft())
            {
                emit identPressed();
            }
            else if (senderButton == this->ui->pb_Opacity050)
            {
                emit changedOpacity(50);
            }
            else if (senderButton == this->ui->pb_Opacity100)
            {
                emit changedOpacity(100);
            }
            else if (senderButton == this->ui->pb_SoundMaxVolume && sGui->getIContextAudio())
            {
                sGui->getIContextAudio()->setVoiceOutputVolume(100);
            }
            else if (senderButton == this->ui->pb_SoundMute && sGui->getIContextAudio())
            {
                bool mute = sGui->getIContextAudio()->isMuted();
                sGui->getIContextAudio()->setMute(!mute);
            }
            else if (senderButton == this->ui->pb_Connect)
            {
                emit connectPressed();
            }
        }

        void CMainKeypadAreaComponent::ps_connectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);

            // Connected button
            if (to == INetwork::Connected)
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
            QString c(this->ui->le_CommandLineInput->text().trimmed().simplified());
            if (c.isEmpty()) { return; }
            emit this->commandEntered(c, keypadIdentifier());
            this->ui->le_CommandLineInput->clear();
        }

        void CMainKeypadAreaComponent::ps_ownAircraftCockpitChanged(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
        {
            Q_UNUSED(originator);
            bool ident = aircraft.getTransponder().getTransponderMode() == CTransponder::StateIdent;

            //check state to avoid undelibarate signals
            if (ident != this->ui->pb_CockpitIdent->isChecked())
            {
                this->ui->pb_CockpitIdent->setChecked(ident);
            }
        }

        void CMainKeypadAreaComponent::ps_muteChanged(bool muted)
        {
            //check state to avoid undelibarate signals
            if (muted != this->ui->pb_SoundMute->isChecked())
            {
                this->ui->pb_SoundMute->setChecked(muted);
            }
        }

        CMainInfoAreaComponent::InfoArea CMainKeypadAreaComponent::buttonToMainInfoArea(const QObject *button) const
        {
            if (button == ui->pb_MainAircrafts) return CMainInfoAreaComponent::InfoAreaAircraft;
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

        QPushButton *CMainKeypadAreaComponent::mainInfoAreaToButton(CMainInfoAreaComponent::InfoArea area) const
        {
            switch (area)
            {
            case CMainInfoAreaComponent::InfoAreaAircraft: return ui->pb_MainAircrafts;
            case CMainInfoAreaComponent::InfoAreaAtc: return ui->pb_MainAtc;
            case CMainInfoAreaComponent::InfoAreaCockpit: return ui->pb_MainCockpit;
            case CMainInfoAreaComponent::InfoAreaFlightPlan: return ui->pb_MainFlightplan;
            case CMainInfoAreaComponent::InfoAreaLog: return ui->pb_MainLog;
            case CMainInfoAreaComponent::InfoAreaMappings: return ui->pb_MainMappings;
            case CMainInfoAreaComponent::InfoAreaSettings: return ui->pb_MainSettings;
            case CMainInfoAreaComponent::InfoAreaSimulator: return ui->pb_MainSimulator;
            case CMainInfoAreaComponent::InfoAreaTextMessages: return ui->pb_MainTextMessages;
            case CMainInfoAreaComponent::InfoAreaUsers: return ui->pb_MainUsers;
            case CMainInfoAreaComponent::InfoAreaWeather: return ui->pb_MainWeather;
            default: break;
            }
            return nullptr;
        }

        void CMainKeypadAreaComponent::unsetInfoAreaButtons()
        {
            ui->pb_MainAircrafts->setChecked(false);
            ui->pb_MainAtc->setChecked(false);
            ui->pb_MainCockpit->setChecked(false);
            ui->pb_MainFlightplan->setChecked(false);
            ui->pb_MainLog->setChecked(false);
            ui->pb_MainMappings->setChecked(false);
            ui->pb_MainSettings->setChecked(false);
            ui->pb_MainSimulator->setChecked(false);
            ui->pb_MainTextMessages->setChecked(false);
            ui->pb_MainUsers->setChecked(false);
            ui->pb_MainWeather->setChecked(false);
        }

        CIdentifier CMainKeypadAreaComponent::keypadIdentifier()
        {
            if (m_identifier.getName().isEmpty())
            {
                m_identifier = CIdentifier(QStringLiteral("KEYPADAREACOMPONENT"));
            }
            return m_identifier;
        }

    } // namespace
} // namespace
