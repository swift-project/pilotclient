/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "fsxsettingscomponent.h"
#include "ui_fsxsettingscomponent.h"
#include "simulatorfsxcommon.h"
#include "blackgui/guiapplication.h"

using namespace BlackCore;
using namespace BlackGui;

namespace BlackSimPlugin
{
    namespace FsxCommon
    {
        CFsxSettingsComponent::CFsxSettingsComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CFsxSettingsComponent)
        {
            ui->setupUi(this);
            ui->cb_TraceSimConnectCalls->setChecked(false);

            connect(ui->cb_TraceSimConnectCalls, &QCheckBox::released, this, &CFsxSettingsComponent::onSimConnectTraceChanged);
            connect(ui->cb_EnableTerrainProbe, &QCheckBox::released, this, &CFsxSettingsComponent::onEnableTerrainProbeChanged);

            const CSimulatorFsxCommon *fsx = this->getFsxSimulator();
            if (fsx)
            {
                ui->cb_TraceSimConnectCalls->setChecked(fsx->isTracingSendId());
            }
        }

        CFsxSettingsComponent::~CFsxSettingsComponent()
        { }

        void CFsxSettingsComponent::onSimConnectTraceChanged()
        {
            CSimulatorFsxCommon *fsx = this->getFsxSimulator();
            if (!fsx) { return; }
            fsx->setTractingSendId(ui->cb_TraceSimConnectCalls->isChecked());
        }

        void CFsxSettingsComponent::onEnableTerrainProbeChanged()
        {
            CSimulatorFsxCommon *fsx = this->getFsxSimulator();
            if (!fsx) { return; }
            fsx->setUsingFsxTerrainProbe(ui->cb_EnableTerrainProbe->isChecked());
        }

        CSimulatorFsxCommon *CFsxSettingsComponent::getFsxSimulator() const
        {
            if (!sGui || !sGui->getISimulator() || sGui->isShuttingDown()) { return nullptr; }
            ISimulator *sim = sGui->getISimulator();
            if (!sim->getSimulatorInfo().isFsxP3DFamily()) { return nullptr; }
            if (sim->getSimulatorInfo() != m_simulator) { return nullptr; }
            CSimulatorFsxCommon *fsx = static_cast<CSimulatorFsxCommon *>(sim); // wonder why qobject_cast does not work here
            return fsx;
        }
    } // ns
} // ns
