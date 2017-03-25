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
#include "../fscommon/simulatorfscommon.h"
#include "blackcore/simulator.h"
#include "blackmisc/simulation/interpolatorlinear.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/simulatorsettings.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/network/client.h"
#include "blackmisc/pixmap.h"

#include <simconnect/SimConnect.h>
#include <QObject>
#include <QtPlugin>
#include <QHash>
#include <QFutureWatcher>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

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

            //! SimConnect Callback
            static void CALLBACK SimConnectProc(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext);

            //! \name ISimulator implementations
            //! @{
            virtual bool connectTo() override;
            virtual bool disconnectFrom() override;
            virtual bool physicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &newRemoteAircraft) override;
            virtual bool physicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual int physicallyRemoveAllRemoteAircraft() override;
            virtual bool updateOwnSimulatorCockpit(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft, const BlackMisc::CIdentifier &originator) override;
            virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const override;
            virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const override;
            virtual bool isPhysicallyRenderedAircraft(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Aviation::CCallsignSet physicallyRenderedAircraft() const override;
            virtual bool setInterpolatorMode(BlackMisc::Simulation::CInterpolatorMulti::Mode mode, const BlackMisc::Aviation::CCallsign &callsign) override;
            //! @}

        protected:
            //! \name Interface implementations
            //! @{
            virtual bool isConnected() const override;
            virtual bool isSimulating() const override;
            //! @}

            //! \name Base class overrides
            //! @{
            virtual void reset() override;
            virtual void clearAllAircraft() override;
            virtual void initSimulatorInternals() override;
            virtual void injectWeatherGrid(const BlackMisc::Weather::CWeatherGrid &weatherGrid) override;
            virtual void ps_remoteProviderAddAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation) override;
            virtual void ps_remoteProviderAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts) override;
            //! @}

            //! Timer event (our SimConnect event loop), runs ps_dispatch
            //! \sa m_simconnectTimerId
            virtual void timerEvent(QTimerEvent *event) override;

        private slots:
            //! Dispatch SimConnect messages
            void ps_dispatch();

            //! Remove aircraft not in provider anymore
            //! \remark kind of cleanup function, in an ideal this should never need to cleanup something
            BlackMisc::Aviation::CCallsignSet ps_physicallyRemoveAircraftNotInProvider();

            //! Handle that an object has been added in simulator
            //! \remark checks if the object was really added after an add request and not directly removed again
            bool ps_deferredSimulatorReportedObjectAdded(const BlackMisc::Aviation::CCallsign &callsign);

            //! Try to add the aircraft currently out of bubble
            void ps_addAircraftCurrentlyOutOfBubble();

        private:
            //! Call this method to declare the simulator connected
            void setSimConnected();

            //! Called when sim has started
            void onSimRunning();

            //! Slot called every visual frame
            void onSimFrame();

            //! Called when simulator has stopped, e.g. by selecting the "select aircraft screen"
            void onSimStopped();

            //! Simulator is going down
            void onSimExit();

            //! Get new request id, overflow safe
            DWORD obtainRequestIdSimData();

            //! Init when connected
            HRESULT initWhenConnected();

            //! Initialize SimConnect system events
            HRESULT initEvents();

            //! Initialize SimConnect data definitions
            HRESULT initDataDefinitionsWhenConnected();

            //! Update remote aircraft
            void updateRemoteAircraft();

            //! Update remote aircraft parts (send to FSX)
            bool updateRemoteAircraftParts(const CSimConnectObject &simObj,
                                           const BlackMisc::Aviation::CAircraftParts &parts, const BlackMisc::Simulation::CPartsStatus &partsStatus);

            //! Update remote aircraft parts by guessing (send to FSX)
            bool guessAndUpdateRemoteAircraftParts(const CSimConnectObject &simObj,
                                                   const BlackMisc::Aviation::CAircraftSituation &interpolatedSituation, const BlackMisc::Simulation::CInterpolationStatus &interpolationStatus);

            //! Send parts to simulator
            bool sendRemoteAircraftPartsToSimulator(const CSimConnectObject &simObj, DataDefinitionRemoteAircraftPartsWithoutLights &ddRemoteAircraftParts, const BlackMisc::Aviation::CAircraftLights &lights);

            //! Send lights to simulator (those which have to be toggled)
            //! \remark challenge here is that I can only sent those value if I have already obtained the current light state from simulator
            void sendToggledLightsToSimulator(const CSimConnectObject &simObj, const BlackMisc::Aviation::CAircraftLights &lightsWanted, bool force = false);

            //! Called when data about our own aircraft are received
            void updateOwnAircraftFromSimulator(const DataDefinitionOwnAircraft &simulatorOwnAircraft);

            //! Remote aircraft data sent from simulator
            void updateRemoteAircraftFromSimulator(const CSimConnectObject &simObject, const DataDefinitionRemoteAircraftSimData &remoteAircraftData);

            //! Update from SB client area
            void updateOwnAircraftFromSimulator(const DataDefinitionClientAreaSb &sbDataArea);

            //! An AI aircraft was added in the simulator
            bool simulatorReportedObjectAdded(DWORD objectID);

            //! Simulator reported that AI aircraft was removed
            bool simulatorReportedObjectRemoved(DWORD objectID);

            //! Set ID of a SimConnect object, so far we only have an request id in the object
            bool setSimConnectObjectId(DWORD requestID, DWORD objectID);

            //! Remember current lights
            bool setCurrentLights(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftLights &lights);

            //! Remember lights sent
            bool setLightsAsSent(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftLights &lights);

            //! Display receive exceptions?
            bool stillDisplayReceiveExceptions();

            //! The simconnect related objects
            const CSimConnectObjects &getSimConnectObjects() const { return m_simConnectObjects; }

            //! Format conversion
            SIMCONNECT_DATA_INITPOSITION aircraftSituationToFsxPosition(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! Sync time with user's computer
            void synchronizeTime(const BlackMisc::PhysicalQuantities::CTime &zuluTimeSim, const BlackMisc::PhysicalQuantities::CTime &localTimeSim);

            //! Request data for a simObject (aka remote aircraft)
            bool requestDataForSimObject(const CSimConnectObject &simObject, SIMCONNECT_PERIOD period = SIMCONNECT_PERIOD_SECOND);

            //! Request lights for a simObject
            bool requestLightsForSimObject(const CSimConnectObject &simObject);

            //! FSX position as string
            static QString fsxPositionToString(const SIMCONNECT_DATA_INITPOSITION &position);

            //! Get the callsigns which are no longer in the provider, but still in m_simConnectObjects
            BlackMisc::Aviation::CCallsignSet getCallsignsMissingInProvider() const;

            //! Request for sim data?
            static bool isRequestForSimData(DWORD requestId) { return requestId >= (RequestSimDataStart + RequestSimDataOffset) && requestId < (RequestSimDataStart + RequestSimDataOffset + SimObjectNumber); }

            //! Request for sim data?
            static bool isRequestForLights(DWORD requestId) { return requestId >= (RequestSimDataStart + RequestLightsOffset) && requestId < (RequestSimDataStart + RequestLightsOffset + SimObjectNumber); }

            static constexpr int GuessRemoteAircraftPartsCycle = 20; //!< guess every n-th cycle
            static constexpr int SkipUpdateCyclesForCockpit = 10;    //!< skip x cycles before updating cockpit again
            static constexpr int IgnoreReceiveExceptions = 10;       //!< skip exceptions when displayed more than x times
            static constexpr int SimObjectNumber = 10000;            //!< max. SimObjects at the same time
            static constexpr int RequestSimDataStart = static_cast<int>(CSimConnectDefinitions::RequestEndMarker);
            static constexpr int RequestSimDataEnd = RequestSimDataStart + SimObjectNumber - 1;
            static constexpr int RequestSimDataOffset = 0 * SimObjectNumber;
            static constexpr int RequestLightsOffset  = 1 * SimObjectNumber;

            QString m_simConnectVersion;            //!< SimConnect version
            bool m_simConnected  = false;           //!< Is simulator connected?
            bool m_simSimulating = false;           //!< Simulator running?
            bool m_useSbOffsets  = true;            //!< with SB offsets
            int  m_syncDeferredCounter =  0;        //!< Set when synchronized, used to wait some time
            int  m_simConnectTimerId = -1;          //!< Timer identifier
            int  m_skipCockpitUpdateCycles = 0;     //!< skip some update cycles to allow changes in simulator cockpit to be set
            int  m_interpolationRequest = 0;        //!< current interpolation request
            int  m_dispatchErrors = 0;              //!< number of dispatched failed, \sa ps_dispatch
            int  m_receiveExceptionCount = 0;       //!< exceptions
            HANDLE m_hSimConnect = nullptr;         //!< handle to SimConnect object
            CSimConnectObjects m_simConnectObjects; //!< AI objects and their object / request ids
            QTimer m_realityBubbleTimer { this };   //!< updating of aircraft out of reality bubble
            DWORD m_requestIdSimData = RequestSimDataStart; //!< request id, use obtainRequestId() to get id
            BlackMisc::Simulation::CSimulatedAircraftList m_outOfRealityBubble; //!< aircraft removed by FSX because they are out of reality bubble
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

        protected:
            //! Test if connection can be established
            void checkConnection();

            //! Check simulator version and type
            bool checkVersionAndSimulator() const;

            //! Check the simconnect.dll
            bool checkSimConnectDll() const;

        private:
            QTimer *m_timer { nullptr };
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
