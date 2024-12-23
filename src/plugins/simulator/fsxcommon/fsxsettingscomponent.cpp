// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "fsxsettingscomponent.h"

#include <QPointer>
#include <QTimer>

#include "simulatorfsxcommon.h"
#include "ui_fsxsettingscomponent.h"

#include "config/buildconfig.h"
#include "core/context/contextsimulator.h"
#include "gui/guiapplication.h"
#include "gui/overlaymessagesframe.h"

using namespace swift::core;
using namespace swift::core::context;
using namespace swift::gui;
using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::config;

namespace swift::simplugin::fsxcommon
{
    CFsxSettingsComponent::CFsxSettingsComponent(QWidget *parent) : QFrame(parent), ui(new Ui::CFsxSettingsComponent)
    {
        ui->setupUi(this);
        ui->cb_TraceSimConnectCalls->setChecked(false);

        connect(ui->cb_AddingAsSimulatedObject, &QCheckBox::released, this,
                &CFsxSettingsComponent::onSimulatedObjectChanged);
        connect(ui->cb_TraceSimConnectCalls, &QCheckBox::released, this,
                &CFsxSettingsComponent::onSimConnectTraceChanged);
        connect(ui->cb_EnableTerrainProbe, &QCheckBox::released, this,
                &CFsxSettingsComponent::onEnableTerrainProbeChanged);
        connect(ui->cb_SBOffsets, &QCheckBox::released, this, &CFsxSettingsComponent::onSBOffsetsChanged);
        connect(ui->pb_Refresh, &QPushButton::released, this, &CFsxSettingsComponent::refresh);

        if (sGui && sGui->getIContextSimulator())
        {
            connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this,
                    &CFsxSettingsComponent::onSimulatorStatusChanged, Qt::QueuedConnection);
            connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorPluginChanged, this,
                    &CFsxSettingsComponent::onSimulatorPluginChanged, Qt::QueuedConnection);
        }

        QPointer<CFsxSettingsComponent> myself(this);
        QTimer::singleShot(2000, this, [=] {
            if (!sGui || !myself || sGui->isShuttingDown()) { return; }
            this->refresh();
        });
    }

    CFsxSettingsComponent::~CFsxSettingsComponent() {}

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
        ui->cb_EnableTerrainProbe->setEnabled(CBuildConfig::isRunningOnWindowsNtPlatform() &&
                                              (CBuildConfig::buildWordSize() == 32) && localSim);
        ui->cb_SBOffsets->setEnabled(localSim);
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
} // namespace swift::simplugin::fsxcommon
