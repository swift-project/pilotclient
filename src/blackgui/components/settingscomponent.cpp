/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/audionotificationcomponent.h"
#include "blackgui/components/settingscomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"
#include "blackconfig/buildconfig.h"
#include "blackmisc/logmessage.h"
#include "ui_settingscomponent.h"

#include <QAction>
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
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Settings;
using namespace BlackCore;
using namespace BlackGui;
using namespace BlackConfig;

namespace BlackGui::Components
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

        this->initActions();
    }

    CSettingsComponent::~CSettingsComponent()
    { }

    void CSettingsComponent::initActions()
    {
        QAction *a = new QAction(this);
        a->setObjectName("overview");
        a->setShortcut(QKeySequence(static_cast<Qt::Key>(Qt::ALT) + Qt::Key_S, Qt::Key_O));
        connect(a, &QAction::triggered, this, &CSettingsComponent::onActionTriggered);
        this->addAction(a);
        ui->lbl_Hint->setText("Hint: See tooltips for shortcuts, \"ALT+S, O\" for overview.");

        a = new QAction(this);
        a->setObjectName("audio");
        a->setShortcut(QKeySequence(static_cast<Qt::Key>(Qt::ALT) + Qt::Key_S, Qt::Key_A));
        ui->pb_Audio->setToolTip(a->shortcut().toString());
        connect(a, &QAction::triggered, this, &CSettingsComponent::onActionTriggered);
        this->addAction(a);

        a = new QAction(this);
        a->setObjectName("data");
        a->setShortcut(QKeySequence(static_cast<Qt::Key>(Qt::ALT) + Qt::Key_S, Qt::Key_D));
        ui->pb_DataLoadAndCaches->setToolTip(a->shortcut().toString());
        connect(a, &QAction::triggered, this, &CSettingsComponent::onActionTriggered);
        this->addAction(a);

        a = new QAction(this);
        a->setObjectName("gui");
        a->setShortcut(QKeySequence(static_cast<Qt::Key>(Qt::ALT) + Qt::Key_S, Qt::Key_G));
        ui->pb_Gui->setToolTip(a->shortcut().toString());
        connect(a, &QAction::triggered, this, &CSettingsComponent::onActionTriggered);
        this->addAction(a);

        a = new QAction(this);
        a->setObjectName("hotkeys");
        a->setShortcut(QKeySequence(static_cast<Qt::Key>(Qt::ALT) + Qt::Key_S, Qt::Key_H));
        ui->pb_Hotkeys->setToolTip(a->shortcut().toString());
        connect(a, &QAction::triggered, this, &CSettingsComponent::onActionTriggered);
        this->addAction(a);

        a = new QAction(this);
        a->setObjectName("network");
        a->setShortcut(QKeySequence(static_cast<Qt::Key>(Qt::ALT) + Qt::Key_S, Qt::Key_N));
        ui->pb_Network->setToolTip(a->shortcut().toString());
        connect(a, &QAction::triggered, this, &CSettingsComponent::onActionTriggered);
        this->addAction(a);

        a = new QAction(this);
        a->setObjectName("simulator");
        a->setShortcut(QKeySequence(static_cast<Qt::Key>(Qt::ALT) + Qt::Key_S, Qt::Key_S));
        ui->pb_Simulator->setToolTip(a->shortcut().toString());
        connect(a, &QAction::triggered, this, &CSettingsComponent::onActionTriggered);
        this->addAction(a);

        a = new QAction(this);
        a->setObjectName("matching");
        a->setShortcut(QKeySequence(static_cast<Qt::Key>(Qt::ALT) + Qt::Key_S, Qt::Key_M));
        ui->pb_Matching->setToolTip(a->shortcut().toString());
        connect(a, &QAction::triggered, this, &CSettingsComponent::onActionTriggered);
        this->addAction(a);
    }

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
        if (sender == ui->pb_Matching) { this->setCurrentIndex(SettingTabMatching); return; }
        this->setCurrentIndex(SettingTabOverview);
    }

    void CSettingsComponent::onActionTriggered()
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
        if (a.contains("matching")) { this->setCurrentIndex(SettingTabMatching); return; }
        if (a.contains("advanced")) { this->setCurrentIndex(SettingTabAdvanced); return; }
    }
} // namespace
