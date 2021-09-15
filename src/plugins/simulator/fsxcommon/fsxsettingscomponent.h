/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FSXCOMMON_FSXSETTINGSCOMPONENT_H
#define BLACKSIMPLUGIN_FSXCOMMON_FSXSETTINGSCOMPONENT_H

#include "plugins/simulator/fsxcommon/fsxcommonexport.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CFsxSettingsComponent; }
namespace BlackGui { class COverlayMessagesFrame; }
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

        //! FSUIPC setting changed
        void onFsuipcChanged();

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
