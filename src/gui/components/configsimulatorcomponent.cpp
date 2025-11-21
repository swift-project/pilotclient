// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "configsimulatorcomponent.h"

#include "ui_configsimulatorcomponent.h"

#include "config/buildconfig.h"
#include "gui/guiutility.h"
#include "misc/logmessage.h"
#include "misc/simulation/fscommon/fsdirectories.h"

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::fscommon;
using namespace swift::config;

namespace swift::gui::components
{
    CConfigSimulatorComponent::CConfigSimulatorComponent(QWidget *parent)
        : QFrame(parent), ui(new Ui::CConfigSimulatorComponent)
    {
        ui->setupUi(this);
        this->preselectSimulators();
    }

    CConfigSimulatorComponent::~CConfigSimulatorComponent() = default;

    void CConfigSimulatorComponent::save() { ui->comp_SettingsSimulator->save(); }

    void CConfigSimulatorComponent::load()
    {
        // CSimulatorInfo sims = this->m_enabledSimulators.get();
        // ui->comp_SimulatorSelector->setSimulatorVisible(sims);
        this->preselectSimulators();
    }

    bool CConfigSimulatorComponent::hasUnsavedChanges() const
    {
        return ui->comp_SettingsSimulator->hasUnsavedChanges();
    }

    void CConfigSimulatorComponent::resetUnsavedChanges() { ui->comp_SettingsSimulator->resetUnsavedChanges(); }

    void CConfigSimulatorComponent::preselectSimulators()
    {
        CSimulatorInfo sims = m_enabledSimulators.get();
        // if (!sims.isAnySimulator())
        //{
        CSimulatorInfo simWithModels = m_modelSets.simulatorsWithModels();
        //    sims = simWithModels;
        //}
        if (sims > 127 || sims == 0) sims = CSimulatorInfo::allSimulators();
        // sims = 0; // reset to none if invalid (happens if no enabledsimulators.json exists)

        // no x64 check as we would allow to config 32bit with launcher x64 and vice versa
        const bool p3d =
            (sims.isP3D() || !CFsDirectories::p3dDir().isEmpty()) && CBuildConfig::isCompiledWithP3DSupport();
        const bool fsx =
            (sims.isFSX() || !CFsDirectories::fsxDir().isEmpty()) && CBuildConfig::isCompiledWithFsxSupport();
        const bool fs9 =
            (sims.isFS9() || !CFsDirectories::fs9Dir().isEmpty()) && CBuildConfig::isCompiledWithFs9Support();

        // const bool msfs2020 = (sims.isMSFS2020() || !CFsDirectories::msfs2020Dir().isEmpty()) &&
        //                       CBuildConfig::isCompiledWithMSFS2020Support();
        // const bool msfs2024 = (sims.isMSFS2024() || !CFsDirectories::msfs2024Dir().isEmpty()) &&
        //                       CBuildConfig::isCompiledWithMSFS2024Support();

        const bool msfs2020 = (sims.isMSFS2020() && CBuildConfig::isCompiledWithMSFS2020Support());

        const bool msfs2024 = (sims.isMSFS2024() && CBuildConfig::isCompiledWithMSFS2024Support());

        const bool xp = sims.isXPlane() && CBuildConfig::isCompiledWithXPlaneSupport();
        const bool fg = sims.isFG() && CBuildConfig::isCompiledWithFGSupport();

        if (p3d) { ui->comp_SettingsSimulator->setSimulator(CSimulatorInfo(CSimulatorInfo::P3D)); }
        else if (fsx) { ui->comp_SettingsSimulator->setSimulator(CSimulatorInfo(CSimulatorInfo::FSX)); }
        else if (fs9) { ui->comp_SettingsSimulator->setSimulator(CSimulatorInfo(CSimulatorInfo::FS9)); }
        else if (xp) { ui->comp_SettingsSimulator->setSimulator(CSimulatorInfo(CSimulatorInfo::XPLANE)); }
        else if (fg) { ui->comp_SettingsSimulator->setSimulator(CSimulatorInfo(CSimulatorInfo::FG)); }
        else if (msfs2020) { ui->comp_SettingsSimulator->setSimulator(CSimulatorInfo(CSimulatorInfo::MSFS2020)); }
        else if (msfs2024) { ui->comp_SettingsSimulator->setSimulator(CSimulatorInfo(CSimulatorInfo::MSFS2024)); }

        ui->comp_SettingsSimulator->hideSelector(sims);
    }

    void CConfigSimulatorWizardPage::initializePage()
    {
        m_config->resetUnsavedChanges();
        Q_ASSERT_X(m_config, Q_FUNC_INFO, "Missing config");
        m_config->load();
    }

    bool CConfigSimulatorWizardPage::validatePage()
    {
        Q_ASSERT_X(m_config, Q_FUNC_INFO, "Missing config");
        m_config->save();
        return true;
    }
} // namespace swift::gui::components
