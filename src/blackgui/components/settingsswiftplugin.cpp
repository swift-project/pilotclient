// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "settingsswiftplugin.h"
#include "blackmisc/logmessage.h"
#include "ui_settingsswiftplugin.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Settings;

namespace BlackGui::Components
{
    CSettingsSwiftPlugin::CSettingsSwiftPlugin(QWidget *parent) : QFrame(parent),
                                                                  ui(new Ui::CSettingsSwiftPlugin)
    {
        ui->setupUi(this);
        ui->comp_EmulatedSimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
        this->onSettingsChanged();

        connect(ui->pb_Save, &QPushButton::clicked, this, &CSettingsSwiftPlugin::save);
    }

    CSettingsSwiftPlugin::~CSettingsSwiftPlugin()
    {}

    BlackMisc::Simulation::Settings::CSwiftPluginSettings CSettingsSwiftPlugin::getPluginSettings() const
    {
        return m_settings.get();
    }

    void CSettingsSwiftPlugin::onSettingsChanged()
    {
        const CSwiftPluginSettings settings(m_settings.get());
        ui->comp_EmulatedSimulatorSelector->setValue(settings.getEmulatedSimulator());
        ui->lep_OwnModel->setText(settings.getOwnModel().getModelString());
        ui->lep_DefaultModel->setText(settings.getDefaultModel().getModelString());
        ui->cb_LogFunctionCalls->setChecked(settings.isLoggingFunctionCalls());
    }

    void CSettingsSwiftPlugin::save()
    {
        const CSwiftPluginSettings settings(this->getSettings());
        const CStatusMessage msg = m_settings.setAndSave(settings);
        if (!msg.isEmpty()) { CLogMessage::preformatted(msg); }
    }

    CSwiftPluginSettings CSettingsSwiftPlugin::getSettings() const
    {
        CAircraftModel model = ui->lep_OwnModel->getAircraftModel();
        if (!model.hasModelString())
        {
            model.setModelString(ui->lep_OwnModel->text());
        }

        CSwiftPluginSettings settings;
        settings.setEmulatedSimulator(ui->comp_EmulatedSimulatorSelector->getValue());
        settings.setOwnModel(model);
        settings.setDefaultModel(ui->lep_DefaultModel->getAircraftModel());
        settings.setLoggingFunctionCalls(ui->cb_LogFunctionCalls->isChecked());
        return settings;
    }
} // ns
