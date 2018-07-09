/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_INTERPOLATIONLOGDISPLAY_H
#define BLACKGUI_COMPONENTS_INTERPOLATIONLOGDISPLAY_H

#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"
#include "blackcore/simulatorcommon.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/identifiable.h"
#include <QFrame>
#include <QTimer>
#include <QScopedPointer>
#include <QPointer>

namespace BlackCore { class CAirspaceMonitor; }
namespace Ui { class CInterpolationLogDisplay; }
namespace BlackGui
{
    namespace Components
    {
        /**
         * Display live data of interpolation
         */
        class BLACKGUI_EXPORT CInterpolationLogDisplay :
            public COverlayMessagesFrame,
            public BlackMisc::CIdentifiable
        {
            Q_OBJECT

        public:
            //! Tabs
            enum Tab
            {
                TabFlow,
                TabFlow2,
                TabInterpolation,
                TabTextLog
            };

            //! Constructor
            explicit CInterpolationLogDisplay(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CInterpolationLogDisplay();

            //! Set simulator
            void setSimulator(BlackCore::CSimulatorCommon *simulatorCommon);

            //! Set corresponding airspace monitor
            void setAirspaceMonitor(BlackCore::CAirspaceMonitor *airspaceMonitor);

            //! If possible link with airspace monitor
            void linkWithAirspaceMonitor();

        private:
            //! Update log.
            void updateLog();

            //! Get last interpolation
            void displayLastInterpolation();

            //! Slider interval
            void onSliderChanged(int timeSecs);

            //! Callsign has been changed
            void onCallsignEntered();

            //! Toggle start/stop
            void toggleStartStop();

            //! Display log in simulator
            void showLogInSimulator();

            //! Follow aircraft in simulator
            void followInSimulator();

            //! Start displaying
            bool start();

            //! Stop displaying
            void stop();

            //! Log the current callsign
            bool logCallsign(const BlackMisc::Aviation::CCallsign &cs) const;

            //! About to shutdown
            void onAboutToShutdown();

            //! Simulator unloaded
            void onSimulatorUnloaded();

            //! \copydoc BlackCore::CAirspaceMonitor::addedAircraftSituation
            void onSituationAdded(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! \copydoc BlackCore::CAirspaceMonitor::addedAircraftSituation
            void onPartsAdded(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts);

            //! \copydoc BlackCore::CSimulatorCommon::simulatorStatusChanged
            void onSimulatorStatusChanged(BlackCore::ISimulator::SimulatorStatus status);

            //! \copydoc BlackCore::CSimulatorCommon::receivedRequestedElevation
            void onElevationReceived(const BlackMisc::Geo::CElevationPlane &plane, const BlackMisc::Aviation::CCallsign &callsign);

            //! \copydoc BlackCore::CSimulatorCommon::requestedElevation
            void onElevationRequested(const BlackMisc::Aviation::CCallsign &callsign);

            //! Call the callback of requested elevations as it would come from the simulator
            void onInjectElevation();

            //! \copydoc BlackCore::CSimulatorCommon::resetAircraftStatistics
            void resetStatistics();

            //! Clear
            void clear();

            //! Check if can do logging, otherwise stop and display message
            bool checkLogPrerequisites();

            //! Init the parts view
            void initPartsView();

            //! Display elevation requested/receive
            void displayElevationRequestReceive();

            //! Do request elevation
            void requestElevationClicked();

            //! Do request elevation
            void requestElevation(const BlackMisc::Aviation::CAircraftSituation &situation);

            QScopedPointer<Ui::CInterpolationLogDisplay> ui;
            QTimer m_updateTimer;
            QPointer<BlackCore::CSimulatorCommon> m_simulatorCommon; //!< related simulator
            QPointer<BlackCore::CAirspaceMonitor> m_airspaceMonitor; //!< related airspace monitor
            BlackMisc::Aviation::CCallsign m_callsign; //!< current callsign
            int m_elvRequested = 0; //!< counted via signal
            int m_elvReceived = 0;  //!< counted via signal

            static const QString &startText();
            static const QString &stopText();
        };
    } // ns
} // ns

#endif // guard
