// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/audionotificationcomponent.h"
#include "blackgui/components/settingscomponent.h"
#include "blackgui/guiapplication.h"
#include "blackconfig/buildconfig.h"
#include "ui_settingscomponent.h"

#include <QAction>
#include <QComboBox>
#include <QLineEdit>
#include <QTabBar>
#include <QToolButton>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Input;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Settings;
using namespace BlackCore;
using namespace BlackGui;
using namespace BlackConfig;

namespace BlackGui::Components
{
    CSettingsComponent::CSettingsComponent(QWidget *parent) : QTabWidget(parent),
                                                              ui(new Ui::CSettingsComponent)
    {
        ui->setupUi(this);

        this->tabBar()->setExpanding(false);
        this->tabBar()->setUsesScrollButtons(true);
        this->setCurrentIndex(0); // 1st tab
        ui->comp_DataLoadOverview->showVisibleDbRefreshButtons(CBuildConfig::isDebugBuild() || sGui->isDeveloperFlagSet());
        ui->comp_DataLoadOverview->showVisibleLoadAllButtons(false, false, false);

        connect(ui->comp_SettingsGuiGeneral, &CSettingsGuiComponent::changedWindowsOpacity, this, &CSettingsComponent::changedWindowsOpacity);
        connect(ui->pb_Advanced, &QPushButton::released, this, &CSettingsComponent::onOverviewButtonClicked);
        connect(ui->pb_Audio, &QPushButton::released, this, &CSettingsComponent::onOverviewButtonClicked);
        connect(ui->pb_Gui, &QPushButton::released, this, &CSettingsComponent::onOverviewButtonClicked);
        connect(ui->pb_Hotkeys, &QPushButton::released, this, &CSettingsComponent::onOverviewButtonClicked);
        connect(ui->pb_Network, &QPushButton::released, this, &CSettingsComponent::onOverviewButtonClicked);
        connect(ui->pb_Servers, &QPushButton::released, this, &CSettingsComponent::onOverviewButtonClicked);
        connect(ui->pb_Simulator, &QPushButton::released, this, &CSettingsComponent::onOverviewButtonClicked);
        connect(ui->pb_SimulatorBasics, &QPushButton::released, this, &CSettingsComponent::onOverviewButtonClicked);
        connect(ui->pb_SimulatorMessages, &QPushButton::released, this, &CSettingsComponent::onOverviewButtonClicked);
        connect(ui->pb_Matching, &QPushButton::released, this, &CSettingsComponent::onOverviewButtonClicked);
        connect(ui->pb_DataLoadAndCaches, &QPushButton::released, this, &CSettingsComponent::onOverviewButtonClicked);
    }

    CSettingsComponent::~CSettingsComponent()
    {}

    bool CSettingsComponent::playNotificationSounds() const
    {
        return ui->comp_AudioNotification->playNotificationSounds();
    }

    CSpecializedSimulatorSettings CSettingsComponent::getSimulatorSettings(const CSimulatorInfo &simulator) const
    {
        return ui->comp_SettingsSimulatorBasics->getSimulatorSettings(simulator);
    }

    void CSettingsComponent::setTab(CSettingsComponent::SettingTab tab)
    {
        this->setCurrentIndex(static_cast<int>(tab));
    }

    void CSettingsComponent::setSettingsOverviewTab()
    {
        this->setTab(SettingTabOverview);
    }

    void CSettingsComponent::setGuiOpacity(double value)
    {
        ui->comp_SettingsGuiGeneral->setGuiOpacity(value);
    }

    void CSettingsComponent::onOverviewButtonClicked()
    {
        const QObject *sender = QObject::sender();
        if (sender == ui->pb_Advanced)
        {
            this->setCurrentIndex(SettingTabAdvanced);
            return;
        }
        if (sender == ui->pb_Audio)
        {
            this->setCurrentIndex(SettingTabAudio);
            return;
        }
        if (sender == ui->pb_Gui)
        {
            this->setCurrentIndex(SettingTabGui);
            return;
        }
        if (sender == ui->pb_Hotkeys)
        {
            this->setCurrentIndex(SettingTabHotkeys);
            return;
        }
        if (sender == ui->pb_Network)
        {
            this->setCurrentIndex(SettingTabNetwork);
            return;
        }
        if (sender == ui->pb_Servers)
        {
            this->setCurrentIndex(SettingTabServers);
            return;
        }
        if (sender == ui->pb_Simulator)
        {
            this->setCurrentIndex(SettingTabSimulator);
            return;
        }
        if (sender == ui->pb_SimulatorBasics)
        {
            this->setCurrentIndex(SettingTabSimulatorBasics);
            return;
        }
        if (sender == ui->pb_DataLoadAndCaches)
        {
            this->setCurrentIndex(SettingTabDataAndCaches);
            return;
        }
        if (sender == ui->pb_SimulatorMessages)
        {
            this->setCurrentIndex(SettingTabSimulatorMessages);
            return;
        }
        if (sender == ui->pb_Matching)
        {
            this->setCurrentIndex(SettingTabMatching);
            return;
        }
        this->setCurrentIndex(SettingTabOverview);
    }
} // namespace
