/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWS_SIMULATEDAIRCRAFTVIEW_H
#define BLACKGUI_VIEWS_SIMULATEDAIRCRAFTVIEW_H

#include "blackgui/views/viewcallsignobjects.h"
#include "blackgui/models/simulatedaircraftlistmodel.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include <QObject>

namespace BlackMisc
{
    namespace Aviation { class CCallsign; }
    namespace Simulation { class CSimulatedAircraft; }
}

namespace BlackCore
{
    namespace Context
    {
        class IContextSimulator;
        class IContextNetwork;
    }
}
namespace BlackGui
{
    namespace Menus { class CMenuActions; }
    namespace Views
    {
        //! Aircraft view
        class BLACKGUI_EXPORT CSimulatedAircraftView :
            public CViewWithCallsignObjects<Models::CSimulatedAircraftListModel, BlackMisc::Simulation::CSimulatedAircraftList, BlackMisc::Simulation::CSimulatedAircraft>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CSimulatedAircraftView(QWidget *parent = nullptr);

            //! Mode
            void setAircraftMode(Models::CSimulatedAircraftListModel::AircraftMode mode);

            //! configure the menu
            void configureMenu(bool menuHighlightAndFollow, bool menuEnableAircraft, bool menuFastPositionUpdates, bool menuGndFlag);

        signals:
            //! Request a text message
            void requestTextMessageWidget(const BlackMisc::Aviation::CCallsign &callsign);

        protected:
            //! \copydoc CViewBase::customMenu
            virtual void customMenu(Menus::CMenuActions &menuActions) override;

        private:
            //! Request text message for selected aircraft
            void requestTextMessage();

            //! Toogle aircraft state for selected aircraft
            void toggleEnabledAircraft();

            //! Toggle fast position updates for selected aircraft
            void toggleFastPositionUpdates();

            //! Enable gnd. flag (enable gnd flag capability for given aircraft)
            void toggleSupportingGndFlag();

            //! Highlight aircraft in simulator
            void requestHighlightInSimulator();

            //! Follow in simulator
            void requestFollowInSimulator();

            //! Show position log for selected aircraft
            void showPositionLogInSimulator();

            //! Enable all disabled aircraft
            void enableAllDisabledAircraft();

            //! Disable all enabled aircraft
            void disableAllEnabledAircraft();

            //! Enable all unrendered aircraft
            void reEnableAllUnrenderedAircraft();

            //! Enable or disable aircraft
            void enableOrDisableAircraft(const BlackMisc::Simulation::CSimulatedAircraftList &aircraft, bool newEnabled);

            //! Follow aircraft in simulator
            void followAircraftInSimulator(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Highlight in simulator
            void highlightInSimulator(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Enable fast position updates
            void enableFastPositionUpdates(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Update if aircraft is enabled
            void updateAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Update the gnd.flag support
            void updateAircraftSupportingGndFLag(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Simulator context
            static BlackCore::Context::IContextSimulator *simulatorContext();

            //! Network context
            static BlackCore::Context::IContextNetwork *networkContext();

            bool m_withMenuHighlightAndFollow = true;
            bool m_withMenuEnableAircraft     = true;
            bool m_withMenuEnableGndFlag      = true;
            bool m_withMenuFastPosition       = true;
        };
    } // ns
} // ns
#endif // guard
