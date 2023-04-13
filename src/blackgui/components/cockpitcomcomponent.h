/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_COCKPITCOMCOMPONENT_H
#define BLACKGUI_COMPONENTS_COCKPITCOMCOMPONENT_H

#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/blackguiexport.h"

#include "blackmisc/aviation/selcal.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/identifier.h"
#include "blackmisc/simulation/simulatedaircraft.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

class QPaintEvent;

namespace Ui
{
    class CCockpitComComponent;
}
namespace BlackMisc::Aviation
{
    class CComSystem;
}
namespace BlackGui::Components
{
    //! The main cockpit area
    class BLACKGUI_EXPORT CCockpitComComponent :
        public QFrame,
        public BlackMisc::CIdentifiable,
        public CEnableForDockWidgetInfoArea
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCockpitComComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CCockpitComComponent() override;

        //! Set to BlackMisc::Aviation::CTransponder::StateIdent
        void setTransponderModeStateIdent();

    signals:
        //! \copydoc BlackGui::Components::CTransponderModeSelector::transponderModeChanged
        void transponderModeChanged(BlackMisc::Aviation::CTransponder::TransponderMode newMode);

        //! \copydoc BlackGui::Components::CTransponderModeSelector::transponderStateIdentEnded
        void transponderStateIdentEnded();

        //! @{
        //! Request COM text messages
        void requestCom1TextMessage();
        void requestCom2TextMessage();
        //! @}

    protected:
        //! \copydoc QWidget::paintEvent
        virtual void paintEvent(QPaintEvent *event) override;

    private:
        //! Cockpit values have been changed in GUI
        void testSelcal();

        //! Get own aircraft
        BlackMisc::Simulation::CSimulatedAircraft getOwnAircraft() const;

        //! Cockpit updates
        bool updateOwnCockpitInContext(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft);

        //! SELCAL changed in GUI
        void updateSelcalInContext(const BlackMisc::Aviation::CSelcal &selcal);

        //! SELCAL was changed
        void updateSelcalFromContext(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator);

        //! Update cockpit from context
        void updateCockpitFromContext(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft, const BlackMisc::CIdentifier &originator);

        //! Update the cockpit from aircraft context
        void forceCockpitUpdateFromOwnAircraftContext();

        //! Stations changed
        void onAtcStationsChanged();

        QScopedPointer<Ui::CCockpitComComponent> ui;
    };
} // namespace

#endif // guard
