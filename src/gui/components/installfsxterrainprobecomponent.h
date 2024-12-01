// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_INSTALLFSXTERRAINPROBECOMPONENT_H
#define SWIFT_GUI_COMPONENTS_INSTALLFSXTERRAINPROBECOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "misc/simulation/settings/simulatorsettings.h"

namespace Ui
{
    class CInstallFsxTerrainProbeComponent;
}
namespace swift::gui::components
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
        void onSimulatorChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

        swift::misc::simulation::settings::CMultiSimulatorSettings m_simulatorSettings {
            this
        }; //!< for SimObjects directories
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_INSTALLFSXTERRAINPROBECOMPONENT_H
