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

#include "blackgui/blackguiexport.h"
#include "blackcore/simulatorcommon.h"
#include "blackmisc/aviation/callsign.h"
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
        class BLACKGUI_EXPORT CInterpolationLogDisplay : public QFrame
        {
            Q_OBJECT

        public:
            //! Tabs
            enum Tab
            {
                TabFlow,
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

            //! Slider interval
            void onSliderChanged(int timeSecs);

            //! Callsign has been changed
            void onCallsignEntered();

            //! Toggle start/stop
            void toggleStartStop();

            //! Start displaying
            void start();

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

            //! \copydoc BlackCore::CSimulatorCommon::resetAircraftStatistics
            void resetStatistics();

            //! Clear
            void clear();

            QScopedPointer<Ui::CInterpolationLogDisplay> ui;
            QTimer m_updateTimer;
            QPointer<BlackCore::CSimulatorCommon> m_simulatorCommon; //!< related simulator
            QPointer<BlackCore::CAirspaceMonitor> m_airspaceMonitor; //!< related airspace monitor
            BlackMisc::Aviation::CCallsign m_callsign; //!< current callsign
            int m_elvRequested = 0;
            int m_elvReceived = 0;

            static const QString &startText();
            static const QString &stopText();
        };
    } // ns
} // ns

#endif // guard
