// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_FSXCOMMON_FSXSETTINGSCOMPONENT_H
#define BLACKSIMPLUGIN_FSXCOMMON_FSXSETTINGSCOMPONENT_H

#include "plugins/simulator/fsxcommon/fsxcommonexport.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CFsxSettingsComponent;
}
namespace BlackGui
{
    class COverlayMessagesFrame;
}
namespace BlackSimPlugin::FsxCommon
{
    class CSimulatorFsxCommon;

    //! FSX/P3D settings
    class FSXCOMMON_EXPORT CFsxSettingsComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CFsxSettingsComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CFsxSettingsComponent() override;

        //! Simulator, P3D/FSX
        void setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) { m_simulator = simulator; }

        //! Represented simulator
        BlackMisc::Simulation::CSimulatorInfo getSimulator() const;

        //! Update the values
        void refresh();

    private:
        //! Trace checkbox changed
        void onSimConnectTraceChanged();

        //! Terrain probe checkbox changed
        void onEnableTerrainProbeChanged();

        //! Using simulated objects
        void onSimulatedObjectChanged();

        //! Offsets on/off
        void onSBOffsetsChanged();

        //! Copy the terrain probe
        void copyTerrainProbe();

        //! Status has been changed
        void onSimulatorStatusChanged(int status);

        //! Plugin changed
        void onSimulatorPluginChanged(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

        //! Access the concrete implementation
        CSimulatorFsxCommon *getFsxOrP3DSimulator() const;

        BlackMisc::Simulation::CSimulatorInfo m_simulator { "FSX" };
        BlackGui::COverlayMessagesFrame *m_mf = nullptr;
        QScopedPointer<Ui::CFsxSettingsComponent> ui;
    };
} // ns

#endif // guard
