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
        }

        CMainKeypadAreaComponent::~CMainKeypadAreaComponent()
        { }

        void CMainKeypadAreaComponent::ps_buttonPressed()
        {
            const QObject *sender = QObject::sender();
            CMainInfoAreaComponent::InfoArea ia = buttonToMainInfoArea(sender);
            if (ia != CMainInfoAreaComponent::InfoAreaNone)
            {
                emit selectMainInfoAreaDockWidget(ia);
            }
        }

        void CMainKeypadAreaComponent::ps_buttonDoubleClicked()
        {

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

    } // namespace
} // namespace
