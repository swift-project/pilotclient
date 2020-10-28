/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_INTERPOLATIONLOGDISPLAY_H
#define BLACKGUI_COMPONENTS_INTERPOLATIONLOGDISPLAY_H

#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"
#include "blackcore/simulator.h"
#include "blackmisc/simulation/interpolationlogger.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/identifiable.h"

#include <QStringListModel>
#include <QFrame>
#include <QTimer>
#include <QScopedPointer>
#include <QPointer>

namespace Ui { class CInterpolationLogDisplay; }
namespace BlackCore  { class CAirspaceMonitor; }
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
            //! Categories
            static const QStringList &getLogCategories();

            //! Tabs
            enum Tab
            {
                TabFlow,
                TabFlow2,
                TabInterpolation,
                TabLoopback,
                TabTextLog,
                TabElevation
            };

            //! Constructor
            explicit CInterpolationLogDisplay(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CInterpolationLogDisplay() override;

            //! Set simulator
            void setSimulator(BlackCore::ISimulator *simulator);

            //! Set corresponding airspace monitor
            void setAirspaceMonitor(BlackCore::CAirspaceMonitor *airspaceMonitor);

            //! If possible link with airspace monitor
            void linkWithAirspaceMonitor();

        private:
            //! Update log.
            void updateLog();

            //! Get last interpolation
            void getLogAmdDisplayLastInterpolation();

            //! Get last interpolation
            void displayLastInterpolation(const BlackMisc::Simulation::SituationLog &sLog);

            //! Display the loopback information
            void displayLoopback();

            //! Slider interval
            void onSliderChanged(int timeSecs);

            //! Callsign has been changed
            void onCallsignEntered();

            //! Use pseudo elevation
            void onPseudoElevationToggled(bool checked);

            //! Toggle start/stop
            void toggleStartStop();

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

            //! \copydoc BlackCore::ISimulator::simulatorStatusChanged
            void onSimulatorStatusChanged(BlackCore::ISimulator::SimulatorStatus status);

            //! \copydoc BlackCore::ISimulator::receivedRequestedElevation
            void onElevationReceived(const BlackMisc::Geo::CElevationPlane &elevationPlane, const BlackMisc::Aviation::CCallsign &callsign);

            //! \copydoc BlackCore::ISimulator::requestedElevation
            void onElevationRequested(const BlackMisc::Aviation::CCallsign &callsign);

            //! Call the callback of requested elevations as it would come from the simulator
            void onInjectElevation();

            //! Entering a count failed
            void onElevationHistoryCountFinished();

            //! \copydoc BlackCore::ISimulator::resetAircraftStatistics
            void resetStatistics();

            //! \copydoc BlackCore::ISimulator::resetLastSentValues
            void resetLastSentValues();

            //! Clear
            void clear();

            //! Clear elevation results
            void clearElevationResults();

            //! Clear log. command
            void clearLogCommand();

            //! Write log. command
            void writeLogCommand();

            //! Display log in simulator
            void logPosCommand();

            //! Check if can do logging, otherwise stop and display message
            bool checkLogPrerequisites();

            //! Init the parts view
            void initPartsView();

            //! Display elevation requested/receive
            void displayElevationRequestReceive();

            //! Display arbitrary elevation
            void displayArbitraryElevation(const BlackMisc::Geo::CElevationPlane &elevation);

            //! Do request elevation
            void requestElevationClicked();

            //! Do request elevation
            void requestElevation(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! Request arbitrary elevation
            void requestElevationAtPosition();

            //! Do a full interpolation cycle for a while
            void requestRecalculateAll();

            //! Can use the log. display?
            bool canLog() const;

            //! Pseudo callsign for CInterpolationLogDisplay::requestElevationAtPosition
            static const BlackMisc::Aviation::CCallsign &pseudoCallsignElevation();

            QScopedPointer<Ui::CInterpolationLogDisplay> ui;
            QTimer m_updateTimer;
            QPointer<BlackCore::ISimulator> m_simulator;                      //!< related simulator
            QPointer<BlackCore::CAirspaceMonitor> m_airspaceMonitor;          //!< related airspace monitor
            BlackMisc::Aviation::CAircraftSituationList m_lastInterpolations; //!< list of last interpolations
            BlackMisc::Aviation::CCallsign m_callsign; //!< current callsign

            int m_elvRequested = 0;         //!< counted via signal
            int m_elvReceived  = 0;         //!< counted via signal
            int m_elvRequestedLoggedCs = 0; //!< counted via signal
            int m_elvReceivedLoggedCs  = 0; //!< counted via signal

            int m_elvHistoryCount = -1;                //!< how many in history
            QStringListModel *m_elvHistoryModel = nullptr;

            static const QString &startText();
            static const QString &stopText();
        };
    } // ns
} // ns

#endif // guard
