/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingssimulatormessagescomponent.h"
#include "ui_settingssimulatormessagescomponent.h"
#include "blackmisc/simulation/simulationsettings.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Components
    {
        CSettingsSimulatorMessagesComponent::CSettingsSimulatorMessagesComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsSimulatorMessagesComponent)
        {
            ui->setupUi(this);
            connect(ui->pb_Save, &QPushButton::clicked, this, &CSettingsSimulatorMessagesComponent::ps_save);
            connect(ui->pb_Cancel, &QPushButton::clicked, this, &CSettingsSimulatorMessagesComponent::ps_load);

            this->ps_load();
        }

        CSettingsSimulatorMessagesComponent::~CSettingsSimulatorMessagesComponent()
        { }

        void CSettingsSimulatorMessagesComponent::ps_save()
        {
            CSettingsSimulatorMessages settings;
            settings.setGloballyEnabled(ui->cb_Messages->isChecked());
            if (ui->rb_NoTechnicalMessages->isChecked())
            {
                settings.disableTechnicalMessages();
            }
            else if (ui->rb_ErrorsOnly->isChecked())
            {
                settings.setTechnicalLogSeverity(CStatusMessage::SeverityError);
            }
            else if (ui->rb_ErrorsAndWarnings->isChecked())
            {
                settings.setTechnicalLogSeverity(CStatusMessage::SeverityWarning);
            }
            else if (ui->rb_ErrorWarningsInfo->isChecked())
            {
                settings.setTechnicalLogSeverity(CStatusMessage::SeverityInfo);
            }

            CSettingsSimulatorMessages::TextMessageType mt = CSettingsSimulatorMessages::NoTextMessages;
            if (ui->cb_PrivateMessages->isChecked()) { mt |= CSettingsSimulatorMessages::TextMessagePrivate; }
            if (ui->cb_SupervisorMessages->isChecked()) { mt |= CSettingsSimulatorMessages::TextMessageSupervisor; }
            if (ui->cb_Com1->isChecked()) { mt |= CSettingsSimulatorMessages::TextMessagesCom1; }
            if (ui->cb_Com2->isChecked()) { mt |= CSettingsSimulatorMessages::TextMessagesCom2; };
            settings.setRelayedTextMessages(mt);

            // save
            this->m_settings.setAndSave(settings);
        }

        void CSettingsSimulatorMessagesComponent::ps_load()
        {
            const CSettingsSimulatorMessages settings(this->m_settings.get());
            ui->cb_Messages->setChecked(settings.isGloballyEnabled());
            if (settings.isRelayedInfoMessages())
            {
                ui->rb_ErrorWarningsInfo->setChecked(true);
            }
            else if (settings.isRelayedWarningMessages())
            {
                ui->rb_ErrorsAndWarnings->setChecked(true);
            }
            else if (settings.isRelayedErrorsMessages())
            {
                ui->rb_ErrorsOnly->setChecked(true);
            }
            else if (!settings.isRelayedTechnicalMessages())
            {
                ui->rb_NoTechnicalMessages->setChecked(true);
            }

            ui->cb_PrivateMessages->setChecked(settings.isRelayedPrivateTextMessages());
            ui->cb_SupervisorMessages->setChecked(settings.isRelayedSupervisorTextMessages());
            ui->cb_Com1->setChecked(settings.isRelayedCom1TextMessages());
            ui->cb_Com2->setChecked(settings.isRelayedCom2TextMessages());
        }
    } // ns
} // ns
