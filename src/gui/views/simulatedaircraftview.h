// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_SIMULATEDAIRCRAFTVIEW_H
#define SWIFT_GUI_VIEWS_SIMULATEDAIRCRAFTVIEW_H

#include <QObject>

#include "gui/models/simulatedaircraftlistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewcallsignobjects.h"
#include "misc/pq/angle.h"
#include "misc/simulation/simulatedaircraftlist.h"

namespace swift::misc
{
    namespace aviation
    {
        class CCallsign;
    }
    namespace simulation
    {
        class CSimulatedAircraft;
    }
} // namespace swift::misc

namespace swift::core::context
{
    class IContextSimulator;
    class IContextNetwork;
} // namespace swift::core::context
namespace swift::gui
{
    namespace menus
    {
        class CMenuActions;
    }
    namespace views
    {
        class CFlightPlanDialog;

        //! Aircraft view
        class SWIFT_GUI_EXPORT CSimulatedAircraftView :
            public CViewWithCallsignObjects<models::CSimulatedAircraftListModel>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CSimulatedAircraftView(QWidget *parent = nullptr);

            //! Mode
            void setAircraftMode(models::CSimulatedAircraftListModel::AircraftMode mode);

            //! Configure the menu
            void configureMenu(bool menuRecalculate, bool menuHighlightAndFollow, bool menuEnableAircraft, bool menuFastPositionUpdates, bool menuGndFlag, bool menuFlightPlan);

            //! Configure fast position updates menu
            void configureMenuFastPositionUpdates(bool menuFastPositionUpdates);

        signals:
            //! Request a text message
            void requestTextMessageWidget(const swift::misc::aviation::CCallsign &callsign);

            //! Disable for matching
            void requestTempDisableModelsForMatching(const swift::misc::simulation::CAircraftModelList &models);

        protected:
            //! \copydoc CViewBase::customMenu
            virtual void customMenu(menus::CMenuActions &menuActions) override;

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
            void enableOrDisableAircraft(const swift::misc::simulation::CSimulatedAircraftList &aircraft, bool newEnabled);

            //! Follow aircraft in simulator
            void followAircraftInSimulator(const swift::misc::simulation::CSimulatedAircraft &aircraft);

            //! Enable aircraft parts
            void enableParts(const swift::misc::simulation::CSimulatedAircraft &aircraft, bool enabled);

            //! Set pitch
            void setPitchOnGround(const swift::misc::simulation::CSimulatedAircraft &aircraft, const swift::misc::physical_quantities::CAngle &pitch);

            //! Aircraft supporting parts?
            bool isSupportingAircraftParts(const swift::misc::aviation::CCallsign &cs) const;

            //! Fully recalculate all aircraft
            void recalculateAllAircraft();

            //! Match all aircraft again
            void doMatchingsAgain();

            //! Match all aircraft again
            void doMatchingsAgainForSelected();

            //! Enable fast position updates
            void enableFastPositionUpdates(const swift::misc::simulation::CSimulatedAircraft &aircraft);

            //! Update if aircraft is enabled
            void updateAircraftEnabled(const swift::misc::simulation::CSimulatedAircraft &aircraft);

            //! Update the gnd.flag support
            void updateAircraftSupportingGndFLag(const swift::misc::simulation::CSimulatedAircraft &aircraft);

            //! FP dialog
            void showFlightPlanDialog();

            //! Simulator context
            static swift::core::context::IContextSimulator *simulatorContext();

            //! Network context
            static swift::core::context::IContextNetwork *networkContext();

            bool m_withRecalculate = true;
            bool m_withMenuHighlightAndFollow = true;
            bool m_withMenuEnableAircraft = true;
            bool m_withMenuEnableGndFlag = true;
            bool m_withMenuFastPosition = true;
            bool m_withMenuFlightPlan = true;

            CFlightPlanDialog *m_fpDialog = nullptr;
        };
    } // namespace views
} // namespace swift::gui
#endif // guard
