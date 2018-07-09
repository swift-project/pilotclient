/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_SIMULATOR_COMMON_H
#define BLACKCORE_SIMULATOR_COMMON_H

#include <QObject>
#include <QTimer>
#include <QReadWriteLock>
#include <QtGlobal>
#include <QMap>

#include "blackcore/aircraftmatcher.h"
#include "blackcore/blackcoreexport.h"
#include "blackcore/simulator.h"
#include "blackmisc/simulation/aircraftmodelsetloader.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/simulatorinternals.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/simulation/interpolationlogger.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/tokenbucket.h"
#include "blackmisc/connectionguard.h"

namespace BlackMisc
{
    class CSimpleCommandParser;

    namespace Aviation
    {
        class CAircraftParts;
        class CAircraftSituation;
        class CCallsign;
    }
    namespace Simulation
    {
        class CAirspaceAircraftSnapshot;
        class CSimulatedAircraft;
    }
}

namespace BlackCore
{
    //! Common base class with providers, interface and some base functionality
    class BLACKCORE_EXPORT CSimulatorCommon : public ISimulator
    {
        Q_OBJECT
        Q_INTERFACES(BlackCore::ISimulator)
        Q_INTERFACES(BlackMisc::Simulation::ISimulationEnvironmentProvider)
        Q_INTERFACES(BlackMisc::Simulation::IInterpolationSetupProvider)

    public:
        //! Log categories
        static const BlackMisc::CLogCategoryList &getLogCategories();

        //! Destructor
        virtual ~CSimulatorCommon();

