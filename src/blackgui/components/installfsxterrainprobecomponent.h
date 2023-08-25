// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_INSTALLFSXTERRAINPROBECOMPONENT_H
#define BLACKGUI_COMPONENTS_INSTALLFSXTERRAINPROBECOMPONENT_H

#include "blackmisc/simulation/settings/simulatorsettings.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CInstallFsxTerrainProbeComponent;
}
namespace BlackGui::Components
{
    //! Install the FSX/P3D terrain probe
    class CInstallFsxTerrainProbeComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CInstallFsxTerrainProbeComponent(QWidget *parent = nullptr);

        //! Destructors
        virtual ~CInstallFsxTerrainProbeComponent();

    private:
        QScopedPointer<Ui::CInstallFsxTerrainProbeComponent> ui;

        //! Select the directory
        void selectSimObjectsDir();

        //! Copy probe
        void copyProbe();

        //! Simulator has been changed
        void onSimulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        BlackMisc::Simulation::Settings::CMultiSimulatorSettings m_simulatorSettings { this }; //!< for SimObjects directories
    };
} // ns

#endif // guard
