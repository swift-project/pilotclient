// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_SIMULATOR_H
#define SWIFT_CORE_SIMULATOR_H

#include <atomic>

#include <QFlags>
#include <QObject>
#include <QString>

#include "config/buildconfig.h"
#include "core/application.h"
#include "core/swiftcoreexport.h"
#include "misc/aviation/airportlist.h"
#include "misc/aviation/callsignset.h"
#include "misc/geo/elevationplane.h"
#include "misc/identifiable.h"
#include "misc/identifier.h"
#include "misc/network/clientprovider.h"
#include "misc/pixmap.h"
#include "misc/pq/length.h"
#include "misc/pq/time.h"
#include "misc/simplecommandparser.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/autopublishdata.h"
#include "misc/simulation/interpolation/interpolationrenderingsetup.h"
#include "misc/simulation/interpolation/interpolationsetupprovider.h"
#include "misc/simulation/interpolation/interpolatormulti.h"
#include "misc/simulation/ownaircraftprovider.h"
#include "misc/simulation/remoteaircraftprovider.h"
#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/simulationenvironmentprovider.h"
#include "misc/simulation/simulatorinternals.h"
#include "misc/simulation/simulatorplugininfo.h"
#include "misc/statusmessage.h"
#include "misc/tokenbucket.h"

namespace swift::misc::network
{
    class CTextMessage;
}

