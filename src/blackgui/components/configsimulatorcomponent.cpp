/* Copyright (C) 2017
 * swift project community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "configsimulatorcomponent.h"
#include "blackconfig/buildconfig.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/simulation/fscommon/fscommonutil.h"
#include "ui_configsimulatorcomponent.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackConfig;

namespace BlackGui
{
    namespace Components
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

        void CConfigSimulatorComponent::preselectSimulators()
        {
            CSimulatorInfo sims;
            if (m_enabledSimulators.isSaved())
            {
                sims = CSimulatorInfo(m_enabledSimulators.get());
            }
            else
            {
                // by model set
                sims = m_modelSets.simulatorsWithInitializedCache();
            }

            const bool p3d = (sims.p3d() || !CFsCommonUtil::p3dDir().isEmpty()) && CBuildConfig::isCompiledWithP3DSupport();
            const bool fsx = (sims.fsx() || !CFsCommonUtil::fsxDir().isEmpty()) && CBuildConfig::isCompiledWithFsxSupport();
            const bool fs9 = (sims.fs9() || !CFsCommonUtil::fs9Dir().isEmpty()) && CBuildConfig::isCompiledWithFs9Support();
            const bool xp = sims.xplane() && CBuildConfig::isCompiledWithXPlaneSupport();

            ui->cb_P3D->setChecked(p3d);
            ui->cb_FSX->setChecked(fsx);
            ui->cb_FS9->setChecked(fs9);
            ui->cb_XP->setChecked(xp);

            ui->cb_P3D->setEnabled(CBuildConfig::isCompiledWithP3DSupport());
            ui->cb_FSX->setEnabled(CBuildConfig::isCompiledWithFsxSupport());
            ui->cb_FS9->setEnabled(CBuildConfig::isCompiledWithFs9Support());
            ui->cb_XP->setEnabled(CBuildConfig::isCompiledWithXPlaneSupport());

            if (p3d) { ui->comp_SettingsSimulator->setSimulator(CSimulatorInfo(CSimulatorInfo::P3D)); }
            else if (fsx) { ui->comp_SettingsSimulator->setSimulator(CSimulatorInfo(CSimulatorInfo::FSX)); }
            else if (fs9) { ui->comp_SettingsSimulator->setSimulator(CSimulatorInfo(CSimulatorInfo::FS9)); }
            else if (xp)  { ui->comp_SettingsSimulator->setSimulator(CSimulatorInfo(CSimulatorInfo::XPLANE)); }
        }

        QStringList CConfigSimulatorComponent::selectedSimsToPluginIds()
        {
            QStringList ids;

            // have to match full canonical ids from swift-plugin-simulators.xml
            if (ui->cb_FS9->isChecked()) { ids << CSimulatorPluginInfo::fs9PluginIdentifier(); }
            if (ui->cb_FSX->isChecked()) { ids << CSimulatorPluginInfo::fsxPluginIdentifier(); }
            if (ui->cb_P3D->isChecked()) { ids << CSimulatorPluginInfo::p3dPluginIdentifier(); }
            if (ui->cb_XP->isChecked())  { ids << CSimulatorPluginInfo::xplanePluginIdentifier(); }

            return ids;
        }

        bool CConfigSimulatorWizardPage::validatePage()
        {
            Q_ASSERT_X(m_config, Q_FUNC_INFO, "Missing config");
            m_config->save();
            return true;
        }
    } // ns
} // ns
