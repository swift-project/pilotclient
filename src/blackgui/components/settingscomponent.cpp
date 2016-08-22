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

            this->connect(ui->hs_SettingsGuiAircraftRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedAircraftUpdateInterval);
            this->connect(ui->hs_SettingsGuiAtcRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedAtcStationsUpdateInterval);
            this->connect(ui->hs_SettingsGuiUserRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedUsersUpdateInterval);
            this->connect(ui->comp_SettingsGuiGeneral, &CSettingsGuiComponent::changedWindowsOpacity, this, &CSettingsComponent::changedWindowsOpacity);
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
    }
} // namespace
