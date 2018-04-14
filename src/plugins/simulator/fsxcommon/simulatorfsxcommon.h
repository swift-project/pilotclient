/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FSXCOMMON_SIMULATORFSXCOMMON_H
#define BLACKSIMPLUGIN_FSXCOMMON_SIMULATORFSXCOMMON_H

#include "simconnectdatadefinition.h"
#include "simconnectobject.h"
#include "../fsxcommon/simconnectwindows.h"
#include "../fscommon/simulatorfscommon.h"
#include "blackcore/simulator.h"
#include "blackmisc/simulation/interpolatorlinear.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/network/client.h"
#include "blackmisc/pixmap.h"

#include <QObject>
#include <QtPlugin>
#include <QHash>
#include <QList>
#include <QFutureWatcher>

namespace BlackSimPlugin
{
    namespace FsxCommon
    {
        //! SimConnect Event IDs
        enum EventIds
        {
            SystemEventSimStatus,
            SystemEventObjectAdded,
            SystemEventObjectRemoved,
            SystemEventSlewToggle,
            SystemEventFrame,
            SystemEventPause,
            SystemEventFlightLoaded,
            EventPauseToggle,
            EventFreezeLat,
            EventFreezeAlt,
            EventFreezeAtt,
            EventSetCom1Active,
            EventSetCom2Active,
            EventSetCom1Standby,
            EventSetCom2Standby,
            EventSetTransponderCode,
            EventTextMessage,
            EventSetTimeZuluYear,
            EventSetTimeZuluDay,
            EventSetTimeZuluHours,
            EventSetTimeZuluMinutes,
            // ------------ lights -------------
            EventLandingLightsOff,
            EventLandinglightsOn,
            EventLandingLightsSet,
            EventLandingLightsToggle,
            EventPanelLightsOff,
            EventPanelLightsOn,
            EventPanelLightsSet,
            EventStrobesOff,
            EventStrobesOn,
            EventStrobesSet,
            EventStrobesToggle,
            EventToggleBeaconLights,
            EventToggleCabinLights,
            EventToggleLogoLights,
            EventToggleNavLights,
            EventToggleRecognitionLights,
            EventToggleTaxiLights,
            EventToggleWingLights
        };

        //! Struct to trace send ids
        struct TraceFsxSendId
        {
            //! Ctor
            TraceFsxSendId(DWORD sendId, DWORD simObjectId, const QString &comment) :
                sendId(sendId), simObjectId(simObjectId), comment(comment)
            { }

            // DWORD is unsigned
            DWORD sendId = 0;      //!< the send id
            DWORD simObjectId = 0; //!< corresponding CSimConnectObject
            QString comment;       //!< where sent
        };

        //! FSX Simulator Implementation
        class CSimulatorFsxCommon : public BlackSimPlugin::FsCommon::CSimulatorFsCommon
        {
            Q_OBJECT

        public:
            //! Constructor, parameters as in \sa BlackCore::ISimulatorFactory::create
            CSimulatorFsxCommon(
                const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                BlackMisc::Weather::IWeatherGridProvider *weatherGridProvider,
                QObject *parent = nullptr);

            //! Destructor
            virtual ~CSimulatorFsxCommon();

