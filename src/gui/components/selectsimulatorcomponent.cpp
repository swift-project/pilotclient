// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "selectsimulatorcomponent.h"

#include "ui_selectsimulatorcomponent.h"

#include "config/buildconfig.h"
#include "gui/components/configurationwizard.h"
#include "gui/guiutility.h"
#include "misc/logmessage.h"
#include "misc/simulation/fscommon/fsdirectories.h"

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::fscommon;
using namespace swift::config;

namespace swift::gui::components
{
    CSelectSimulatorComponent::CSelectSimulatorComponent(QWidget *parent)
        : QFrame(parent), ui(new Ui::CSelectSimulatorComponent)
    {
        ui->setupUi(this);
        this->preselectSimulators();
        this->preselectOptions();
    }

    CSelectSimulatorComponent::~CSelectSimulatorComponent() = default;

    CSimulatorInfo CSelectSimulatorComponent::getSelectedSimulators()
    {
        int sims = ui->cb_FS9->isChecked() ? 1 : 0;
        sims += ui->cb_FSX->isChecked() ? 2 : 0;
        sims += ui->cb_P3D->isChecked() ? 4 : 0;
        sims += ui->cb_XP->isChecked() ? 8 : 0;
        sims += ui->cb_FG->isChecked() ? 16 : 0;
        sims += ui->cb_MSFS2020->isChecked() ? 32 : 0;
        sims += ui->cb_MSFS2024->isChecked() ? 64 : 0;
        return sims;
    }

    void CSelectSimulatorComponent::save()
    {
        const QStringList sims = this->selectedSimsToPluginIds();
        const CStatusMessage msg = this->m_enabledSimulators.setAndSave(sims);
        CLogMessage::preformatted(msg);

        const QStringList opt = this->selectedOptions();
        CLogMessage::preformatted(m_enabledConfigOptions.setAndSave(opt));
        CLogMessage::preformatted(msg);
    }

    void CSelectSimulatorComponent::load() { this->preselectSimulators(); }

    void CSelectSimulatorComponent::preselectOptions()
    {
        QStringList opt = this->m_enabledConfigOptions.get(); // force reload
        QString cb_GenerateModelsets = "false";
        QString cb_PTT = "false";
        QString cb_SetExportMode = "false";
        if (!opt.isEmpty())
        {
            cb_GenerateModelsets = opt.first();
            opt.removeFirst();
            if (!opt.isEmpty()) cb_PTT = opt.first();
            opt.removeFirst();
            if (!opt.isEmpty()) cb_SetExportMode = opt.first();
        }
        m_PTT = cb_PTT.contains("true") ? true : false;
        m_SetExportMode = cb_SetExportMode.contains("true") ? true : false;
        m_GenerateModelsets = cb_GenerateModelsets.contains("true") ? true : false;
    }
    void CSelectSimulatorComponent::preselectSimulators()
    {
        CSimulatorInfo sims = this->m_enabledSimulators.get();
        // if (!sims.isAnySimulator())
        //{
        CSimulatorInfo simWithModels = m_modelSets.simulatorsWithModels();
        // sims = simWithModels;
        //}

        if (sims > 127) sims = 0; // reset to none if invalid (happens if no enabledsimulators.json exists)

        // no x64 check as we would allow to config 32bit with launcher x64 and vice versa
        const bool p3d =
            (sims.isP3D() || !CFsDirectories::p3dDir().isEmpty()) && CBuildConfig::isCompiledWithP3DSupport();
        const bool fsx =
            (sims.isFSX() || !CFsDirectories::fsxDir().isEmpty()) && CBuildConfig::isCompiledWithFsxSupport();
        const bool fs9 =
            (sims.isFS9() || !CFsDirectories::fs9Dir().isEmpty()) && CBuildConfig::isCompiledWithFs9Support();

        const bool msfs2020 =
            ((sims.isMSFS2020() || simWithModels.isMSFS2020()) && !CFsDirectories::msfs2020Dir().isEmpty()) &&
            CBuildConfig::isCompiledWithMSFS2020Support();

        const bool msfs2024 =
            ((sims.isMSFS2024() || simWithModels.isMSFS2024()) && !CFsDirectories::msfs2024Dir().isEmpty()) &&
            CBuildConfig::isCompiledWithMSFS2024Support();

        const bool xp = sims.isXPlane() && CBuildConfig::isCompiledWithXPlaneSupport();
        const bool fg = sims.isFG() && CBuildConfig::isCompiledWithFGSupport();

        ui->cb_P3D->setChecked(p3d);
        ui->cb_FSX->setChecked(fsx);
        ui->cb_FS9->setChecked(fs9);
        ui->cb_XP->setChecked(xp);
        ui->cb_FG->setChecked(fg);
        ui->cb_MSFS2020->setChecked(msfs2020);
        ui->cb_MSFS2024->setChecked(msfs2024);

        ui->cb_P3D->setEnabled(CBuildConfig::isCompiledWithP3DSupport());
        ui->cb_FSX->setEnabled(CBuildConfig::isCompiledWithFsxSupport());
        ui->cb_FS9->setEnabled(CBuildConfig::isCompiledWithFs9Support());
        ui->cb_XP->setEnabled(CBuildConfig::isCompiledWithXPlaneSupport());
        ui->cb_FG->setEnabled(CBuildConfig::isCompiledWithFGSupport());
        ui->cb_MSFS2020->setEnabled(CBuildConfig::isCompiledWithMSFS2020Support());
        ui->cb_MSFS2024->setEnabled(CBuildConfig::isCompiledWithMSFS2024Support());

        CGuiUtility::checkBoxReadOnly(ui->cb_P3D, !CBuildConfig::isCompiledWithP3DSupport());
        CGuiUtility::checkBoxReadOnly(ui->cb_FSX, !CBuildConfig::isCompiledWithFsxSupport());
        CGuiUtility::checkBoxReadOnly(ui->cb_FS9, !CBuildConfig::isCompiledWithFs9Support());
        CGuiUtility::checkBoxReadOnly(ui->cb_XP, !CBuildConfig::isCompiledWithXPlaneSupport());
        CGuiUtility::checkBoxReadOnly(ui->cb_FG, !CBuildConfig::isCompiledWithFGSupport());
        CGuiUtility::checkBoxReadOnly(ui->cb_MSFS2020, !CBuildConfig::isCompiledWithMSFS2020Support());
        CGuiUtility::checkBoxReadOnly(ui->cb_MSFS2024, !CBuildConfig::isCompiledWithMSFS2024Support());

        ui->cb_PTT->setChecked(m_PTT);
        ui->cb_SetExportMode->setChecked(m_SetExportMode);

        if (simWithModels.isAnySimulator())
        {
            ui->cb_GenerateModelsets->setText("Create first model sets. Since you have already imported model sets "
                                              "from an existing installation, you can skip this step");
            ui->cb_GenerateModelsets->setChecked(m_GenerateModelsets);
        }
        else
        {
            ui->cb_GenerateModelsets->setText("Create first model sets.");
            ui->cb_GenerateModelsets->setChecked(true);
        }
    }

