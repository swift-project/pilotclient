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
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextsimulator.h"

using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;

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
            connect(ui->cb_UseFsuipc, &QCheckBox::released, this, &CFsxSettingsComponent::onFsuipcChanged);
            connect(ui->cb_SBOffsets, &QCheckBox::released, this, &CFsxSettingsComponent::onSBOffsetsChanged);
            connect(ui->pb_CopyTerrainProbe, &QPushButton::released, this, &CFsxSettingsComponent::copyTerrainProbe);

            const CSimulatorFsxCommon *fsx = this->getFsxSimulator();
            if (fsx)
            {
                ui->cb_TraceSimConnectCalls->setChecked(fsx->isTracingSendId());
                ui->cb_EnableTerrainProbe->setChecked(fsx->isUsingFsxTerrainProbe());
                ui->cb_SBOffsets->setChecked(fsx->isUsingSbOffsetValues());
                ui->cb_UseFsuipc->setChecked(fsx->isFsuipcConnected());
            }
        }

        CFsxSettingsComponent::~CFsxSettingsComponent()
        { }

        CSimulatorInfo CFsxSettingsComponent::getSimulator() const
        {
            const CSimulatorFsxCommon *fsx = this->getFsxSimulator();
            return fsx ? fsx->getSimulatorInfo() : m_simulator;
        }

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

        void CFsxSettingsComponent::onFsuipcChanged()
        {
            CSimulatorFsxCommon *fsx = this->getFsxSimulator();
            if (!fsx) { return; }
            fsx->useFsuipc(ui->cb_UseFsuipc->isChecked());
        }

        void CFsxSettingsComponent::onSBOffsetsChanged()
        {
            CSimulatorFsxCommon *fsx = this->getFsxSimulator();
            if (!fsx) { return; }
            fsx->setUsingSbOffsetValues(ui->cb_SBOffsets->isChecked());
        }

        void CFsxSettingsComponent::copyTerrainProbe()
        {
            if (!sGui || !sGui->getIContextSimulator() || sGui->isShuttingDown()) { return; }
            const CSimulatorInfo simulator(this->getSimulator());
            const CStatusMessageList msgs = sGui->getIContextSimulator()->copyFsxTerrainProbe(simulator);
            CLogMessage::preformatted(msgs);
            if (!m_mf) { m_mf = CGuiUtility::nextOverlayMessageFrame(this); }
            if (m_mf)  { m_mf->showOverlayMessages(msgs); }
        }

        CSimulatorFsxCommon *CFsxSettingsComponent::getFsxSimulator() const
        {
            if (!sGui || !sGui->getISimulator() || sGui->isShuttingDown()) { return nullptr; }
            ISimulator *simulator = sGui->getISimulator();
            if (!simulator->getSimulatorInfo().isFsxP3DFamily()) { return nullptr; }
            if (simulator->getSimulatorInfo() != m_simulator) { return nullptr; }
            CSimulatorFsxCommon *fsx = static_cast<CSimulatorFsxCommon *>(simulator); // wonder why qobject_cast does not work here
            return fsx;
        }
    } // ns
} // ns
