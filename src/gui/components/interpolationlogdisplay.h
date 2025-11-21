// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_INTERPOLATIONLOGDISPLAY_H
#define SWIFT_GUI_COMPONENTS_INTERPOLATIONLOGDISPLAY_H

#include <QFrame>
#include <QPointer>
#include <QScopedPointer>
#include <QStringListModel>
#include <QTimer>

#include "core/simulator.h"
#include "gui/overlaymessagesframe.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/aircraftsituationlist.h"
#include "misc/aviation/callsign.h"
#include "misc/identifiable.h"
#include "misc/logcategories.h"
#include "misc/simulation/interpolation/interpolationlogger.h"

namespace Ui
{
    class CInterpolationLogDisplay;
}
namespace swift::core
{
    class CAirspaceMonitor;
}
namespace swift::gui::components
{
    /*!
     * Display live data of interpolation
     */
    class SWIFT_GUI_EXPORT CInterpolationLogDisplay : public COverlayMessagesFrame, public swift::misc::CIdentifiable
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
        ~CInterpolationLogDisplay() override;

        //! Set simulator
        void setSimulator(swift::core::ISimulator *simulator);

        //! Set corresponding airspace monitor
        void setAirspaceMonitor(swift::core::CAirspaceMonitor *airspaceMonitor);

        //! If possible link with airspace monitor
        void linkWithAirspaceMonitor();

    private:
        //! Update log.
        void updateLog();

        //! Get last interpolation
        void getLogAmdDisplayLastInterpolation();

        //! Get last interpolation
        void displayLastInterpolation(const swift::misc::simulation::SituationLog &sLog);

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
        bool logCallsign(const swift::misc::aviation::CCallsign &cs) const;

        //! About to shutdown
        void onAboutToShutdown();

        //! Simulator unloaded
        void onSimulatorUnloaded();

        //! \copydoc swift::core::CAirspaceMonitor::addedAircraftSituation
        void onSituationAdded(const swift::misc::aviation::CAircraftSituation &situation);

        //! \copydoc swift::core::CAirspaceMonitor::addedAircraftSituation
        void onPartsAdded(const swift::misc::aviation::CCallsign &callsign,
                          const swift::misc::aviation::CAircraftParts &parts);

        //! \copydoc swift::core::ISimulator::simulatorStatusChanged
        void onSimulatorStatusChanged(swift::core::ISimulator::SimulatorStatus status);

        //! \copydoc swift::core::ISimulator::receivedRequestedElevation
        void onElevationReceived(const swift::misc::geo::CElevationPlane &elevationPlane,
                                 const swift::misc::aviation::CCallsign &callsign);

        //! \copydoc swift::core::ISimulator::requestedElevation
        void onElevationRequested(const swift::misc::aviation::CCallsign &callsign);

        //! Call the callback of requested elevations as it would come from the simulator
        void onInjectElevation();

        //! Entering a count failed
        void onElevationHistoryCountFinished();

        //! \copydoc swift::core::ISimulator::resetAircraftStatistics
        void resetStatistics();

        //! \copydoc swift::core::ISimulator::resetLastSentValues
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
        void displayArbitraryElevation(const swift::misc::geo::CElevationPlane &elevation);

        //! Do request elevation
        void requestElevationClicked();

        //! Do request elevation
        void requestElevation(const swift::misc::aviation::CAircraftSituation &situation);

        //! Request arbitrary elevation
        void requestElevationAtPosition();

        //! Do a full interpolation cycle for a while
        void requestRecalculateAll();

        //! Can use the log. display?
        bool canLog() const;

        //! Pseudo callsign for CInterpolationLogDisplay::requestElevationAtPosition
        static const swift::misc::aviation::CCallsign &pseudoCallsignElevation();

        QScopedPointer<Ui::CInterpolationLogDisplay> ui;
        QTimer m_updateTimer;
        QPointer<swift::core::ISimulator> m_simulator; //!< related simulator
        QPointer<swift::core::CAirspaceMonitor> m_airspaceMonitor; //!< related airspace monitor
        swift::misc::aviation::CAircraftSituationList m_lastInterpolations; //!< list of last interpolations
        swift::misc::aviation::CCallsign m_callsign; //!< current callsign

        int m_elvRequested = 0; //!< counted via signal
        int m_elvReceived = 0; //!< counted via signal
        int m_elvRequestedLoggedCs = 0; //!< counted via signal
        int m_elvReceivedLoggedCs = 0; //!< counted via signal

        int m_elvHistoryCount = -1; //!< how many in history
        QStringListModel *m_elvHistoryModel = nullptr;

        static const QString &startText();
        static const QString &stopText();
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_INTERPOLATIONLOGDISPLAY_H
