/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_FSX_H
#define BLACKSIMPLUGIN_SIMULATOR_FSX_H

#include "simconnectdatadefinition.h"
#include "simconnectobject.h"
#include "../fscommon/simulatorfscommon.h"
#include "blackcore/simulator/simulatorsettings.h"
#include "blackcore/simulator.h"
#include "blackmisc/interpolatorlinear.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
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
    namespace Fsx
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
            EventToggleTaxiLights
        };

        //! FSX Simulator Implementation
        class CSimulatorFsx : public BlackSimPlugin::FsCommon::CSimulatorFsCommon
        {
            Q_OBJECT

        public:
            //! Constructor, parameters as in \sa BlackCore::ISimulatorFactory::create
            CSimulatorFsx(
                const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                BlackMisc::Weather::IWeatherGridProvider *weatherGridProvider,
                QObject *parent = nullptr);

            //! Destructor
            virtual ~CSimulatorFsx();

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
            //! @}

            //! Display receive exceptions?
            bool stillDisplayReceiveExceptions();

            //! Called when data about our own aircraft are received
            void updateOwnAircraftFromSimulator(DataDefinitionOwnAircraft simulatorOwnAircraft);

            //! Update from SB client area
            void updateOwnAircraftFromSimulator(DataDefinitionClientAreaSb sbDataArea);

            //! An AI aircraft was added in the simulator
            bool simulatorReportedObjectAdded(DWORD objectID);

            //! Simulator reported that AI aircraft was removed
            bool simulatorReportedObjectRemoved(DWORD objectID);

            //! Set ID of a SimConnect object, so far we only have an request id in the object
            bool setSimConnectObjectId(DWORD requestID, DWORD objectID);

            //! The simconnect related objects
            const CSimConnectObjects &getSimConnectObjects() const { return m_simConnectObjects; }

        protected:
            //! \copydoc BlackCore::ISimulator::isConnected()
            virtual bool isConnected() const override;

            //! \copydoc BlackCore::ISimulator::isSimulating
            virtual bool isSimulating() const override;

            //! Timer event (our SimConnect event loop), runs ps_dispatch
            //! \sa m_simconnectTimerId
            virtual void timerEvent(QTimerEvent *event) override;

        private slots:
            //! Dispatch SimConnect messages
            void ps_dispatch();

            //! Remove aircraft not in provider anymore
            //! \remark kind of cleanup function, in an ideal this should never need to cleanup something
            BlackMisc::Aviation::CCallsignSet ps_physicallyRemoveAircraftNotInProvider();

            //! Handle that an object has been added
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

            //! Init when connected
            HRESULT initWhenConnected();

            //! Initialize SimConnect system events
            HRESULT initEvents();

            //! Initialize SimConnect data definitions
            HRESULT initDataDefinitionsWhenConnected();

            //! Update remote aircraft
            void updateRemoteAircraft();

            //! Update remote airacraft parts (send to FSX)
            bool updateRemoteAircraftParts(const CSimConnectObject &simObj, const BlackMisc::Aviation::CAircraftPartsList &parts,
                                           BlackMisc::IInterpolator::PartsStatus partsStatus, const BlackMisc::Aviation::CAircraftSituation &interpolatedSituation, bool isOnGround) const;

            //! Format conversion
            SIMCONNECT_DATA_INITPOSITION aircraftSituationToFsxPosition(const BlackMisc::Aviation::CAircraftSituation &situation, bool guessOnGround = true);

            //! Sync time with user's computer
            void synchronizeTime(const BlackMisc::PhysicalQuantities::CTime &zuluTimeSim, const BlackMisc::PhysicalQuantities::CTime &localTimeSim);

            //! Inject weather grid to simulator
            void injectWeatherGrid(const BlackMisc::Weather::CWeatherGrid &weatherGrid);

            //! Reload weather settings
            void reloadWeatherSettings();

            //! Reset values when restarted
            virtual void reset() override;

            //! Clear all aircraft lists
            virtual void clearAllAircraft() override;

            //! FSX position as string
            static QString fsxPositionToString(const SIMCONNECT_DATA_INITPOSITION &position);

            //! Get the callsigns which are no longer in the provider, but still in m_simConnectObjects
            BlackMisc::Aviation::CCallsignSet getCallsignsMissingInProvider() const;

            static constexpr int SkipUpdateCyclesForCockpit = 10; //!< skip x cycles before updating cockpit again
            static constexpr int IgnoreReceiveExceptions = 10;    //!< skip exceptions when displayed more than x times
            bool m_simConnected  = false;           //!< Is simulator connected?
            bool m_simSimulating = false;           //!< Simulator running?
            bool m_useSbOffsets  = true;            //!< with SB offsets
            int  m_syncDeferredCounter =  0;        //!< Set when synchronized, used to wait some time
            int  m_simconnectTimerId   = -1;        //!< Timer identifier
            int  m_skipCockpitUpdateCycles = 0;     //!< Skip some update cycles to allow changes in simulator cockpit to be set
            int  m_interpolationRequest  = 0;       //!< current interpolation request
            int  m_interpolationsSkipped = 0;       //!< number of skipped interpolation request
            int  m_requestId = 1;                   //!< request id
            int  m_dispatchErrors = 0;              //!< number of dispatched failed, \sa ps_dispatch
            int  m_receiveExceptionCount = 0;       //!< exceptions
            HANDLE m_hSimConnect = nullptr;         //!< Handle to SimConnect object
            CSimConnectObjects m_simConnectObjects; //!< AI objects and their object / request ids
            BlackMisc::Simulation::CSimulatedAircraftList m_outOfRealityBubble; //!< aircraft removed by FSX because they are out of reality bubble
            BlackMisc::Geo::CCoordinateGeodetic m_lastWeatherPosition;          //!< Own aircraft position at which weather was fetched and injected last
            BlackMisc::CSetting<BlackCore::Simulator::TSelectedWeatherScenario> m_weatherScenarioSettings { this, &CSimulatorFsx::reloadWeatherSettings };
            QTimer m_realityBubbleTimer { this };   //!< updating of aircraft out of reality bubble
        };

        //! Listener for FSX
        class CSimulatorFsxListener : public BlackCore::ISimulatorListener
        {
            Q_OBJECT

        public:
            //! Constructor
            CSimulatorFsxListener(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

        public slots:
            //! \copydoc BlackCore::ISimulatorListener::start
            virtual void start() override;

            //! \copydoc BlackCore::ISimulatorListener::stop
            virtual void stop() override;

        private slots:
            //! Test if connection can be established
            void ps_checkConnection();

        private:
            QTimer *m_timer { nullptr };
        };
    }
} // namespace

#endif // guard