    QStringList CSelectSimulatorComponent::selectedSimsToPluginIds()
    {
        QStringList ids;

        // have to match full canonical ids from swift-plugin-simulators.xml
        if (ui->cb_FS9->isChecked()) { ids << CSimulatorPluginInfo::fs9PluginIdentifier(); }
        if (ui->cb_FSX->isChecked()) { ids << CSimulatorPluginInfo::fsxPluginIdentifier(); }
        if (ui->cb_P3D->isChecked()) { ids << CSimulatorPluginInfo::p3dPluginIdentifier(); }
        if (ui->cb_XP->isChecked()) { ids << CSimulatorPluginInfo::xplanePluginIdentifier(); }
        if (ui->cb_FG->isChecked()) { ids << CSimulatorPluginInfo::fgPluginIdentifier(); }
        if (ui->cb_MSFS2020->isChecked()) { ids << CSimulatorPluginInfo::msfs2020PluginIdentifier(); }
        if (ui->cb_MSFS2024->isChecked()) { ids << CSimulatorPluginInfo::msfs2024PluginIdentifier(); }

        return ids;
    }

    QStringList CSelectSimulatorComponent::selectedOptions()
    {
        QString cb_PTT = ui->cb_PTT->isChecked() ? "true" : "false";
        QString cb_GenerateModelsets = ui->cb_GenerateModelsets->isChecked() ? "true" : "false";
        QString cb_SetExportMode = ui->cb_SetExportMode->isChecked() ? "true" : "false";

        QStringList options = { cb_GenerateModelsets, cb_PTT, cb_SetExportMode };

        return options;
    }

    void CSelectSimulatorWizardPage::initializePage()
    {
        Q_ASSERT_X(m_config, Q_FUNC_INFO, "Missing config");
        m_config->load();
    }

    bool CSelectSimulatorWizardPage::validatePage()
    {
        Q_ASSERT_X(m_config, Q_FUNC_INFO, "Missing config");
        m_config->save();
        return true;
    }
} // namespace swift::gui::components
