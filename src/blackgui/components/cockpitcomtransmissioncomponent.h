/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_COCKPITCOMPONENTTRANSMISSION_H
#define BLACKGUI_COMPONENTS_COCKPITCOMPONENTTRANSMISSION_H

#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackgui/blackguiexport.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CCockpitComTransmissionComponent;
}
namespace BlackGui::Components
{
    //! Transmission for COM units
    class BLACKGUI_EXPORT CCockpitComTransmissionComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCockpitComTransmissionComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CCockpitComTransmissionComponent() override;

        //! Update given COM system
        void updateComSystem(BlackMisc::Aviation::CComSystem &comSystem, BlackMisc::Aviation::CComSystem::ComUnit comUnit) const;

        //! Set values of given COM system
        void setComSystem(const BlackMisc::Aviation::CComSystem &comSystem, BlackMisc::Aviation::CComSystem::ComUnit comUnit);

        //! Set botb systems
        void setComSystems(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

    signals:
        //! Values changed for unit
        void changedValues(BlackMisc::Aviation::CComSystem::ComUnit unit);

    private:
        //! Save clicked
        void onSave();

        QScopedPointer<Ui::CCockpitComTransmissionComponent> ui;
    };
} // ns

#endif // guard
