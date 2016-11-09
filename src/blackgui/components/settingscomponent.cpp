/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/audiosetupcomponent.h"
#include "blackgui/components/settingscomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"
#include "blackmisc/logmessage.h"
#include "ui_settingscomponent.h"

#include <QComboBox>
#include <QLineEdit>
#include <QSlider>
#include <QString>
#include <QTabBar>
#include <QToolButton>
#include <QtGlobal>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Input;

namespace BlackGui
{
    namespace Components
    {
        CSettingsComponent::CSettingsComponent(QWidget *parent) :
            QTabWidget(parent),
            ui(new Ui::CSettingsComponent)
        {
            ui->setupUi(this);

            this->tabBar()->setExpanding(false);
            this->tabBar()->setUsesScrollButtons(true);
            this->setCurrentIndex(0); // 1st tab
            ui->comp_DataLoadOverview->setVisibleDbRefreshButtons(false);

            this->connect(ui->hs_SettingsGuiAircraftRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedAircraftUpdateInterval);
            this->connect(ui->hs_SettingsGuiAtcRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedAtcStationsUpdateInterval);
            this->connect(ui->hs_SettingsGuiUserRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedUsersUpdateInterval);
            this->connect(ui->comp_SettingsGuiGeneral, &CSettingsGuiComponent::changedWindowsOpacity, this, &CSettingsComponent::changedWindowsOpacity);

            connect(ui->pb_Advanced, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_Audio, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_Gui, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_Hotkeys, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_Network, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_Servers, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_Simulator, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_SimulatorBasics, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_SimulatorMessages, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
        }

        CSettingsComponent::~CSettingsComponent()
        { }

        bool CSettingsComponent::playNotificationSounds() const
        {
            return ui->comp_AudioSetup->playNotificationSounds();
        }

        int CSettingsComponent::getAtcUpdateIntervalSeconds() const
        {
            return ui->hs_SettingsGuiAtcRefreshTime->value();
        }

        int CSettingsComponent::getAircraftUpdateIntervalSeconds() const
        {
            return ui->hs_SettingsGuiAircraftRefreshTime->value();
        }

        int CSettingsComponent::getUsersUpdateIntervalSeconds() const
        {
            return ui->hs_SettingsGuiUserRefreshTime->value();
        }

        void CSettingsComponent::setSettingsTab(CSettingsComponent::SettingTab tab)
        {
            this->setCurrentIndex(static_cast<int>(tab));
        }

        void CSettingsComponent::setGuiOpacity(double value)
        {
            ui->comp_SettingsGuiGeneral->setGuiOpacity(value);
        }

        void CSettingsComponent::ps_overviewButtonClicked()
        {
            const QObject *sender = QObject::sender();
            if (sender == ui->pb_Advanced) { this->setCurrentIndex(SettingTabAdvanced); return; }
            if (sender == ui->pb_Audio) { this->setCurrentIndex(SettingTabAudio); return; }
            if (sender == ui->pb_Gui) { this->setCurrentIndex(SettingTabGui); return; }
            if (sender == ui->pb_Hotkeys) { this->setCurrentIndex(SettingTabHotkeys); return; }
            if (sender == ui->pb_Network) { this->setCurrentIndex(SettingTabNetwork); return; }
            if (sender == ui->pb_Servers) { this->setCurrentIndex(SettingTabServers); return; }
            if (sender == ui->pb_Simulator) { this->setCurrentIndex(SettingTabSimulator); return; }
            if (sender == ui->pb_SimulatorBasics) { this->setCurrentIndex(SettingTabSimulatorBasics); return; }
            if (sender == ui->pb_SimulatorMessages) { this->setCurrentIndex(SettingTabSimulatorMessages); return; }
            this->setCurrentIndex(SettingTabOverview);
        }
    }
} // namespace