        // --------- ISimulator implementations ------------
        virtual void highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime) override;
        virtual const BlackMisc::Simulation::CSimulatorInternals &getSimulatorInternals() const override;
        virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const override;
        virtual void setWeatherActivated(bool activated) override;
        virtual void unload() override;
        virtual bool disconnectFrom() override;
        virtual bool logicallyReAddRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual BlackMisc::Aviation::CCallsignSet unrenderedEnabledAircraft() const override;
        virtual BlackMisc::Aviation::CCallsignSet renderedDisabledAircraft() const override;
        virtual int physicallyRemoveMultipleRemoteAircraft(const BlackMisc::Aviation::CCallsignSet &callsigns) override;
        virtual int physicallyRemoveAllRemoteAircraft() override;
        virtual bool changeRemoteAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft) override;
        virtual bool changeRemoteAircraftModel(const BlackMisc::Simulation::CSimulatedAircraft &aircraft) override;
        virtual void clearAllRemoteAircraftData() override;
        virtual void resetAircraftStatistics() override;
        virtual BlackMisc::CStatusMessageList debugVerifyStateAfterAllAircraftRemoved() const override;

        //! \addtogroup swiftdotcommands
        //! @{
        //! <pre>
        //! .drv unload                    unload plugin                           BlackCore::CSimulatorCommon
        //! .drv limit number              limit the number of updates             BlackCore::CSimulatorCommon
        //! .drv logint callsign           log interpolator for callsign           BlackCore::CSimulatorCommon
        //! .drv logint off                no log information for interpolator     BlackCore::CSimulatorCommon
        //! .drv logint write              write interpolator log to file          BlackCore::CSimulatorCommon
        //! .drv logint clear              clear current log                       BlackCore::CSimulatorCommon
        //! .drv pos callsign              shows current position in simulator     BlackCore::CSimulatorCommon
        //! .drv spline|linear callsign    interpolator spline or linear           BlackCore::CSimulatorCommon
        //! .drv aircraft readd callsign   re-add (add again) aircraft             BlackCore::CSimulatorCommon
        //! .drv aircraft readd all        re-add all aircraft                     BlackCore::CSimulatorCommon
        //! .drv aircraft rm callsign      remove aircraft                         BlackCore::CSimulatorCommon
        //! </pre>
        //! @}
        //! \copydoc ISimulator::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

        //! \name Interface implementations, called from context
        //! @{
        virtual bool logicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft) override;
        virtual bool logicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;
        //! @}

        // --------- ISimulator implementations ------------

        //! Register help
        static void registerHelp();

        //!  Counter added aircraft
        int getStatisticsPhysicallyAddedAircraft() const { return m_statsPhysicallyAddedAircraft; }

        //!  Counter removed aircraft
        int getStatisticsPhysicallyRemovedAircraft() const { return m_statsPhysicallyRemovedAircraft; }

        //! Current update time in ms
        double getStatisticsCurrentUpdateTimeMs() const { return m_statsCurrentUpdateTimeMs; }

        //! Average update time in ms
        double getStatisticsAverageUpdateTimeMs() const { return m_statsUpdateAircraftTimeAvgMs; }

        //! Total update time in ms
        qint64 getStatisticsTotalUpdateTimeMs() const { return m_statsUpdateAircraftTimeTotalMs; }

        //! Max.update time in ms
        qint64 getStatisticsMaxUpdateTimeMs() const { return m_statsMaxUpdateTimeMs; }

        //! Number of update runs
        int getStatisticsUpdateRuns() const { return m_statsUpdateAircraftRuns; }

        //! Time between two update requests
        qint64 getStatisticsAircraftUpdatedRequestedDeltaMs() const { return m_statsUpdateAircraftRequestedDeltaMs; }

        //! Access to logger
        const BlackMisc::Simulation::CInterpolationLogger &interpolationLogger() const { return m_interpolationLogger; }

        //! The latest logged data formatted
        //! \remark public only for log. displays
        QString latestLoggedDataFormatted(const BlackMisc::Aviation::CCallsign &cs) const;

        //! Info about update aircraft limitations
        QString updateAircraftLimitationInfo() const;

    protected:
        //! Constructor
        CSimulatorCommon(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                         BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                         BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                         BlackMisc::Weather::IWeatherGridProvider *weatherGridProvider,
                         BlackMisc::Network::IClientProvider *clientProvider,
                         QObject *parent);

        //! \name When swift DB data are read
        //! @{
        virtual void onSwiftDbAllDataRead();
        virtual void onSwiftDbModelMatchingEntitiesRead();
        virtual void onSwiftDbAirportsRead();
        //! @}

        //! \name Connected with remote aircraft provider signals
        //! @{
        //! Recalculate the rendered aircraft, this happens when restrictions are applied (max. aircraft, range)
        virtual void onRecalculatedRenderedAircraft(const BlackMisc::Simulation::CAirspaceAircraftSnapshot &snapshot);
        //! @}

        //! Max.airports in range
        int maxAirportsInRange() const;

        //! Full reset of state
        //! \remark reset as it was unloaded without unloading
        //! \sa ISimulator::clearAllRemoteAircraftData
        virtual void reset();

        //! Reset highlighting
        void resetHighlighting();

        //! Restore all highlighted aircraft
        void stopHighlighting();

        //! Inject weather grid to simulator
        virtual void injectWeatherGrid(const BlackMisc::Weather::CWeatherGrid &weatherGrid) { Q_UNUSED(weatherGrid); }

        //! Airports from web services
        BlackMisc::Aviation::CAirportList getWebServiceAirports() const;

        //! Airport from web services by ICAO code
        BlackMisc::Aviation::CAirport getWebServiceAirport(const BlackMisc::Aviation::CAirportIcaoCode &icao) const;

        //! Blink the highlighted aircraft
        void blinkHighlightedAircraft();

        //! Restore aircraft from the provider data
        void resetAircraftFromProvider(const BlackMisc::Aviation::CCallsign &callsign);

        //! Clear the related data as statistics etc.
        virtual void clearData(const BlackMisc::Aviation::CCallsign &callsign);

        //! Set own model
        void reverseLookupAndUpdateOwnAircraftModel(const BlackMisc::Simulation::CAircraftModel &model);

        //! Set own model
        void reverseLookupAndUpdateOwnAircraftModel(const QString &modelString);

        //! Reload weather settings
        void reloadWeatherSettings();

        //! Parse driver specific details for ISimulator::parseCommandLine
        virtual bool parseDetails(const BlackMisc::CSimpleCommandParser &parser);

        //! Display a debug log message based on BlackMisc::Simulation::CInterpolationAndRenderingSetup
        //! remark shows log messages of functions calls
        void debugLogMessage(const QString &msg);

        //! Display a debug log message based on BlackMisc::Simulation::CInterpolationAndRenderingSetup
        //! remark shows log messages of functions calls
        void debugLogMessage(const QString &funcInfo, const QString &msg);

        //! Show log messages?
        bool showDebugLogMessage() const;

        //! Slow timer used to highlight aircraft, can be used for other things too
        virtual void oneSecondTimerTimeout();

        //! Kill timer if id is valid
        void safeKillTimer();

        //! Info about invalid situation
        QString getInvalidSituationLogMessage(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CInterpolationStatus &status, const QString &details = {}) const;

        //! Can a new log message be generated without generating a "message" overflow
        //! \remark works per callsign
        //! \remark use this function when there is a risk that a lot of log. messages will be generated in a short time
        bool clampedLog(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::CStatusMessage &message);

        //! Mark as justed logged
        //! \remark touch, but also return if it can be logged
        //! \remark use this function when there is a risk that a lot of log. messages will be generated in a short time
        void removedClampedLog(const BlackMisc::Aviation::CCallsign &callsign);

        //! Update stats and flags
        void setStatsRemoteAircraftUpdate(qint64 startTime, bool limited = false);

        //! Equal to last sent situation
        bool isEqualLastSent(const BlackMisc::Aviation::CAircraftSituation &compare) const;

        //! Equal to last sent situation
        bool isEqualLastSent(const BlackMisc::Aviation::CAircraftParts &compare, const BlackMisc::Aviation::CCallsign &callsign) const;

        //! Remember as last sent
        void rememberLastSent(const BlackMisc::Aviation::CAircraftSituation &sent);

        //! Remember as last sent
        void rememberLastSent(const BlackMisc::Aviation::CAircraftParts &sent, const BlackMisc::Aviation::CCallsign &callsign);

        //! Last sent situations
        BlackMisc::Aviation::CAircraftSituationList getLastSentCanLikelySkipNearGroundInterpolation() const;

        //! Lookup against DB data
        static BlackMisc::Simulation::CAircraftModel reverseLookupModel(const BlackMisc::Simulation::CAircraftModel &model);

        bool   m_pausedSimFreezesInterpolation = false;   //!< paused simulator will also pause interpolation (so AI aircraft will hold)
        bool   m_autoCalcAirportDistance = true;          //!< automatically calculate airport distance and bearing
        bool   m_updateRemoteAircraftInProgress = false;  //!< currently updating remote aircraft
        int    m_timerId = -1;                            //!< dispatch timer id
        int    m_statsUpdateAircraftRuns = 0;             //!< statistics update count
        int    m_statsUpdateAircraftLimited = 0;          //!< skipped because of max.update limitations
        double m_statsUpdateAircraftTimeAvgMs = 0;        //!< statistics average update time
        qint64 m_statsUpdateAircraftTimeTotalMs = 0;      //!< statistics total update time
        qint64 m_statsCurrentUpdateTimeMs = 0;            //!< statistics current update time
        qint64 m_statsMaxUpdateTimeMs = 0;                //!< statistics max.update time
        qint64 m_statsLastUpdateAircraftRequestedMs  = 0; //!< when was the last aircraft update requested
        qint64 m_statsUpdateAircraftRequestedDeltaMs = 0; //!< delta time between 2 aircraft updates

        BlackMisc::Simulation::CSimulatorInternals  m_simulatorInternals;  //!< setup object
        BlackMisc::Simulation::CInterpolationLogger m_interpolationLogger; //!< log.interpolation
        BlackMisc::Aviation::CTimestampPerCallsign  m_clampedLogMsg;       //!< when logged last for this callsign, can be used so there is no log message overflow
        BlackMisc::Aviation::CAircraftSituationPerCallsign m_lastSentSituations; //!< last situation sent to simulator
        BlackMisc::Aviation::CAircraftPartsPerCallsign     m_lastSentParts;      //!< last parts sent to simulator

        // some optional functionality which can be used by the simulators as needed
        BlackMisc::Simulation::CSimulatedAircraftList m_addAgainAircraftWhenRemoved; //!< add this model again when removed, normally used to change model

        // limit the update aircraft to a maximum per second
        BlackMisc::CTokenBucket m_limitUpdateAircraftBucket { 5, 100, 5 }; //!< means 50 per second
        bool m_limitUpdateAircraft = false; //!< limit the update frequency by using BlackMisc::CTokenBucket

        //! Limit reached (max number of updates by token bucket if enabled)
        bool isUpdateAircraftLimited(qint64 timestamp = -1);

        //! Limited as CSimulatorCommon::isUpdateAircraftLimited plus updating statistics
        bool isUpdateAircraftLimitedWithStats(qint64 startTime = -1);

        //! Limit to updates per second
        bool limitToUpdatesPerSecond(int numberPerSecond);

        // weather
        bool m_isWeatherActivated = false;                         //!< Is simulator weather activated?
        BlackMisc::Geo::CCoordinateGeodetic m_lastWeatherPosition; //!< Own aircraft position at which weather was fetched and injected last
        BlackMisc::CSetting<BlackMisc::Simulation::Settings::TSelectedWeatherScenario> m_weatherScenarioSettings { this, &CSimulatorCommon::reloadWeatherSettings }; //!< Selected weather scenario

    private:
        // remote aircraft provider ("rap") bound
        void rapOnRecalculatedRenderedAircraft(const BlackMisc::Simulation::CAirspaceAircraftSnapshot &snapshot);
        void rapOnRemoteProviderRemovedAircraft(const BlackMisc::Aviation::CCallsign &callsign);

        // call with counters updated
        void callPhysicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft);
        void callPhysicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &remoteCallsign, bool blinking = false);

        //! Display a logged situation in simulator
        void displayLoggedSituationInSimulator(const BlackMisc::Aviation::CCallsign &cs, bool stopLogging, int times = 40);

        bool m_blinkCycle = false;            //!< used for highlighting
        qint64 m_highlightEndTimeMsEpoch = 0; //!< end highlighting
        int m_timerCounter = 0;               //!< allows to calculate n seconds
        QTimer m_oneSecondTimer;              //!< multi purpose timer with 1 sec. interval
        BlackMisc::Simulation::CSimulatedAircraftList m_highlightedAircraft;               //!< all other aircraft are to be ignored
        BlackMisc::Aviation::CCallsignSet             m_callsignsToBeRendered;             //!< callsigns which will be rendered
        BlackMisc::CConnectionGuard                   m_remoteAircraftProviderConnections; //!< connected signal/slots

        // statistics values of how often those functions are called
        // those are the added counters, overflow will not be an issue here (discussed in T171 review)
        int m_statsPhysicallyAddedAircraft = 0;   //!< statistics, how many aircraft added
        int m_statsPhysicallyRemovedAircraft = 0; //!< statistics, how many aircraft removed
    };
} // namespace

#endif // guard
