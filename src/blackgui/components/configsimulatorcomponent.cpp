/* Copyright (C) 2017
 * swift project community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "configsimulatorcomponent.h"
#include "blackgui/guiutility.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/simulation/fscommon/fsdirectories.h"
#include "blackconfig/buildconfig.h"
#include "ui_configsimulatorcomponent.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackConfig;

namespace BlackGui::Components
{
    CConfigSimulatorComponent::CConfigSimulatorComponent(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CConfigSimulatorComponent)
    {
        ui->setupUi(this);
        this->preselectSimulators();
    }

    CConfigSimulatorComponent::~CConfigSimulatorComponent()
    { }

    void CConfigSimulatorComponent::save()
    {
        ui->comp_SettingsSimulator->save();
        const QStringList sims = this->selectedSimsToPluginIds();
        const CStatusMessage msg = m_enabledSimulators.setAndSave(sims);
        CLogMessage::preformatted(msg);
    }

    bool CConfigSimulatorComponent::hasUnsavedChanges() const
    {
        return ui->comp_SettingsSimulator->hasUnsavedChanges();
    }

    void CConfigSimulatorComponent::resetUnsavedChanges()
    {
        ui->comp_SettingsSimulator->resetUnsavedChanges();
    }

    void CConfigSimulatorComponent::preselectSimulators()
    {
        CSimulatorInfo sims = m_enabledSimulators.get();
        if (!sims.isAnySimulator())
        {
            CSimulatorInfo simWithModels = m_modelSets.simulatorsWithModels();
            sims = simWithModels;
        }

        // no x64 check as we would allow to config 32bit with launcher x64 and vice versa
        const bool p3d = (sims.isP3D() || !CFsDirectories::p3dDir().isEmpty()) && CBuildConfig::isCompiledWithP3DSupport();
        const bool fsx = (sims.isFSX() || !CFsDirectories::fsxDir().isEmpty()) && CBuildConfig::isCompiledWithFsxSupport();
        const bool fs9 = (sims.isFS9() || !CFsDirectories::fs9Dir().isEmpty()) && CBuildConfig::isCompiledWithFs9Support();
        const bool xp = sims.isXPlane() && CBuildConfig::isCompiledWithXPlaneSupport();
        const bool fg = sims.isFG() && CBuildConfig::isCompiledWithFGSupport();

        ui->cb_P3D->setChecked(p3d);
        ui->cb_FSX->setChecked(fsx);
        ui->cb_FS9->setChecked(fs9);
        ui->cb_XP->setChecked(xp);
        ui->cb_FG->setChecked(fg);

        ui->cb_P3D->setEnabled(CBuildConfig::isCompiledWithP3DSupport());
        ui->cb_FSX->setEnabled(CBuildConfig::isCompiledWithFsxSupport());
        ui->cb_FS9->setEnabled(CBuildConfig::isCompiledWithFs9Support());
        ui->cb_XP->setEnabled(CBuildConfig::isCompiledWithXPlaneSupport());
        ui->cb_FG->setEnabled(CBuildConfig::isCompiledWithFGSupport());

        CGuiUtility::checkBoxReadOnly(ui->cb_P3D, !CBuildConfig::isCompiledWithP3DSupport());
        CGuiUtility::checkBoxReadOnly(ui->cb_FSX, !CBuildConfig::isCompiledWithFsxSupport());
        CGuiUtility::checkBoxReadOnly(ui->cb_FS9, !CBuildConfig::isCompiledWithFs9Support());
        CGuiUtility::checkBoxReadOnly(ui->cb_XP,  !CBuildConfig::isCompiledWithXPlaneSupport());
        CGuiUtility::checkBoxReadOnly(ui->cb_FG,  !CBuildConfig::isCompiledWithFGSupport());

        /** does NOT work
        const qreal fadeOut = 0.25;
        ui->lbl_P3D->setWindowOpacity(CBuildConfig::isCompiledWithP3DSupport() ? 1.0 : fadeOut);
        ui->lbl_FSX->setWindowOpacity(CBuildConfig::isCompiledWithFsxSupport() ? 1.0 : fadeOut);
        ui->lbl_FS9->setWindowOpacity(CBuildConfig::isCompiledWithFs9Support() ? 1.0 : fadeOut);
        ui->lbl_XP->setWindowOpacity(CBuildConfig::isCompiledWithXPlaneSupport() ? 1.0 : fadeOut);
        ui->lbl_FG->setWindowOpacity(CBuildConfig::isCompiledWithFGSupport() ? 1.0 : fadeOut);
        **/

        if (p3d) { ui->comp_SettingsSimulator->setSimulator(CSimulatorInfo(CSimulatorInfo::P3D)); }
        else if (fsx) { ui->comp_SettingsSimulator->setSimulator(CSimulatorInfo(CSimulatorInfo::FSX)); }
        else if (fs9) { ui->comp_SettingsSimulator->setSimulator(CSimulatorInfo(CSimulatorInfo::FS9)); }
        else if (xp)  { ui->comp_SettingsSimulator->setSimulator(CSimulatorInfo(CSimulatorInfo::XPLANE)); }
        else if (fg)  { ui->comp_SettingsSimulator->setSimulator(CSimulatorInfo(CSimulatorInfo::FG)); }
    }

    QStringList CConfigSimulatorComponent::selectedSimsToPluginIds()
    {
        QStringList ids;

        // have to match full canonical ids from swift-plugin-simulators.xml
        if (ui->cb_FS9->isChecked()) { ids << CSimulatorPluginInfo::fs9PluginIdentifier(); }
        if (ui->cb_FSX->isChecked()) { ids << CSimulatorPluginInfo::fsxPluginIdentifier(); }
        if (ui->cb_P3D->isChecked()) { ids << CSimulatorPluginInfo::p3dPluginIdentifier(); }
        if (ui->cb_XP->isChecked())  { ids << CSimulatorPluginInfo::xplanePluginIdentifier(); }
        if (ui->cb_FG->isChecked())  { ids << CSimulatorPluginInfo::fgPluginIdentifier(); }

        return ids;
    }

    void CConfigSimulatorWizardPage::initializePage()
    {
        m_config->resetUnsavedChanges();
    }

    bool CConfigSimulatorWizardPage::validatePage()
    {
        Q_ASSERT_X(m_config, Q_FUNC_INFO, "Missing config");
        m_config->save();
        return true;
    }
} // ns
