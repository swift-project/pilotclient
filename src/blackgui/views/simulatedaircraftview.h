// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_SIMULATEDAIRCRAFTVIEW_H
#define BLACKGUI_VIEWS_SIMULATEDAIRCRAFTVIEW_H

#include "blackgui/views/viewcallsignobjects.h"
#include "blackgui/models/simulatedaircraftlistmodel.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/pq/angle.h"

#include <QObject>

namespace BlackMisc
{
    namespace Aviation
    {
        class CCallsign;
    }
    namespace Simulation
    {
        class CSimulatedAircraft;
    }
}

namespace BlackCore::Context
{
    class IContextSimulator;
    class IContextNetwork;
}
namespace BlackGui
{
    namespace Menus
    {
        class CMenuActions;
    }
    namespace Views
    {
        class CFlightPlanDialog;

        //! Aircraft view
        class BLACKGUI_EXPORT CSimulatedAircraftView :
            public CViewWithCallsignObjects<Models::CSimulatedAircraftListModel>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CSimulatedAircraftView(QWidget *parent = nullptr);

            //! Mode
            void setAircraftMode(Models::CSimulatedAircraftListModel::AircraftMode mode);

            //! Configure the menu
            void configureMenu(bool menuRecalculate, bool menuHighlightAndFollow, bool menuEnableAircraft, bool menuFastPositionUpdates, bool menuGndFlag, bool menuFlightPlan);

            //! Configure fast position updates menu
            void configureMenuFastPositionUpdates(bool menuFastPositionUpdates);

        signals:
            //! Request a text message
            void requestTextMessageWidget(const BlackMisc::Aviation::CCallsign &callsign);

            //! Disable for matching
            void requestTempDisableModelsForMatching(const BlackMisc::Simulation::CAircraftModelList &models);

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

            //! Follow in simulator
            void requestFollowInSimulator();

            //! @{
            //! Enable/disable parts
            void requestEnableParts();
            void requestDisableParts();
            //! @}

            //! @{
            //! Enable/disable parts
            void request0PitchOnGround();
            void requestNullPitchOnGround();
            //! @}

            //! Request temp disabling of matching models
            void requestTempDisable();

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

            //! Enable aircraft parts
            void enableParts(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, bool enabled);

            //! Set pitch
            void setPitchOnGround(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::PhysicalQuantities::CAngle &pitch);

            //! Aircraft supporting parts?
            bool isSupportingAircraftParts(const BlackMisc::Aviation::CCallsign &cs) const;

            //! Fully recalculate all aircraft
            void recalculateAllAircraft();

            //! Match all aircraft again
            void doMatchingsAgain();

            //! Match all aircraft again
            void doMatchingsAgainForSelected();

            //! Enable fast position updates
            void enableFastPositionUpdates(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Update if aircraft is enabled
            void updateAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Update the gnd.flag support
            void updateAircraftSupportingGndFLag(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! FP dialog
            void showFlightPlanDialog();

            //! Simulator context
            static BlackCore::Context::IContextSimulator *simulatorContext();

            //! Network context
            static BlackCore::Context::IContextNetwork *networkContext();

            bool m_withRecalculate = true;
            bool m_withMenuHighlightAndFollow = true;
            bool m_withMenuEnableAircraft = true;
            bool m_withMenuEnableGndFlag = true;
            bool m_withMenuFastPosition = true;
            bool m_withMenuFlightPlan = true;

            CFlightPlanDialog *m_fpDialog = nullptr;
        };
    } // ns
} // ns
#endif // guard
