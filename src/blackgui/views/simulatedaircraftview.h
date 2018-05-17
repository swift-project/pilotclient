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

#include "blackgui/blackguiexport.h"
#include "blackgui/views/viewcallsignobjects.h"
#include "blackgui/models/simulatedaircraftlistmodel.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"

#include <QObject>

class QWidget;

namespace BlackMisc
{
    namespace Aviation { class CCallsign; }
    namespace Simulation { class CSimulatedAircraft; }
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

            //! Request enable / disable fast position updates, \sa CSimulatedAircraft::fastPositionUpdates
            void requestFastPositionUpdates(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Request to enable / disable aircraft, \sa CSimulatedAircraft::isEnabled
            void requestEnableAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Request to enable / disable gnd.flag, \sa CSimulatedAircraft::isSupportingGndFlag
            void requestSupportingGndFlag(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Highlight given aircraft in simulator
            void requestHighlightInSimulator(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Highlight given aircraft in simulator
            void requestFollowInSimulator(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        protected:
            //! \copydoc CViewBase::customMenu
            virtual void customMenu(BlackGui::Menus::CMenuActions &menuActions) override;

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
            void reqHighlightInSimulator();

            //! Follow in simulator
            void reqFollowInSimulator();

            //! Enable gnd. flag capability for server
            void reqEnableGndFlagForServer();

            //! Show position log for selected aircraft
            void showPositionLogInSimulator();

            //! Enable all disabled aircraft
            void enableAllDisabledAircraft();

            //! Enable all unrendered aircraft
            void reEnableAllUnrenderedAircraft();

            bool m_withMenuHighlightAndFollow = true;
            bool m_withMenuEnableAircraft = true;
            bool m_withMenuEnableGndFlag = true;
            bool m_withMenuFastPosition = true;
        };
    } // ns
} // ns
#endif // guard
