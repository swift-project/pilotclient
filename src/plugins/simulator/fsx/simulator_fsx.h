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

#include "simconnect_datadefinition.h"
#include "simconnect_object.h"
#include "../fscommon/fsuipc.h"
#include "blackcore/simulator.h"
#include "blackcore/interpolator_linear.h"
#include "blacksim/simulatorinfo.h"
#include "blacksim/fscommon/aircraftmapper.h"
#include "blackmisc/avaircraft.h"
#include "blackmisc/avairportlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/nwaircraftmodel.h"
#include "blackmisc/nwclient.h"

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
            EventSetTimeZuluMinutes
        };

        //! FSX Simulator Implementation
        class CSimulatorFsx : public BlackCore::ISimulator
        {
            Q_OBJECT

        public:
            //! Constructor
            CSimulatorFsx(QObject *parent = nullptr);

            //! Destructor
            virtual ~CSimulatorFsx();

            //! \copydoc ISimulator::isConnected()
            virtual bool isConnected() const override;

            //! \copydoc ISimulator::isSimulating
            virtual bool isSimulating() const override;

            //! FSUIPC connected?
            bool isFsuipcConnected() const;

            //! \copydoc ISimulator::canConnect()
            virtual bool canConnect() const override;

            //! SimConnect Callback
            static void CALLBACK SimConnectProc(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext);

        public slots:

            //! \copydoc ISimulator::connectTo()
            virtual bool connectTo() override;

            //! \copydoc ISimulator::asyncConnectTo()
            virtual void asyncConnectTo() override;

            //! \copydoc ISimulator::disconnectFrom()
            virtual bool disconnectFrom() override;

            //! \copydoc ISimulator::getOwnAircraft()
            virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const override { return m_ownAircraft; }

            //! \copydoc ISimulator::addRemoteAircraft()
            virtual void addRemoteAircraft(const BlackMisc::Aviation::CAircraft &remoteAircraft, const BlackMisc::Network::CClient &remoteClient) override;

            //! \copydoc ISimulator::addAircraftSituation()
            virtual void addAircraftSituation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &initialSituation) override;

            //! \copydoc ISimulator::removeRemoteAircraft()
            virtual void removeRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc ISimulator::getSimulatorInfo()
            virtual BlackSim::CSimulatorInfo getSimulatorInfo() const override;

            //! \copydoc ISimulator::updateOwnCockpit
            virtual bool updateOwnSimulatorCockpit(const BlackMisc::Aviation::CAircraft &ownAircraft) override;

            //! \copydoc ISimulator::displayStatusMessage
            virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const override;

            //! \copydoc ISimulator::displayTextMessage()
            virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const override;

            //! \copydoc ISimulator::getAircraftModel()
            virtual BlackMisc::Network::CAircraftModel getOwnAircraftModel() const override { return m_ownAircraftModel; }

            //! \copydoc BlackCore::ISimulator::getInstalledModels
            virtual BlackMisc::Network::CAircraftModelList getInstalledModels() const override;

            //! \copydoc BlackCore::ISimulator::getCurrentlyMatchedModels
            virtual BlackMisc::Network::CAircraftModelList getCurrentlyMatchedModels() const override { return m_matchedModels; }

            //! \copydoc ISimulator::getAirportsInRange
            virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const override;

            //! \copydoc ISimulator::setTimeSynchronization
            virtual void setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset) override;

            //! \copydoc ISimulator::isTimeSynchronized
            virtual bool isTimeSynchronized() const override { return m_simTimeSynced; }

            //! \copydoc ISimulator::getTimeSynchronizationOffset
            virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override { return m_syncTimeOffset; }

            //! \copydoc ISimulator::isSimPaused
            virtual bool isPaused() const override { return m_simPaused; }

            //! Called when sim has started
            void onSimRunning();

            //! Called when sim has stopped
            void onSimStopped();

            //! Slot called every visual frame
            void onSimFrame();

            //! Called when data about our own aircraft are received
            void updateOwnAircraftFromSim(DataDefinitionOwnAircraft simulatorOwnAircraft);

            //! Update from SB client area
            void updateOwnAircraftFromSim(DataDefinitionClientAreaSb sbDataArea);

            //! Set ID of a SimConnect object
            void setSimConnectObjectID(DWORD requestID, DWORD objectID);

            //! \private
            void onSimExit();

            //! \private
            void setOwnAircraftModel(const BlackMisc::Network::CAircraftModel &model);

        protected:
            //! Timer event
            virtual void timerEvent(QTimerEvent *event);

        private slots:

            //! Dispatch SimConnect messages
            void ps_dispatch();

            //! Called when asynchronous connection to Simconnect has finished
            void ps_connectToFinished();

        private:

            //! Remove a remote aircraft
            void removeRemoteAircraft(const CSimConnectObject &simObject);

            //! Init when connected
            HRESULT initWhenConnected();

            //! Initialize SimConnect system events
            HRESULT initEvents();

            //! Initialize SimConnect data definitions
            HRESULT initDataDefinitionsWhenConnected();

            //! Update other aircrafts
            void updateOtherAircraft();

            //! Format conversion
            SIMCONNECT_DATA_INITPOSITION aircraftSituationToFsxInitPosition(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! Sync time with user's computer
            void synchronizeTime(const BlackMisc::PhysicalQuantities::CTime &zuluTimeSim, const BlackMisc::PhysicalQuantities::CTime &localTimeSim);

            //! Get the mapper singleton
            static BlackSim::FsCommon::CAircraftMapper *mapperInstance();

            //! Experimental model matching
            static BlackMisc::Network::CAircraftModel modelMatching(const BlackMisc::Aviation::CAircraft &remoteAircraft, const BlackMisc::Network::CClient &remoteClient);

            //! SimObjects directory
            static QString simObjectsDir();

            static const int SkipUpdateCyclesForCockpit = 10; //!< skip x cycles before updating cockpit again
            bool m_simConnected  = false;       //!< Is simulator connected?
            bool m_simRunning    = false;       //!< Simulator running?
            bool m_simPaused     = false;       //!< Simulator paused?
            bool m_simTimeSynced = false;       //!< Time synchronized?
            bool m_useSbOffsets  = true;        //!< with SB offsets
            int  m_syncDeferredCounter =  0;    //!< Set when synchronized, used to wait some time
            int  m_simconnectTimerId   = -1;
            int  m_skipCockpitUpdateCycles = 0; //!< Skip some update cycles to allow changes in simulator cockpit to be set
            HANDLE  m_hSimConnect = nullptr;    //!< Handle to SimConnect object
            uint    m_nextObjID   = 1;          //!< object ID TODO: also used as request id, where to we place other request ids as for facilities
            BlackMisc::PhysicalQuantities::CTime m_syncTimeOffset;
            QString simulatorDetails;           //!< describes version etc.
            BlackSim::CSimulatorInfo               m_simulatorInfo;    //!< about the simulator
            BlackMisc::Aviation::CAircraft         m_ownAircraft;      //!< Object representing our own aircraft from simulator
            BlackMisc::Network::CAircraftModel     m_ownAircraftModel; //!< own model
            BlackMisc::Network::CAircraftModelList m_matchedModels;    //!< mapped models
            BlackMisc::Aviation::CAirportList      m_airportsInRange;  //!< airports in range

            QHash<BlackMisc::Aviation::CCallsign, CSimConnectObject> m_simConnectObjects;
            QFutureWatcher<bool> m_watcherConnect;
            QScopedPointer<FsCommon::CFsuipc> m_fsuipc;
        };
    }

} // namespace BlackCore

#endif // guard
