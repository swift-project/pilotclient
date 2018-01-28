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
#include "blackconfig/buildconfig.h"
#include "blackmisc/logmessage.h"
#include "ui_settingscomponent.h"

#include <QComboBox>
#include <QLineEdit>
#include <QSlider>
#include <QString>
#include <QTabBar>
#include <QToolButton>
#include <QtGlobal>
#include <QKeySequence>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Input;
using namespace BlackCore;
using namespace BlackGui;
using namespace BlackConfig;

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
            ui->comp_DataLoadOverview->showVisibleDbRefreshButtons(CBuildConfig::isDebugBuild() || sGui->isDeveloperFlagSet());
            ui->comp_DataLoadOverview->showVisibleLoadAllButtons(false, false);

            connect(ui->comp_SettingsGuiGeneral, &CSettingsGuiComponent::changedWindowsOpacity, this, &CSettingsComponent::changedWindowsOpacity);
            connect(ui->pb_Advanced, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_Audio, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_Gui, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_Hotkeys, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_Network, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_Servers, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_Simulator, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_SimulatorBasics, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_DataLoadAndCaches, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);
            connect(ui->pb_SimulatorMessages, &QPushButton::released, this, &CSettingsComponent::ps_overviewButtonClicked);

            this->initActions();
        }

        CSettingsComponent::~CSettingsComponent()
        { }

        void CSettingsComponent::initActions()
        {
            QAction *a = nullptr;
            a = new QAction(this);
            a->setObjectName("overview");
            a->setShortcut(QKeySequence(Qt::ALT + Qt::Key_S, Qt::Key_O));
            connect(a, &QAction::triggered, this, &CSettingsComponent::ps_actionTriggered);
            this->addAction(a);

            a = new QAction(this);
            a->setObjectName("audio");
            a->setShortcut(QKeySequence(Qt::ALT + Qt::Key_S, Qt::Key_A));
            ui->pb_Audio->setToolTip(a->shortcut().toString());
            connect(a, &QAction::triggered, this, &CSettingsComponent::ps_actionTriggered);
            this->addAction(a);

            a = new QAction(this);
            a->setObjectName("data");
            a->setShortcut(QKeySequence(Qt::ALT + Qt::Key_S, Qt::Key_D));
            ui->pb_DataLoadAndCaches->setToolTip(a->shortcut().toString());
            connect(a, &QAction::triggered, this, &CSettingsComponent::ps_actionTriggered);
            this->addAction(a);

            a = new QAction(this);
            a->setObjectName("gui");
            a->setShortcut(QKeySequence(Qt::ALT + Qt::Key_S, Qt::Key_G));
            ui->pb_Gui->setToolTip(a->shortcut().toString());
            connect(a, &QAction::triggered, this, &CSettingsComponent::ps_actionTriggered);
            this->addAction(a);

            a = new QAction(this);
            a->setObjectName("hotkeys");
            a->setShortcut(QKeySequence(Qt::ALT + Qt::Key_S, Qt::Key_H));
            ui->pb_Hotkeys->setToolTip(a->shortcut().toString());
            connect(a, &QAction::triggered, this, &CSettingsComponent::ps_actionTriggered);
            this->addAction(a);

            a = new QAction(this);
            a->setObjectName("network");
            a->setShortcut(QKeySequence(Qt::ALT + Qt::Key_S, Qt::Key_N));
            ui->pb_Network->setToolTip(a->shortcut().toString());
            connect(a, &QAction::triggered, this, &CSettingsComponent::ps_actionTriggered);
            this->addAction(a);

            a = new QAction(this);
            a->setObjectName("simulator");
            a->setShortcut(QKeySequence(Qt::ALT + Qt::Key_S, Qt::Key_S));
            ui->pb_Simulator->setToolTip(a->shortcut().toString());
            connect(a, &QAction::triggered, this, &CSettingsComponent::ps_actionTriggered);
            this->addAction(a);
        }

        bool CSettingsComponent::playNotificationSounds() const
        {
            return ui->comp_AudioSetup->playNotificationSounds();
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
            if (sender == ui->pb_DataLoadAndCaches) { this->setCurrentIndex(SettingTabDataAndCaches); return; }
            if (sender == ui->pb_SimulatorMessages) { this->setCurrentIndex(SettingTabSimulatorMessages); return; }
            this->setCurrentIndex(SettingTabOverview);
        }

        void CSettingsComponent::ps_actionTriggered()
        {
            const QString a = QObject::sender()->objectName().toLower().trimmed();
            if (a.isEmpty()) { return; }
            if (a.contains("audio")) { this->setCurrentIndex(SettingTabAudio); return; }
            if (a.contains("data")) { this->setCurrentIndex(SettingTabDataAndCaches); return; }
            if (a.contains("gui")) { this->setCurrentIndex(SettingTabGui); return; }
            if (a.contains("hot")) { this->setCurrentIndex(SettingTabHotkeys); return; }
            if (a.contains("network")) { this->setCurrentIndex(SettingTabNetwork); return; }
            if (a.contains("overview")) { this->setCurrentIndex(SettingTabOverview); return; }
            if (a.contains("simulator")) { this->setCurrentIndex(SettingTabSimulator); return; }
        }
    }
} // namespace