            //! \name ISimulator implementations
            //! @{
            virtual bool connectTo() override;
            virtual bool disconnectFrom() override;
            virtual bool physicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &newRemoteAircraft) override;
            virtual bool physicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual int  physicallyRemoveAllRemoteAircraft() override;
            virtual bool updateOwnSimulatorCockpit(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft, const BlackMisc::CIdentifier &originator) override;
            virtual bool updateOwnSimulatorSelcal(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator) override;
            virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const override;
            virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const override;
            virtual bool isPhysicallyRenderedAircraft(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Aviation::CCallsignSet physicallyRenderedAircraft() const override;
            virtual bool setInterpolatorMode(BlackMisc::Simulation::CInterpolatorMulti::Mode mode, const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual void clearAllRemoteAircraftData() override;
            virtual BlackMisc::CStatusMessageList debugVerifyStateAfterAllAircraftRemoved() const override;
            //! @}

            //! \copydoc BlackMisc::Simulation::ISimulationEnvironmentProvider::requestElevation
            virtual bool requestElevation(const BlackMisc::Geo::ICoordinateGeodetic &reference, const BlackMisc::Aviation::CCallsign &callsign) override;

        protected:
            //! SimConnect Callback
            static void CALLBACK SimConnectProc(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext);

            //! \name Interface implementations
            //! @{
            virtual bool isConnected() const override;
            virtual bool isSimulating() const override;
            //! @}

            //! \name Base class overrides
            //! @{
            virtual void reset() override;
            virtual void initSimulatorInternals() override;
            virtual void injectWeatherGrid(const BlackMisc::Weather::CWeatherGrid &weatherGrid) override;
            //! @}

            //! Timer event (our SimConnect event loop), runs dispatch
            //! \sa m_timerId
            //! \sa CSimulatorFsxCommon::dispatch
            virtual void timerEvent(QTimerEvent *event) override;

            //! \addtogroup swiftdotcommands
            //! @{
            //! <pre>
            //! .drv sendid  on|off      tracing simCOnnect sendId on/off
            //! </pre>
            //! @}
            virtual bool parseDetails(const BlackMisc::CSimpleCommandParser &parser) override;

            //! Get new request id, overflow safe
            SIMCONNECT_DATA_REQUEST_ID obtainRequestIdForSimData();

            //! Get new request id, overflow safe
            SIMCONNECT_DATA_REQUEST_ID obtainRequestIdForProbe();

            //! Request for sim data (request in range of sim data)?
            static bool isRequestForSimData(DWORD requestId) { return requestId >= (RequestIdSimDataStart + RequestSimDataOffset) && requestId < (RequestIdSimDataStart + RequestSimDataOffset + MaxSimObjects); }

            //! Request for lights (request in range of lights)?
            static bool isRequestForLights(DWORD requestId) { return requestId >= (RequestIdSimDataStart + RequestLightsOffset) && requestId < (RequestIdSimDataStart + RequestLightsOffset + MaxSimObjects); }

            //! Request for probe (elevation)?
            static bool isRequestForProbe(DWORD requestId) { return requestId >= RequestIdTerrainProbeStart && requestId <= RequestIdTerrainProbeEnd; }

            //! Register help
            static void registerHelp();

            //! Callsign for pending request
            BlackMisc::Aviation::CCallsign getCallsignForPendingProbeRequests(DWORD requestId, bool remove);

            HANDLE m_hSimConnect = nullptr; //!< handle to SimConnect object
            DispatchProc m_dispatchProc = &CSimulatorFsxCommon::SimConnectProc; //!< called function for dispatch, can be overriden by specialized P3D function
            QMap<DWORD, BlackMisc::Aviation::CCallsign> m_pendingProbeRequests; //!< pending elevation requests

        private:
            //! Reason for adding an aircraft
            enum AircraftAddMode
            {
                ExternalCall,     //!< normal external request to add aircraft
                AddByTimer,       //!< add pending aircraft by timer
                AddAfterAdded,    //!< add pending because object successfully added
                AddedAfterRemoved //!< added again after removed
            };

            //! Mode as string
            const QString &modeToString(AircraftAddMode mode);

            //! Dispatch SimConnect messages
            //! \remark very frequently called
            void dispatch();

            //! Implementation of add remote aircraft, which also handles FSX specific adding one by one
            //! \remark main purpose of this function is to only add one aircraft at a time, and only if simulator is not paused/stopped
            bool physicallyAddRemoteAircraftImpl(const BlackMisc::Simulation::CSimulatedAircraft &newRemoteAircraft, AircraftAddMode addMode);

            //! Add AI object for terrain probe
            //! \remark experimental
            bool physicallyAddAITerrainProbe(const BlackMisc::Geo::ICoordinateGeodetic &coordinate);

            //! Remove aircraft no longer in provider
            //! \remark kind of cleanup function, in an ideal this should never need to cleanup something
            BlackMisc::Aviation::CCallsignSet physicallyRemoveAircraftNotInProvider();

            //! Verify that an object has been added in simulator
            //! \remark checks if the object was really added after an "add request" and not directly removed again
            void verifyAddedRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraftIn);

            //! Add next aircraft based on timer
            void addPendingAircraftByTimer();

            //! Add next aircraft after another has been confirmed
            void addPendingAircraftAfterAdded();

            //! Try to add the next aircraft (one by one)
            void addPendingAircraft(AircraftAddMode mode);

            //! Remove as m_addPendingAircraft and m_aircraftToAddAgainWhenRemoved
            void removeFromAddPendingAndAddAgainAircraft(const BlackMisc::Aviation::CCallsign &callsign);

            //! Call this method to declare the simulator connected
            void setSimConnected();

            //! Called when simulator has started
            void onSimRunning();

            //! Deferred version of onSimRunning to avoid jitter
            void onSimRunningDefered(qint64 referenceTs);

            //! Slot called every visual frame
            void onSimFrame();

            //! Called when simulator has stopped, e.g. by selecting the "select aircraft screen"
            void onSimStopped();

            //! Simulator is going down
            void onSimExit();

            //! Init when connected
            HRESULT initWhenConnected();

            //! Initialize SimConnect system events
            HRESULT initEvents();

            //! Initialize SimConnect data definitions
            HRESULT initDataDefinitionsWhenConnected();

            //! Update remote aircraft
            //! \remark this is where the interpolated data are set
            void updateRemoteAircraft();

            //! Update remote aircraft parts (send to FSX)
            bool updateRemoteAircraftParts(const CSimConnectObject &simObject,
                                           const BlackMisc::Aviation::CAircraftParts &parts, const BlackMisc::Simulation::CPartsStatus &partsStatus);

            //! Send parts to simulator
            bool sendRemoteAircraftPartsToSimulator(const CSimConnectObject &simObject, DataDefinitionRemoteAircraftPartsWithoutLights &ddRemoteAircraftParts, const BlackMisc::Aviation::CAircraftLights &lights);

            //! Send lights to simulator (those which have to be toggled)
            //! \remark challenge here is that I can only sent those value if I have already obtained the current light state from simulator
            //! \param force send lights even if they appear to be the same
            void sendToggledLightsToSimulator(const CSimConnectObject &simObject, const BlackMisc::Aviation::CAircraftLights &lightsWanted, bool force = false);

            //! Called when data about our own aircraft are received
            void updateOwnAircraftFromSimulator(const DataDefinitionOwnAircraft &simulatorOwnAircraft);

            //! Remote aircraft data sent from simulator
            void updateRemoteAircraftFromSimulator(const CSimConnectObject &simObject, const DataDefinitionRemoteAircraftSimData &remoteAircraftData);

            //! Probe data sent from simulator
            void updatProbeFromSimulator(const BlackMisc::Aviation::CCallsign &callsign, const DataDefinitionRemoteAircraftSimData &remoteAircraftData);

            //! Update from SB client area
            void updateOwnAircraftFromSimulator(const DataDefinitionClientAreaSb &sbDataArea);

            //! An AI aircraft was added in the simulator
            bool simulatorReportedObjectAdded(DWORD objectId);

            //! An AI probe was added in the simulator
            bool simulatorReportedProbeAdded(DWORD objectId);

            //! Simulator reported that AI aircraft was removed
            bool simulatorReportedObjectRemoved(DWORD objectID);

            //! Set ID of a SimConnect object, so far we only have an request id in the object
            bool setSimConnectObjectId(DWORD requestId, DWORD objectId);

            //! Set ID of a SimConnect object, so far we only have an request id in the object
            bool setSimConnectProbeId(DWORD requestId, DWORD objectId);

            //! Remember current lights
            bool setCurrentLights(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftLights &lights);

            //! Remember lights sent
            bool setLightsAsSent(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftLights &lights);

            //! Display receive exceptions?
            bool stillDisplayReceiveExceptions();

            //! The simconnect related objects
            const CSimConnectObjects &getSimConnectObjects() const { return m_simConnectObjects; }

            //! The simconnect related probes
            const CSimConnectObjects &getSimConnectProbes() const { return m_simConnectProbes; }

            //! Format conversion
            SIMCONNECT_DATA_INITPOSITION aircraftSituationToFsxPosition(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! Format conversion
            SIMCONNECT_DATA_INITPOSITION coordinateToFsxPosition(const BlackMisc::Geo::ICoordinateGeodetic &coordinate);

            //! Sync time with user's computer
            void synchronizeTime(const BlackMisc::PhysicalQuantities::CTime &zuluTimeSim, const BlackMisc::PhysicalQuantities::CTime &localTimeSim);

            //! Request data for a CSimConnectObject (aka remote aircraft)
            bool requestPositionDataForSimObject(const CSimConnectObject &simObject, SIMCONNECT_PERIOD period = SIMCONNECT_PERIOD_SECOND);

            //! Request data for the terrain probe
            bool requestTerrainProbeData(const BlackMisc::Aviation::CCallsign &callsign);

            //! Request lights for a CSimConnectObject
            bool requestLightsForSimObject(const CSimConnectObject &simObject);

            //! Stop requesting data for CSimConnectObject
            bool stopRequestingDataForSimObject(const CSimConnectObject &simObject);

            //! FSX position as string
            static QString fsxPositionToString(const SIMCONNECT_DATA_INITPOSITION &position);

            //! Get the callsigns which are no longer in the provider, but still in m_simConnectObjects
            BlackMisc::Aviation::CCallsignSet getCallsignsMissingInProvider() const;

            //! Set tracing on/off
            void setTraceSendId(bool traceSendId) { m_traceSendId = traceSendId; }

            //! Trace the send id
            void traceSendId(DWORD simObjectId, const QString &function, const QString &details = {});

            //! Get the trace details, otherwise empty string
            QString getSendIdTraceDetails(DWORD sendId) const;

            //! Remove all probes
            int removeAllProbes();

            //! Insert an new SimConnect object
            CSimConnectObject insertNewSimConnectObject(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, DWORD requestId);

            //! Encapsulates creating QString from FSX string data
            static QString fsxCharToQString(const char *fsxChar, int size = -1);

            static constexpr int GuessRemoteAircraftPartsCycle = 20; //!< guess every n-th cycle
            static constexpr int SkipUpdateCyclesForCockpit = 10;    //!< skip x cycles before updating cockpit again
            static constexpr int IgnoreReceiveExceptions = 10;       //!< skip exceptions when displayed more than x times
            static constexpr int MaxSimObjects = 10000;              //!< max.number of SimObjects at the same time
            static constexpr int MaxSendIdTraces = 10000;            //!< max.traces of send id
            static constexpr int RequestIdSimDataStart = static_cast<int>(CSimConnectDefinitions::RequestEndMarker);
            static constexpr int RequestIdSimDataEnd = RequestIdSimDataStart + MaxSimObjects - 1;
            static constexpr int RequestSimDataOffset = 0 * MaxSimObjects; //!< range for sim data requests
            static constexpr int RequestLightsOffset  = 1 * MaxSimObjects; //!< range for lights
            static constexpr int RequestIdTerrainProbeStart = 2 * MaxSimObjects + RequestIdSimDataEnd + 1; //!< range for terrain probe
            static constexpr int RequestIdTerrainProbeEnd = (RequestIdTerrainProbeStart + 1000) - 1;
            static constexpr int AddPendingAircraftIntervalMs = 20 * 1000;
            static constexpr int DispatchIntervalMs = 10;      //!< how often with run the FSX event queue
            static constexpr int DeferSimulatingFlagMs = 1500; //!< simulating can jitter at startup (simulating->stopped->simulating, multiple start events), so we defer detection
            static constexpr int DeferResendingLights = 2500;  //!< Resend light state when aircraft light state was not yet available

            QString m_simConnectVersion;            //!< SimConnect version
            bool m_simConnected  = false;           //!< Is simulator connected?
            bool m_simSimulating = false;           //!< Simulator running?
            bool m_useSbOffsets  = true;            //!< with SB offsets
            bool m_traceSendId   = false;           //!< trace the send ids, meant for dedugging
            qint64 m_simulatingChangedTs = -1;      //!< timestamp, when simulating changed (used to avoid jitter)
            int  m_syncDeferredCounter =  0;        //!< Set when synchronized, used to wait some time
            int  m_skipCockpitUpdateCycles = 0;     //!< skip some update cycles to allow changes in simulator cockpit to be set
            int  m_interpolationRequest = 0;        //!< current interpolation request
            int  m_dispatchErrors = 0;              //!< number of dispatched failed, \sa dispatch
            int  m_receiveExceptionCount = 0;       //!< exceptions
            QList<TraceFsxSendId> m_sendIdTraces;   //!< Send id traces for debugging
            CSimConnectObjects m_simConnectObjects; //!< AI objects and their object / request ids
            CSimConnectObjects m_simConnectProbes;  //!< AI terrain probes
            CSimConnectObjects m_simConnectObjectsPositionAndPartsTraces; //!< position/parts received, but object not yet added, excluded, disabled etc.
            SIMCONNECT_DATA_REQUEST_ID m_requestIdSimData = static_cast<SIMCONNECT_DATA_REQUEST_ID>(RequestIdSimDataStart);    //!< request id, use obtainRequestIdForSimData() to get id
            SIMCONNECT_DATA_REQUEST_ID m_requestIdProbe = static_cast<SIMCONNECT_DATA_REQUEST_ID>(RequestIdTerrainProbeStart); //!< request id, use obtainRequestIdForSimData() to get id
            BlackMisc::Simulation::CSimulatedAircraftList m_addPendingAircraft; //!< aircraft awaiting to be added
            QTimer m_addPendingSimObjTimer; //!< updating of sim objects awaiting to be added
        };

        //! Listener for FSX
        class CSimulatorFsxCommonListener : public BlackCore::ISimulatorListener
        {
            Q_OBJECT

        public:
            //! Constructor
            CSimulatorFsxCommonListener(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

            //! \copydoc BlackCore::ISimulatorListener::backendInfo
            virtual QString backendInfo() const override;

        protected:
            //! \copydoc BlackCore::ISimulatorListener::startImpl
            virtual void startImpl() override;

            //! \copydoc BlackCore::ISimulatorListener::stopImpl
            virtual void stopImpl() override;

            //! Test if connection can be established
            void checkConnection();

            //! Check simulator version and type
            bool checkVersionAndSimulator() const;

            //! Check the simconnect.dll
            bool checkSimConnectDll() const;

        private:
            QTimer  m_timer { this };
            QString m_simulatorVersion;
            QString m_simConnectVersion;
            QString m_simulatorName;
            QString m_simulatorDetails;

            //! SimConnect Callback (simplified version for listener)
            //! \sa CSimConnectObjects::SimConnectProc
            static void CALLBACK SimConnectProc(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext);
        };
    }
} // namespace

#endif // guard
