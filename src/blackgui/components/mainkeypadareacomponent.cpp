/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/context/contextapplication.h"
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
#include <QPointer>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
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
            connect(ui->pb_MainAircrafts,     &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_MainAtc,           &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_MainCockpit,       &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_MainFlightplan,    &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_MainLog,           &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_MainMappings,      &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_MainInterpolation, &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_MainRadar,         &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_MainSettings,      &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_MainSimulator,     &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_MainTextMessages,  &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_MainUsers,         &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_MainWeather,       &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);

            // non info areas
            connect(ui->pb_Connect,        &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_Opacity050,     &QPushButton::pressed,  this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_Opacity100,     &QPushButton::pressed,  this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_SoundMaxVolume, &QPushButton::pressed,  this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_CockpitIdent,   &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_SoundMute,      &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);
            connect(ui->pb_Audio,          &QPushButton::released, this, &CMainKeypadAreaComponent::buttonSelected);

            // command line
            ui->lep_CommandLineInput->setIdentifier(m_identifier);
            connect(ui->lep_CommandLineInput, &CCommandInput::commandEntered, this, &CMainKeypadAreaComponent::commandEntered);
            connect(ui->lep_CommandLineInput, &CCommandInput::textEntered,    this, &CMainKeypadAreaComponent::textEntered);

            connect(sGui->getIContextNetwork(),     &IContextNetwork::connectionStatusChanged, this, &CMainKeypadAreaComponent::connectionStatusChanged);
            connect(sGui->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CMainKeypadAreaComponent::ownAircraftCockpitChanged);
            connect(sGui->getCContextAudioBase(),   &CContextAudioBase::changedMute, this, &CMainKeypadAreaComponent::muteChanged);
            connect(this, &CMainKeypadAreaComponent::commandEntered, sGui->getCoreFacade(), &CCoreFacade::parseCommandLine);

            QPointer<CMainKeypadAreaComponent> myself(this);
            QTimer::singleShot(5000, this, [ = ]
            {
                if (!myself || !sGui) { return; }
                this->update();
            });
        }

        CMainKeypadAreaComponent::~CMainKeypadAreaComponent()
        { }

        void CMainKeypadAreaComponent::onMainInfoAreaChanged(int currentTabIndex, const QList<int> &dockedIndexes, const QList<int> &floatingIndexes)
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

            // check the floating
            for (int floatingIndex : floatingIndexes)
            {
                QPushButton *pb = this->mainInfoAreaToButton(static_cast<CMainInfoAreaComponent::InfoArea>(floatingIndex));
                if (pb) { pb->setChecked(true); }
            }

            this->updateConnectionStatus();

            Q_UNUSED(dockedIndexes)
        }

        void CMainKeypadAreaComponent::focusInEntryField()
        {
            ui->lep_CommandLineInput->setFocus();
        }

        void CMainKeypadAreaComponent::buttonSelected()
        {
            if (!sGui || sGui->isShuttingDown()) { return; }
            QPushButton *senderButton = static_cast<QPushButton *>(QObject::sender());
            Q_ASSERT_X(senderButton, Q_FUNC_INFO, "No sender button");
            if (!senderButton) { return; }
            const CMainInfoAreaComponent::InfoArea infoArea = buttonToMainInfoArea(senderButton);
            if (infoArea != CMainInfoAreaComponent::InfoAreaNone)
            {
                Q_ASSERT(senderButton->isCheckable());
                emit this->selectedMainInfoAreaDockWidget(infoArea);
                senderButton->setChecked(true); // re-check if got unchecked, we use checked buttons like normal buttons
                return;
            }
            else if (senderButton == ui->pb_CockpitIdent && sGui->getIContextOwnAircraft())
            {
                emit this->identPressed();
            }
            else if (senderButton == ui->pb_Opacity050)
            {
                emit this->changedOpacity(50);
            }
            else if (senderButton == ui->pb_Opacity100)
            {
                emit this->changedOpacity(100);
            }
            else if (senderButton == ui->pb_SoundMaxVolume && sGui->getIContextAudio())
            {
                sGui->getCContextAudioBase()->setMasterOutputVolume(100);
            }
            else if (senderButton == ui->pb_SoundMute && sGui->getIContextAudio())
            {
                const bool mute = sGui->getCContextAudioBase()->isMuted();
                sGui->getCContextAudioBase()->setMute(!mute);
            }
            else if (senderButton == ui->pb_Connect)
            {
                emit this->connectPressed();
                this->updateConnectionStatus();
            }
            else if (senderButton == ui->pb_Audio)
            {
                emit this->audioPressed();
            }
        }

        void CMainKeypadAreaComponent::connectionStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
        {
            Q_UNUSED(from)

            // Connected button
            if (to.isConnected())
            {
                ui->pb_Connect->setText("Connected");
                ui->pb_Connect->setChecked(true);
                // moved to stylesheet: ui->pb_Connect->setStyleSheet("background-color: green");
            }
            else
            {
                ui->pb_Connect->setText("Connect");
                ui->pb_Connect->setChecked(false);
                // moved to stylesheet: ui->pb_Connect->setStyleSheet("background-color: ");
            }
        }

        void CMainKeypadAreaComponent::ownAircraftCockpitChanged(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
        {
            Q_UNUSED(originator)
            bool ident = aircraft.getTransponder().getTransponderMode() == CTransponder::StateIdent;

            //check state to avoid undelibarate signals
            if (ident != ui->pb_CockpitIdent->isChecked())
            {
                ui->pb_CockpitIdent->setChecked(ident);
            }
        }

        void CMainKeypadAreaComponent::muteChanged(bool muted)
        {
            //check state to avoid undelibarate signals
            if (muted != ui->pb_SoundMute->isChecked())
            {
                ui->pb_SoundMute->setChecked(muted);
            }
        }

        CMainInfoAreaComponent::InfoArea CMainKeypadAreaComponent::buttonToMainInfoArea(const QObject *button) const
        {
            if (button == ui->pb_MainAircrafts) return CMainInfoAreaComponent::InfoAreaAircraft;
            if (button == ui->pb_MainAtc) return CMainInfoAreaComponent::InfoAreaAtc;
            if (button == ui->pb_MainCockpit) return CMainInfoAreaComponent::InfoAreaCockpit;
            if (button == ui->pb_MainFlightplan) return CMainInfoAreaComponent::InfoAreaFlightPlan;
            if (button == ui->pb_MainLog) return CMainInfoAreaComponent::InfoAreaLog;
            if (button == ui->pb_MainMappings) return CMainInfoAreaComponent::InfoAreaMapping;
            if (button == ui->pb_MainInterpolation) return CMainInfoAreaComponent::InfoAreaInterpolation;
            if (button == ui->pb_MainRadar) return CMainInfoAreaComponent::InfoAreaRadar;
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
            case CMainInfoAreaComponent::InfoAreaMapping: return ui->pb_MainMappings;
            case CMainInfoAreaComponent::InfoAreaInterpolation: return ui->pb_MainInterpolation;
            case CMainInfoAreaComponent::InfoAreaRadar: return ui->pb_MainRadar;
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
            ui->pb_MainInterpolation->setChecked(false);
            ui->pb_MainRadar->setChecked(false);
            ui->pb_MainSettings->setChecked(false);
            ui->pb_MainSimulator->setChecked(false);
            ui->pb_MainTextMessages->setChecked(false);
            ui->pb_MainUsers->setChecked(false);
            ui->pb_MainWeather->setChecked(false);

            this->updateConnectionStatus();
        }

        void CMainKeypadAreaComponent::update()
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->supportsContexts()) { return; }
            if (sGui->getCContextAudioBase())
            {
                this->muteChanged(sGui->getCContextAudioBase()->isMuted());
            }
            this->updateConnectionStatus();
        }

        void CMainKeypadAreaComponent::updateConnectionStatus()
        {
            if (sGui->getIContextNetwork() && sGui->getIContextNetwork()->isConnected())
            {
                this->connectionStatusChanged(CConnectionStatus::Connected, CConnectionStatus::Connected);
            }
            else
            {
                this->connectionStatusChanged(CConnectionStatus::Disconnected, CConnectionStatus::Disconnected);
            }
        }
    } // namespace
} // namespace
