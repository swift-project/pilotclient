// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_COCKPITCOMPONENTTRANSMISSION_H
#define SWIFT_GUI_COMPONENTS_COCKPITCOMPONENTTRANSMISSION_H

#include "misc/simulation/simulatedaircraft.h"
#include "misc/aviation/comsystem.h"
#include "gui/swiftguiexport.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CCockpitComTransmissionComponent;
}
namespace swift::gui::components
{
    //! Transmission for COM units
    class SWIFT_GUI_EXPORT CCockpitComTransmissionComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCockpitComTransmissionComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CCockpitComTransmissionComponent() override;

        //! Update given COM system
        void updateComSystem(swift::misc::aviation::CComSystem &comSystem, swift::misc::aviation::CComSystem::ComUnit comUnit) const;

        //! Set values of given COM system
        void setComSystem(const swift::misc::aviation::CComSystem &comSystem, swift::misc::aviation::CComSystem::ComUnit comUnit);

        //! Set botb systems
        void setComSystems(const swift::misc::simulation::CSimulatedAircraft &aircraft);

    signals:
        //! Values changed for unit
        void changedValues(swift::misc::aviation::CComSystem::ComUnit unit);

    private:
        //! Save clicked
        void onSave();

        QScopedPointer<Ui::CCockpitComTransmissionComponent> ui;
    };
} // ns

#endif // guard