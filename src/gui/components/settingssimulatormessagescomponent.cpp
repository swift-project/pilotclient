// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "settingssimulatormessagescomponent.h"

#include "ui_settingssimulatormessagescomponent.h"

#include "misc/simulation/settings/simulatorsettings.h"

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::settings;

namespace swift::gui::components
{
    CSettingsSimulatorMessagesComponent::CSettingsSimulatorMessagesComponent(QWidget *parent)
        : QFrame(parent), ui(new Ui::CSettingsSimulatorMessagesComponent)
    {
        ui->setupUi(this);
        connect(ui->pb_Save, &QPushButton::clicked, this, &CSettingsSimulatorMessagesComponent::save,
                Qt::QueuedConnection);
        connect(ui->pb_Cancel, &QPushButton::clicked, this, &CSettingsSimulatorMessagesComponent::load,
                Qt::QueuedConnection);

        this->load();
    }

    CSettingsSimulatorMessagesComponent::~CSettingsSimulatorMessagesComponent() {}

    void CSettingsSimulatorMessagesComponent::save()
    {
        CSimulatorMessagesSettings settings;
        settings.setRelayGloballyEnabled(ui->cb_Messages->isChecked());
        if (ui->rb_NoTechnicalMessages->isChecked()) { settings.disableTechnicalMessages(); }
        else if (ui->rb_ErrorsOnly->isChecked()) { settings.setTechnicalLogSeverity(CStatusMessage::SeverityError); }
        else if (ui->rb_ErrorsAndWarnings->isChecked())
        {
            settings.setTechnicalLogSeverity(CStatusMessage::SeverityWarning);
        }
        else if (ui->rb_ErrorWarningsInfo->isChecked())
        {
            settings.setTechnicalLogSeverity(CStatusMessage::SeverityInfo);
        }

        settings.setRelayGloballyEnabled(ui->cb_Messages->isChecked());
        CSimulatorMessagesSettings::TextMessageType mt = CSimulatorMessagesSettings::NoTextMessages;

        if (ui->cb_PrivateMessages->isChecked()) { mt |= CSimulatorMessagesSettings::TextMessagePrivate; }
        if (ui->cb_SupervisorMessages->isChecked()) { mt |= CSimulatorMessagesSettings::TextMessageSupervisor; }
        if (ui->cb_Com1->isChecked()) { mt |= CSimulatorMessagesSettings::TextMessagesCom1; }
        if (ui->cb_Com2->isChecked()) { mt |= CSimulatorMessagesSettings::TextMessagesCom2; };
        settings.setRelayTextMessages(mt);

        // save
        this->m_settings.setAndSave(settings);
    }

    void CSettingsSimulatorMessagesComponent::load()
    {
        const CSimulatorMessagesSettings settings(this->m_settings.get());
        ui->cb_Messages->setChecked(settings.isRelayGloballyEnabled());
        if (settings.isRelayInfoMessages()) { ui->rb_ErrorWarningsInfo->setChecked(true); }
        else if (settings.isRelayWarningMessages()) { ui->rb_ErrorsAndWarnings->setChecked(true); }
        else if (settings.isRelayErrorsMessages()) { ui->rb_ErrorsOnly->setChecked(true); }
        else if (!settings.isRelayTechnicalMessages()) { ui->rb_NoTechnicalMessages->setChecked(true); }

        ui->cb_PrivateMessages->setChecked(settings.isRelayPrivateTextMessages());
        ui->cb_SupervisorMessages->setChecked(settings.isRelaySupervisorTextMessages());
        ui->cb_Com1->setChecked(settings.isRelayCom1TextMessages());
        ui->cb_Com2->setChecked(settings.isRelayCom2TextMessages());
    }
} // namespace swift::gui::components
