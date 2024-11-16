// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_FSXCOMMON_FSXSETTINGSCOMPONENT_H
#define SWIFT_SIMPLUGIN_FSXCOMMON_FSXSETTINGSCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "misc/simulation/simulatorinfo.h"
#include "misc/simulation/simulatorplugininfo.h"
#include "plugins/simulator/fsxcommon/fsxcommonexport.h"

namespace Ui
{
    class CFsxSettingsComponent;
}
namespace swift::gui
{
    class COverlayMessagesFrame;
}
namespace swift::simplugin::fsxcommon
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
        void setSimulator(const swift::misc::simulation::CSimulatorInfo &simulator) { m_simulator = simulator; }

        //! Represented simulator
        swift::misc::simulation::CSimulatorInfo getSimulator() const;

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

        //! Status has been changed
        void onSimulatorStatusChanged(int status);

        //! Plugin changed
        void onSimulatorPluginChanged(const swift::misc::simulation::CSimulatorPluginInfo &info);

        //! Access the concrete implementation
        CSimulatorFsxCommon *getFsxOrP3DSimulator() const;

        swift::misc::simulation::CSimulatorInfo m_simulator { "FSX" };
        swift::gui::COverlayMessagesFrame *m_mf = nullptr;
        QScopedPointer<Ui::CFsxSettingsComponent> ui;
    };
} // namespace swift::simplugin::fsxcommon

#endif // guard
