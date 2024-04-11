// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "fsxsettingscomponent.h"
#include "ui_fsxsettingscomponent.h"
#include "simulatorfsxcommon.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "blackconfig/buildconfig.h"

#include <QPointer>
#include <QTimer>

using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackConfig;

namespace BlackSimPlugin::FsxCommon
{
    CFsxSettingsComponent::CFsxSettingsComponent(QWidget *parent) : QFrame(parent),
                                                                    ui(new Ui::CFsxSettingsComponent)
    {
        ui->setupUi(this);
        ui->cb_TraceSimConnectCalls->setChecked(false);

        connect(ui->cb_AddingAsSimulatedObject, &QCheckBox::released, this, &CFsxSettingsComponent::onSimulatedObjectChanged);
        connect(ui->cb_TraceSimConnectCalls, &QCheckBox::released, this, &CFsxSettingsComponent::onSimConnectTraceChanged);
        connect(ui->cb_EnableTerrainProbe, &QCheckBox::released, this, &CFsxSettingsComponent::onEnableTerrainProbeChanged);
        connect(ui->cb_SBOffsets, &QCheckBox::released, this, &CFsxSettingsComponent::onSBOffsetsChanged);
        connect(ui->pb_CopyTerrainProbe, &QPushButton::released, this, &CFsxSettingsComponent::copyTerrainProbe);
        connect(ui->pb_Refresh, &QPushButton::released, this, &CFsxSettingsComponent::refresh);

        if (sGui && sGui->getIContextSimulator())
        {
            connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this, &CFsxSettingsComponent::onSimulatorStatusChanged, Qt::QueuedConnection);
            connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorPluginChanged, this, &CFsxSettingsComponent::onSimulatorPluginChanged, Qt::QueuedConnection);
        }

        QPointer<CFsxSettingsComponent> myself(this);
        QTimer::singleShot(2000, this, [=] {
            if (!sGui || !myself || sGui->isShuttingDown()) { return; }
            this->refresh();
        });
    }

    CFsxSettingsComponent::~CFsxSettingsComponent()
    {}

    CSimulatorInfo CFsxSettingsComponent::getSimulator() const
    {
        const CSimulatorFsxCommon *fsxOrP3D = this->getFsxOrP3DSimulator();
        return fsxOrP3D ? fsxOrP3D->getSimulatorInfo() : m_simulator;
    }

    void CFsxSettingsComponent::refresh()
    {
        const CSimulatorFsxCommon *fsxOrP3D = this->getFsxOrP3DSimulator();
        const bool localSim = fsxOrP3D;
        if (fsxOrP3D)
        {
            ui->cb_TraceSimConnectCalls->setChecked(fsxOrP3D->isTraceSendId());
            ui->cb_EnableTerrainProbe->setChecked(fsxOrP3D->isUsingFsxTerrainProbe());
            ui->cb_SBOffsets->setChecked(fsxOrP3D->isUsingSbOffsetValues());
            ui->cb_AddingAsSimulatedObject->setChecked(fsxOrP3D->isAddingAsSimulatedObjectEnabled());
        }

        ui->lbl_NoLocalSimulator->setVisible(!localSim);
        ui->cb_TraceSimConnectCalls->setEnabled(localSim);
        ui->cb_EnableTerrainProbe->setEnabled(localSim);
        ui->cb_SBOffsets->setEnabled(localSim);

        const bool terrainProbe = CBuildConfig::isRunningOnWindowsNtPlatform() && (CBuildConfig::buildWordSize() == 32);
        ui->cb_EnableTerrainProbe->setEnabled(terrainProbe);
        ui->pb_CopyTerrainProbe->setVisible(terrainProbe);
    }

    void CFsxSettingsComponent::onSimConnectTraceChanged()
    {
        CSimulatorFsxCommon *fsxOrP3D = this->getFsxOrP3DSimulator();
        if (!fsxOrP3D) { return; }
        fsxOrP3D->setTractingSendId(ui->cb_TraceSimConnectCalls->isChecked());
    }

    void CFsxSettingsComponent::onSimulatedObjectChanged()
    {
        CSimulatorFsxCommon *fsxOrP3D = this->getFsxOrP3DSimulator();
        if (!fsxOrP3D) { return; }
        fsxOrP3D->setAddingAsSimulatedObjectEnabled(ui->cb_AddingAsSimulatedObject->isChecked());
    }

    void CFsxSettingsComponent::onEnableTerrainProbeChanged()
    {
        CSimulatorFsxCommon *fsxOrP3D = this->getFsxOrP3DSimulator();
        if (!fsxOrP3D) { return; }
        fsxOrP3D->setUsingFsxTerrainProbe(ui->cb_EnableTerrainProbe->isChecked());
    }

    void CFsxSettingsComponent::onSBOffsetsChanged()
    {
        CSimulatorFsxCommon *fsxOrP3D = this->getFsxOrP3DSimulator();
        if (!fsxOrP3D) { return; }
        fsxOrP3D->setUsingSbOffsetValues(ui->cb_SBOffsets->isChecked());
    }

    void CFsxSettingsComponent::copyTerrainProbe()
    {
        if (!sGui || !sGui->getIContextSimulator() || sGui->isShuttingDown()) { return; }
        const CSimulatorInfo simulator(this->getSimulator());
        const CStatusMessageList msgs = sGui->getIContextSimulator()->copyFsxTerrainProbe(simulator);
        CLogMessage::preformatted(msgs);
        if (!m_mf) { m_mf = CGuiUtility::nextOverlayMessageFrame(this); }
        if (m_mf) { m_mf->showOverlayMessages(msgs); }
    }

    void CFsxSettingsComponent::onSimulatorStatusChanged(int status)
    {
        Q_UNUSED(status);
        this->refresh();
    }

    void CFsxSettingsComponent::onSimulatorPluginChanged(const CSimulatorPluginInfo &info)
    {
        Q_UNUSED(info);
        this->refresh();
    }

    CSimulatorFsxCommon *CFsxSettingsComponent::getFsxOrP3DSimulator() const
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return nullptr; }
        const CSimulatorPluginInfo plugin = sGui->getIContextSimulator()->getSimulatorPluginInfo();
        if (plugin.isEmulatedPlugin()) { return nullptr; } // cast would fail

        // ISimulator can only be obtained in local environment, not distributed UI
        if (!sGui->hasSimulator()) { return nullptr; }

        ISimulator *simulator = sGui->getISimulator().data();
        if (!simulator || simulator->isEmulatedDriver()) { return nullptr; }
        if (!simulator->getSimulatorInfo().isFsxP3DFamily()) { return nullptr; }
        if (simulator->getSimulatorInfo() != m_simulator) { return nullptr; }

        //! \todo KB 11/18 wonder why qobject_cast does not work here
        CSimulatorFsxCommon *fsx = static_cast<CSimulatorFsxCommon *>(simulator);
        return fsx;
    }
} // ns
