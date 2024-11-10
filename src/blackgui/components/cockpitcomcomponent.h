// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_COCKPITCOMCOMPONENT_H
#define BLACKGUI_COMPONENTS_COCKPITCOMCOMPONENT_H

#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/blackguiexport.h"

#include "misc/aviation/selcal.h"
#include "misc/aviation/transponder.h"
#include "misc/identifiable.h"
#include "misc/identifier.h"
#include "misc/simulation/simulatedaircraft.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

class QPaintEvent;

namespace Ui
{
    class CCockpitComComponent;
}
namespace swift::misc::aviation
{
    class CComSystem;
}
namespace BlackGui::Components
{
    //! The main cockpit area
    class BLACKGUI_EXPORT CCockpitComComponent :
        public QFrame,
        public swift::misc::CIdentifiable,
        public CEnableForDockWidgetInfoArea
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCockpitComComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CCockpitComComponent() override;

        //! Set to swift::misc::aviation::CTransponder::StateIdent
        void setTransponderModeStateIdent();

    signals:
        //! \copydoc BlackGui::Components::CTransponderModeSelector::transponderModeChanged
        void transponderModeChanged(swift::misc::aviation::CTransponder::TransponderMode newMode);

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
        swift::misc::simulation::CSimulatedAircraft getOwnAircraft() const;

        //! Cockpit updates
        bool updateOwnCockpitInContext(const swift::misc::simulation::CSimulatedAircraft &ownAircraft);

        //! SELCAL changed in GUI
        void updateSelcalInContext(const swift::misc::aviation::CSelcal &selcal);

        //! SELCAL was changed
        void updateSelcalFromContext(const swift::misc::aviation::CSelcal &selcal, const swift::misc::CIdentifier &originator);

        //! Update cockpit from context
        void updateCockpitFromContext(const swift::misc::simulation::CSimulatedAircraft &ownAircraft, const swift::misc::CIdentifier &originator);

        //! Update the cockpit from aircraft context
        void forceCockpitUpdateFromOwnAircraftContext();

        //! Stations changed
        void onAtcStationsChanged();

        QScopedPointer<Ui::CCockpitComComponent> ui;
    };
} // namespace

#endif // guard