namespace swift::core
{
    //! Interface to a simulator.
    class SWIFT_CORE_EXPORT ISimulator :
        public QObject,
        public swift::misc::simulation::COwnAircraftAware, // access to in memory own aircraft data
        public swift::misc::simulation::CRemoteAircraftAware, // access to in memory remote aircraft data
        public swift::misc::network::CClientAware, // the network client with its capabilities
        public swift::misc::simulation::ISimulationEnvironmentProvider, // give access to elevation etc.
        public swift::misc::simulation::IInterpolationSetupProvider, // setup
        public swift::misc::CIdentifiable
    {
        Q_OBJECT
        Q_INTERFACES(swift::misc::simulation::ISimulationEnvironmentProvider)
        Q_INTERFACES(swift::misc::simulation::IInterpolationSetupProvider)

    public:
        //! ISimulator status
        enum SimulatorStatusFlag
        {
            Unspecified = 0, //!< unspecied, needed as default value
            Disconnected = 1 << 0, //!< not connected, and hence not simulating/paused
            Connected = 1 << 1, //!< Is the plugin connected to the simulator?
            Simulating = 1 << 2, //!< Is the simulator actually simulating?
            Paused = 1 << 3, //!< Is the simulator paused?
        };
        Q_DECLARE_FLAGS(SimulatorStatus, SimulatorStatusFlag)
        Q_FLAG(SimulatorStatus)

        //! Log categories
        static const QStringList &getLogCategories();

        //! Destructor
        ~ISimulator() override;

        //! Combined status
        virtual SimulatorStatus getSimulatorStatus() const;

        //! Is time synchronization on?
        virtual bool isTimeSynchronized() const = 0;

        //! Get the simulator current internal state
        virtual const swift::misc::simulation::CSimulatorInternals &getSimulatorInternals() const { return m_simulatorInternals; }

        //! Connect to simulator
        virtual bool connectTo() = 0;

        //! Disconnect from simulator
        virtual bool disconnectFrom();

        //! Logically add a new aircraft.
        //! Depending on max. aircraft, enabled status etc. it will physically added to the simulator.
        //! \sa physicallyAddRemoteAircraft
        virtual bool logicallyAddRemoteAircraft(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft);

        //! Logically remove remote aircraft from simulator.
        //! Depending on max. aircraft, enabled status etc. it will physically added to the simulator.
        virtual bool logicallyRemoveRemoteAircraft(const swift::misc::aviation::CCallsign &callsign);

        //! Removes and adds again the aircraft
        //! \sa logicallyRemoveRemoteAircraft
        //! \sa logicallyAddRemoteAircraft
        virtual bool logicallyReAddRemoteAircraft(const swift::misc::aviation::CCallsign &callsign);

        //! Public version to remove all aircraft
        //! \sa physicallyRemoveAllRemoteAircraft
        int removeAllRemoteAircraft()
        {
            return this->physicallyRemoveAllRemoteAircraft();
        }

        //! Change remote aircraft per property
        virtual bool changeRemoteAircraftModel(const swift::misc::simulation::CSimulatedAircraft &aircraft);

        //! Aircraft got enabled / disabled
        virtual bool changeRemoteAircraftEnabled(const swift::misc::simulation::CSimulatedAircraft &aircraft);

        //! Update own aircraft cockpit (usually from context)
        virtual bool updateOwnSimulatorCockpit(const swift::misc::simulation::CSimulatedAircraft &aircraft, const swift::misc::CIdentifier &originator) = 0;

        //! Update own aircraft cockpit (usually from context)
        virtual bool updateOwnSimulatorSelcal(const swift::misc::aviation::CSelcal &selcal, const swift::misc::CIdentifier &originator) = 0;

        //! Display a status message in the simulator
        virtual void displayStatusMessage(const swift::misc::CStatusMessage &message) const = 0;

        //! Display a text message
        virtual void displayTextMessage(const swift::misc::network::CTextMessage &message) const = 0;

        //! Airports in range from simulator, or if not available from web service
        virtual swift::misc::aviation::CAirportList getAirportsInRange(bool recalculateDistance) const;

        //! Set time synchronization between simulator and user's computer time
        //! \remarks not all drivers implement this, e.g. if it is an intrinsic simulator feature
        virtual bool setTimeSynchronization(bool enable, const swift::misc::physical_quantities::CTime &offset) = 0;

        //! Time synchronization offset
        virtual swift::misc::physical_quantities::CTime getTimeSynchronizationOffset() const = 0;

        //! Is the aircraft rendered (displayed in simulator)?
        //! This shall only return true if the aircraft is really visible in the simulator
        virtual bool isPhysicallyRenderedAircraft(const swift::misc::aviation::CCallsign &callsign) const = 0;

        //! Physically rendered (displayed in simulator)
        //! This shall only return aircraft handled in the simulator
        virtual swift::misc::aviation::CCallsignSet physicallyRenderedAircraft() const = 0;

        //! Follow aircraft
        virtual bool followAircraft(const swift::misc::aviation::CCallsign &callsign);

        //! Recalculate all aircraft
        virtual void recalculateAllAircraft();

        //! Flight network has been connected
        //! \remark hint if network connected and we expect any planes
        //! \sa ISimulator::isFlightNetworkConnected
        virtual void setFlightNetworkConnected(bool connected);

        //! Is the flight network connected
        bool isFlightNetworkConnected() const { return m_networkConnected; }

        //! Settings for current simulator
        swift::misc::simulation::settings::CSpecializedSimulatorSettings getSimulatorSettings() const { return m_multiSettings.getSpecializedSettings(this->getSimulatorInfo()); }

        //! Driver will be unloaded
        virtual void unload();

        //! Are we connected to the simulator?
        virtual bool isConnected() const = 0;

        //! Simulator paused?
        virtual bool isPaused() const = 0;

        //! Simulator running?
        virtual bool isSimulating() const { return this->isConnected(); }

        //! Set test mode (driver can skip code parts etc., driver dependent)
        void setTestMode(bool test) { m_test = test; }

        //! Test mode? (driver can skip code parts etc., driver dependent)
        bool isTestMode() const { return m_test; }

        //! Average FPS (frames per second)
        double getAverageFPS() const { return m_averageFps; }

        //! Ratio of simulation time to real time, due to low FPS
        double getSimTimeRatio() const { return m_simTimeRatio; }

        //! Number of track miles over-reported, due to low FPS
        double getTrackMilesShort() const { return m_trackMilesShort; }

        //! Number of minutes behind schedule, due to low FPS
        double getMinutesLate() const { return m_minutesLate; }

        //! Send situation/parts for testing
        virtual bool testSendSituationAndParts(const swift::misc::aviation::CCallsign &callsign, const swift::misc::aviation::CAircraftSituation &situation, const swift::misc::aviation::CAircraftParts &parts) = 0;

        //! Enable pseudo elevations (testing)
        void setTestEnablePseudoElevation(bool enable) { m_enablePseudoElevation = enable; }

        //! Set an elevation for testing
        void setTestElevation(const swift::misc::aviation::CAltitude &altitude) { m_pseudoElevation = altitude; }

        //! Debug function to check state after all aircraft have been removed
        //! \remarks only in local developer builds
        virtual swift::misc::CStatusMessageList debugVerifyStateAfterAllAircraftRemoved() const;

        //! Is overall (swift) application shutting down
        //! \threadsafe
        virtual bool isShuttingDown() const { return (!sApp || sApp->isShuttingDown()); }

        //! Shutting down or disconnected?
        virtual bool isShuttingDownOrDisconnected() const { return (this->isShuttingDown() || !this->isConnected()); }

        //! Shutting down, disconnected, or no remote aircraft
        virtual bool isShuttingDownDisconnectedOrNoAircraft() const { return (this->isShuttingDownOrDisconnected() || this->getAircraftInRangeCount() < 1); }

        //! Shutting down, disconnected, or no remote aircraft
        virtual bool isShuttingDownDisconnectedOrNoAircraft(bool isProbe) const
        {
            return isProbe ?
                       this->isShuttingDownOrDisconnected() :
                       (this->isShuttingDownOrDisconnected() || this->getAircraftInRangeCount() < 1);
        }

        //! \copydoc swift::misc::simulation::ISimulationEnvironmentProvider::requestElevation
        //! \remark needs to be overridden if the concrete driver supports such an option
        //! \sa ISimulator::callbackReceivedRequestedElevation
        bool requestElevation(const swift::misc::geo::ICoordinateGeodetic &reference, const swift::misc::aviation::CCallsign &callsign) override;

        //! \copydoc swift::misc::simulation::ISimulationEnvironmentProvider::requestElevation
        bool requestElevation(const swift::misc::aviation::CAircraftSituation &situation) { return this->requestElevation(situation, situation.getCallsign()); }

        //! A requested elevation has been received
        //! \remark public for testing purposes
        virtual void callbackReceivedRequestedElevation(const swift::misc::geo::CElevationPlane &plane, const swift::misc::aviation::CCallsign &callsign, bool isWater);

        //! Allows to print out simulator specific statistics
        virtual QString getStatisticsSimulatorSpecific() const { return {}; }

        //! Reset the statistics
        virtual void resetAircraftStatistics();

        //! \copydoc swift::misc::IProvider::asQObject
        QObject *asQObject() override { return this; }

        //! Is this the emulated driver just pretending to be P3D, FSX, or XPlane
        bool isEmulatedDriver() const;

        //! \ingroup swiftdotcommands
        //! <pre>
        //! .drv cg length clear|modelstring  set overridden CG for model string      swift::core::ISimulator
        //! .drv unload                       unload plugin                           swift::core::ISimulator
        //! .drv limit number                 limit the number of updates             swift::core::ISimulator
        //! .drv logint callsign              log interpolator for callsign           swift::core::ISimulator
        //! .drv logint off                   no log information for interpolator     swift::core::ISimulator
        //! .drv logint write                 write interpolator log to file          swift::core::ISimulator
        //! .drv logint clear                 clear current log                       swift::core::ISimulator
        //! .drv pos callsign                 shows current position in simulator     swift::core::ISimulator
        //! .drv spline|linear callsign       interpolator spline or linear           swift::core::ISimulator
        //! .drv aircraft readd callsign      re-add (add again) aircraft             swift::core::ISimulator
        //! .drv aircraft readd all           re-add all aircraft                     swift::core::ISimulator
        //! .drv aircraft rm callsign         remove aircraft                         swift::core::ISimulator
        //! .drv fsuipc   on|off              enable/disable FSUIPC (if applicable)   swift::simplugin::fscommon::CSimulatorFsCommon
        //! </pre>
        //! Parse command line for simulator drivers, derived classes can add specific parsing by overriding ISimulator::parseDetails
        virtual bool parseCommandLine(const QString &commandLine, const swift::misc::CIdentifier &originator);

        //! Consolidate setup with other data like from swift::misc::simulation::IRemoteAircraftProvider
        //! \threadsafe
        swift::misc::simulation::CInterpolationAndRenderingSetupPerCallsign getInterpolationSetupConsolidated(const swift::misc::aviation::CCallsign &callsign, bool forceFullUpdate) const;

        //! \copydoc swift::misc::simulation::IInterpolationSetupProvider::setInterpolationSetupGlobal
        bool setInterpolationSetupGlobal(const swift::misc::simulation::CInterpolationAndRenderingSetupGlobal &setup) override;

        //! Interpolation messages for callsign
        virtual swift::misc::CStatusMessageList getInterpolationMessages(const swift::misc::aviation::CCallsign &callsign) const = 0;

        //! Get the data for auto publishing
        const swift::misc::simulation::CAutoPublishData &getPublishData() const { return m_autoPublishing; }

        //!  Counter added aircraft
        int getStatisticsPhysicallyAddedAircraft() const { return m_statsPhysicallyAddedAircraft; }

        //!  Counter removed aircraft
        int getStatisticsPhysicallyRemovedAircraft() const { return m_statsPhysicallyRemovedAircraft; }

        //! Current update time in ms
        qint64 getStatisticsCurrentUpdateTimeMs() const { return m_statsCurrentUpdateTimeMs; }

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

        //! The traced loopback situations
        swift::misc::aviation::CAircraftSituationList getLoopbackSituations(const swift::misc::aviation::CCallsign &callsign) const;

        //! Access to logger
        const swift::misc::simulation::CInterpolationLogger &interpolationLogger() const { return m_interpolationLogger; }

        //! The latest logged data formatted
        //! \remark public only for log. displays
        QString latestLoggedDataFormatted(const swift::misc::aviation::CCallsign &cs) const;

        //! Info about update aircraft limitations
        QString updateAircraftLimitationInfo() const;

        //! Reset the last sent values
        void resetLastSentValues();

        //! Reset the last sent values per callsign
        void resetLastSentValues(const swift::misc::aviation::CCallsign &callsign);

        //! Register help
        static void registerHelp();

        //! Status to string
        static QString statusToString(SimulatorStatus status);

        //! Any connected status?
        static bool isAnyConnectedStatus(SimulatorStatus status);

        //! Get a test callsign
        static const swift::misc::aviation::CCallsign &getTestCallsign();

    signals:
        //! Simulator combined status
        void simulatorStatusChanged(swift::core::ISimulator::SimulatorStatus status); // use emitSimulatorCombinedStatus to emit

        //! Emitted when own aircraft model has changed
        void ownAircraftModelChanged(const swift::misc::simulation::CAircraftModel &model);

        //! Render restrictions have been changed
        void renderRestrictionsChanged(bool restricted, bool enabled, int maxAircraft, const swift::misc::physical_quantities::CLength &maxRenderedDistance);

        //! Interpolation or rendering setup changed
        void interpolationAndRenderingSetupChanged();

        //! Aircraft rendering changed
        void aircraftRenderingChanged(const swift::misc::simulation::CSimulatedAircraft &aircraft);

        //! Adding the remote model failed
        void physicallyAddingRemoteModelFailed(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, bool disabled, bool requestFailover, const swift::misc::CStatusMessage &message);

        //! An airspace snapshot was handled
        void airspaceSnapshotHandled();

        //! Relevant simulator messages to be explicitly displayed
        void driverMessages(const swift::misc::CStatusMessageList &messages);

        //! Requested elevation, call pending
        void requestedElevation(const swift::misc::aviation::CCallsign &callsign);

        //! A requested elevation has been received
        void receivedRequestedElevation(const swift::misc::geo::CElevationPlane &plane, const swift::misc::aviation::CCallsign &callsign);

        //! Auto publish data written for simulator
        void autoPublishDataWritten(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Frame rate has fallen too far below the threshold to maintain consistent sim rate
        void insufficientFrameRateDetected(bool fatal);

    protected:
        //! Constructor with all the providers
        ISimulator(const swift::misc::simulation::CSimulatorPluginInfo &pluginInfo,
                   swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
                   swift::misc::simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                   swift::misc::network::IClientProvider *clientProvider,
                   QObject *parent = nullptr);

        //! @{
        //! When swift DB data are read
        virtual void onSwiftDbAllDataRead();
        virtual void onSwiftDbModelMatchingEntitiesRead();
        virtual void onSwiftDbAirportsRead();
        //! @}

        //! Init the internals info from the simulator
        virtual void initSimulatorInternals();

        //! Parsed in derived classes
        virtual bool parseDetails(const swift::misc::CSimpleCommandParser &parser) = 0;

        //! Airports from web services
        swift::misc::aviation::CAirportList getWebServiceAirports() const;

        //! Airport from web services by ICAO code
        swift::misc::aviation::CAirport getWebServiceAirport(const swift::misc::aviation::CAirportIcaoCode &icao) const;

        //! Max.airports in range
        int maxAirportsInRange() const;

        //! Recalculate the rendered aircraft, this happens when restrictions are applied (max. aircraft, range)
        virtual void onRecalculatedRenderedAircraft(const swift::misc::simulation::CAirspaceAircraftSnapshot &snapshot);

        //! Add new remote aircraft physically to the simulator
        //! \sa changeRemoteAircraftEnabled to hide a remote aircraft
        virtual bool physicallyAddRemoteAircraft(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft) = 0;

        //! Remove remote aircraft from simulator
        virtual bool physicallyRemoveRemoteAircraft(const swift::misc::aviation::CCallsign &callsign) = 0;

        //! Remove remote aircraft from simulator
        virtual int physicallyRemoveMultipleRemoteAircraft(const swift::misc::aviation::CCallsignSet &callsigns);

        //! Clear all aircraft related data, but do not physically remove the aircraft
        virtual void clearAllRemoteAircraftData();

        //! Remove all remote aircraft and their data via ISimulator::clearAllRemoteAircraftData
        //! \remark each driver is supposed to override that, implement the "physically removing part" (simulator specific) and the call the base class
        //! \sa ISimulator::clearAllRemoteAircraftData
        virtual int physicallyRemoveAllRemoteAircraft();

        //! Set elevation and CG in the providers and for auto publishing
        //! \sa ISimulator::updateOwnSituationAndGroundElevation
        void rememberElevationAndSimulatorCG(const swift::misc::aviation::CCallsign &callsign, const swift::misc::simulation::CAircraftModel &model, bool likelyOnGroundElevation, const swift::misc::geo::CElevationPlane &elevation, const swift::misc::physical_quantities::CLength &simulatorCG);

        //! Emit the combined status
        //! \param oldStatus optionally one can capture and provide the old status for comparison. In case of equal status values no signal will be sent
        //! \sa simulatorStatusChanged;
        void emitSimulatorCombinedStatus(SimulatorStatus oldStatus = Unspecified);

        //! \copydoc swift::misc::simulation::IInterpolationSetupProvider::emitInterpolationSetupChanged
        void emitInterpolationSetupChanged() override;

        //! Display a debug log message based on swift::misc::simulation::CInterpolationAndRenderingSetup
        //! remark shows log messages of functions calls
        void debugLogMessage(const QString &msg);

        //! Display a debug log message based on swift::misc::simulation::CInterpolationAndRenderingSetup
        //! remark shows log messages of functions calls
        void debugLogMessage(const QString &funcInfo, const QString &msg);

        //! Show log messages?
        bool showDebugLogMessage() const;

        //! Restore aircraft from the provider data
        void resetAircraftFromProvider(const swift::misc::aviation::CCallsign &callsign);

        //! Clear the related data as statistics etc.
        virtual void clearData(const swift::misc::aviation::CCallsign &callsign);

        //! Add a loopback situation if logging is enabled
        bool addLoopbackSituation(const swift::misc::aviation::CAircraftSituation &situation);

        //! Add a loopback situation if logging is enabled
        bool addLoopbackSituation(const swift::misc::aviation::CCallsign &callsign, const swift::misc::geo::CElevationPlane &elevationPlane, const swift::misc::physical_quantities::CLength &cg);

        //! Full reset of state
        //! \remark reset as it was unloaded without unloading
        //! \sa ISimulator::clearAllRemoteAircraftData
        virtual void reset();

        //! Do update all remote aircraft?
        bool isUpdateAllRemoteAircraft(qint64 currentTimestamp = -1) const;

        //! Update all aircraft for ms
        void setUpdateAllRemoteAircraft(qint64 currentTimestamp = -1, qint64 forMs = -1);

        //! Reset
        void resetUpdateAllRemoteAircraft();

        //! Kill timer if id is valid
        void safeKillTimer();

        //! Equal to last sent situation
        bool isEqualLastSent(const swift::misc::aviation::CAircraftSituation &compare) const;

        //! Equal to last sent situation
        bool isEqualLastSent(const swift::misc::aviation::CAircraftParts &compare, const swift::misc::aviation::CCallsign &callsign) const;

        //! Remember as last sent
        void rememberLastSent(const swift::misc::aviation::CAircraftSituation &sent);

        //! Remember as last sent
        void rememberLastSent(const swift::misc::aviation::CAircraftParts &sent, const swift::misc::aviation::CCallsign &callsign);

        //! Last sent situations
        swift::misc::aviation::CAircraftSituationList getLastSentCanLikelySkipNearGroundInterpolation() const;

        //! Limit reached (max number of updates by token bucket if enabled)
        bool isUpdateAircraftLimited(qint64 timestamp = -1);

        //! Limited as ISimulator::isUpdateAircraftLimited plus updating statistics
        bool isUpdateAircraftLimitedWithStats(qint64 startTime = -1);

        //! Limit to updates per second
        bool limitToUpdatesPerSecond(int numberPerSecond);

        //! Set own model
        void reverseLookupAndUpdateOwnAircraftModel(const swift::misc::simulation::CAircraftModel &model);

        //! Set own model
        void reverseLookupAndUpdateOwnAircraftModel(const QString &modelString);

        //! Info about invalid situation
        QString getInvalidSituationLogMessage(const swift::misc::aviation::CCallsign &callsign, const swift::misc::simulation::CInterpolationStatus &status, const QString &details = {}) const;

        //! Update stats and flags
        void finishUpdateRemoteAircraftAndSetStatistics(qint64 startTime, bool limited = false);

        //! Own model has been changed
        virtual void onOwnModelChanged(const swift::misc::simulation::CAircraftModel &newModel);

        //! Update own aircraft position and if suitable use it to update ground elevation
        bool updateOwnSituationAndGroundElevation(const swift::misc::aviation::CAircraftSituation &situation);

        //! Get the model set
        swift::misc::simulation::CAircraftModelList getModelSet() const;

        //! Validate if model has callsign and such
        bool validateModelOfAircraft(const swift::misc::simulation::CSimulatedAircraft &aircraft) const;

        //! Unified qeeing aircraft message
        void logAddingAircraftModel(const swift::misc::simulation::CSimulatedAircraft &aircraft) const;

        //! Test version aware version of isAircraftInRange
        bool isAircraftInRangeOrTestMode(const swift::misc::aviation::CCallsign &callsign) const;

        //! Lookup against DB data
        static swift::misc::simulation::CAircraftModel reverseLookupModel(const swift::misc::simulation::CAircraftModel &model);

        bool m_updateRemoteAircraftInProgress = false; //!< currently updating remote aircraft
        bool m_enablePseudoElevation = false; //!< return faked elevations (testing)
        int m_timerId = -1; //!< dispatch timer id
        int m_statsUpdateAircraftRuns = 0; //!< statistics update count
        int m_statsUpdateAircraftLimited = 0; //!< skipped because of max.update limitations
        double m_statsUpdateAircraftTimeAvgMs = 0; //!< statistics average update time
        double m_averageFps = -1.0; //!< FPS
        double m_simTimeRatio = 1.0; //!< ratio of simulation time to real time, due to low FPS (X-Plane)
        double m_trackMilesShort = 0.0; //!< difference between real and reported groundspeed, multiplied by time
        double m_minutesLate = 0.0; //!< difference between real and reported groundspeed, integrated over time
        qint64 m_updateAllRemoteAircraftUntil = 0; //!< force an update of all remote aircraft, used when own aircraft is moved, paused to make sure all remote aircraft are updated
        qint64 m_statsUpdateAircraftTimeTotalMs = 0; //!< statistics total update time
        qint64 m_statsCurrentUpdateTimeMs = 0; //!< statistics current update time
        qint64 m_statsMaxUpdateTimeMs = 0; //!< statistics max.update time
        qint64 m_lastRecordedGndElevationMs = 0; //!< when gnd.elevation was last modified
        qint64 m_statsLastUpdateAircraftRequestedMs = 0; //!< when was the last aircraft update requested
        qint64 m_statsUpdateAircraftRequestedDeltaMs = 0; //!< delta time between 2 aircraft updates

        swift::misc::aviation::CAltitude m_pseudoElevation { swift::misc::aviation::CAltitude::null() }; //!< pseudo elevation for testing purposes
        swift::misc::simulation::CSimulatorInternals m_simulatorInternals; //!< setup read from the sim
        swift::misc::simulation::CInterpolationLogger m_interpolationLogger; //!< log.interpolation
        swift::misc::simulation::CAutoPublishData m_autoPublishing; //!< for the DB
        swift::misc::aviation::CAircraftSituationPerCallsign m_lastSentSituations; //!< last situations sent to simulator
        swift::misc::aviation::CAircraftPartsPerCallsign m_lastSentParts; //!< last parts sent to simulator

        // some optional functionality which can be used by the simulators as needed
        swift::misc::simulation::CSimulatedAircraftList m_addAgainAircraftWhenRemoved; //!< add this model again when removed, normally used to change model

        // loopback situations, situations which are received from simulator for remote aircraft
        swift::misc::aviation::CAircraftSituationListPerCallsign m_loopbackSituations; //!< traced loopback situations

        // limit the update aircraft to a maximum per second
        swift::misc::CTokenBucket m_limitUpdateAircraftBucket { 5, 100, 5 }; //!< means 50 per second
        bool m_limitUpdateAircraft = false; //!< limit the update frequency by using swift::misc::CTokenBucket

        // general settings
        swift::misc::simulation::settings::CMultiSimulatorSettings m_multiSettings { this }; //!< simulator settings for all simulators

    private:
        // remote aircraft provider ("rap") bound
        void rapOnRecalculatedRenderedAircraft(const swift::misc::simulation::CAirspaceAircraftSnapshot &snapshot);

        // call with counters updated
        void callPhysicallyAddRemoteAircraft(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft);
        void callPhysicallyRemoveRemoteAircraft(const swift::misc::aviation::CCallsign &remoteCallsign);

        //! Display a logged situation in simulator
        void displayLoggedSituationInSimulator(const swift::misc::aviation::CCallsign &cs, bool stopLogging, int times = 40);

        // statistics values of how often those functions are called
        // those are the added counters, overflow will not be an issue here (discussed in T171 review)
        int m_statsPhysicallyAddedAircraft = 0; //!< statistics, how many aircraft added
        int m_statsPhysicallyRemovedAircraft = 0; //!< statistics, how many aircraft removed

        // misc.
        bool m_networkConnected = false; //!< flight network connected
        bool m_test = false; //!< test mode?
        swift::misc::aviation::CCallsignSet m_callsignsToBeRendered; //!< callsigns which will be rendered
        swift::misc::CConnectionGuard m_remoteAircraftProviderConnections; //!< connected signal/slots
    };

    //! \brief Interface to a simulator listener.
    //! \details The simulator listener is responsible for letting the core know,
    //!          when the corresponding simulator is started.
    //! \note Will be moved to a background thread (context)
    class SWIFT_CORE_EXPORT ISimulatorListener : public QObject
    {
        Q_OBJECT

    public:
        //! Destructor
        ~ISimulatorListener() override = default;

        //! Corresponding info
        const swift::misc::simulation::CSimulatorPluginInfo &getPluginInfo() const { return m_info; }

        //! Info about the backend system (if available)
        virtual QString backendInfo() const;

        //! Running?
        bool isRunning() const { return m_isRunning; }

    public slots:
        //! Start listening for the simulator to start.
        //! \threadsafe
        void start();

        //! Stops listening.
        //! \threadsafe
        void stop();

        //! Check simulator availability
        //! \threadsafe
        void check();

    signals:
        //! Emitted when the listener discovers the simulator running.
        void simulatorStarted(const swift::misc::simulation::CSimulatorPluginInfo &info);

    protected:
        //! Constructor
        //! \sa ISimulatorFactory::createListener().
        explicit ISimulatorListener(const swift::misc::simulation::CSimulatorPluginInfo &info);

        //! Overall (swift) application shutting down
        virtual bool isShuttingDown() const;

        //! Plugin specific implementation to start listener
        virtual void startImpl() = 0;

        //! Plugin specific implementation to stop listener
        virtual void stopImpl() = 0;

        //! Plugin specific implementation to check
        virtual void checkImpl() = 0;

    private:
        //! swift will shutdown
        void onAboutToShutdown();

        swift::misc::simulation::CSimulatorPluginInfo m_info;
        std::atomic_bool m_isRunning { false };
        std::atomic_bool m_aboutToShutdown { false }; // swift will shutdown
    };

    //! Factory pattern class to create instances of ISimulator
    class SWIFT_CORE_EXPORT ISimulatorFactory
    {
    public:
        //! ISimulatorVirtual destructor
        virtual ~ISimulatorFactory() = default;

        //! @{
        //! Not copyable
        ISimulatorFactory(const ISimulatorFactory &) = delete;
        ISimulatorFactory &operator=(const ISimulatorFactory &) = delete;
        //! @}

        //! Create a new instance of a driver
        //! \param info                      metadata about simulator
        //! \param ownAircraftProvider       in memory access to own aircraft data
        //! \param remoteAircraftProvider    in memory access to rendered aircraft data such as situation history and aircraft itself
        //! \param clientProvider            in memory access to client data
        //! \return driver instance
        //!
        virtual ISimulator *create(
            const swift::misc::simulation::CSimulatorPluginInfo &info,
            swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
            swift::misc::simulation::IRemoteAircraftProvider *remoteAircraftProvider,
            swift::misc::network::IClientProvider *clientProvider) = 0;

        //! Simulator listener instance
        virtual ISimulatorListener *createListener(const swift::misc::simulation::CSimulatorPluginInfo &info) = 0;

    protected:
        //! Default ctor
        ISimulatorFactory() {}
    };
} // namespace swift::core

Q_DECLARE_METATYPE(swift::core::ISimulator::SimulatorStatusFlag)
Q_DECLARE_METATYPE(swift::core::ISimulator::SimulatorStatus)
Q_DECLARE_INTERFACE(swift::core::ISimulator, "org.swift-project.swift_core.simulator")
Q_DECLARE_INTERFACE(swift::core::ISimulatorFactory, "org.swift-project.swift_core.simulatorfactory")
Q_DECLARE_OPERATORS_FOR_FLAGS(swift::core::ISimulator::SimulatorStatus)

#endif // guard
