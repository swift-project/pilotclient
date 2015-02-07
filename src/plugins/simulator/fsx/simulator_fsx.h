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
#include "../fscommon/simulator_fscommon.h"
#include "blackcore/simulator.h"
#include "blackcore/interpolator_linear.h"
#include "blacksim/simulatorinfo.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/avairportlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/nwclient.h"
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
        class CSimulatorFsx : public BlackSimPlugin::FsCommon::CSimulatorFsCommon
        {
            Q_OBJECT

        public:
            //! Constructor, parameters as in \sa BlackCore::ISimulatorFactory::create
            CSimulatorFsx(
                BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                BlackMisc::Simulation::IRenderedAircraftProvider *renderedAircraftProvider,
                QObject *parent = nullptr);

            //! Destructor
            virtual ~CSimulatorFsx();

            //! \copydoc ISimulator::isConnected()
            virtual bool isConnected() const override;

            //! \copydoc ISimulator::isSimulating
            virtual bool isSimulating() const override;

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

            //! \copydoc ISimulator::addRemoteAircraft()
            virtual bool addRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft) override;

            //! \copydoc ISimulator::removeRenderedAircraft()
            virtual bool removeRenderedAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc ISimulator::updateOwnCockpit
            virtual bool updateOwnSimulatorCockpit(const BlackMisc::Aviation::CAircraft &ownAircraft, const QString &originator) override;

            //! \copydoc ISimulator::displayStatusMessage
            virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const override;

            //! \copydoc ISimulator::displayTextMessage()
            virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const override;

            //! Called when sim has started
            void onSimRunning();

            //! Called when sim has stopped
            void onSimStopped();

            //! Slot called every visual frame
            void onSimFrame();

            //! Called when data about our own aircraft are received
            void updateOwnAircraftFromSimulator(DataDefinitionOwnAircraft simulatorOwnAircraft);

            //! Update from SB client area
            void updateOwnAircraftFromSimulator(DataDefinitionClientAreaSb sbDataArea);

            //! Set ID of a SimConnect object
            void setSimConnectObjectID(DWORD requestID, DWORD objectID);

            //! \private
            void onSimExit();

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
            bool removeRenderedAircraft(const CSimConnectObject &simObject);

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

            static const int SkipUpdateCyclesForCockpit = 10; //!< skip x cycles before updating cockpit again
            bool m_simConnected  = false;       //!< Is simulator connected?
            bool m_simRunning    = false;       //!< Simulator running?
            bool m_useSbOffsets  = true;        //!< with SB offsets
            int  m_syncDeferredCounter =  0;    //!< Set when synchronized, used to wait some time
            int  m_simconnectTimerId   = -1;    //!< Timer identifier
            int  m_skipCockpitUpdateCycles = 0; //!< Skip some update cycles to allow changes in simulator cockpit to be set
            HANDLE  m_hSimConnect = nullptr;    //!< Handle to SimConnect object
            uint    m_nextObjID   = 1;          //!< object ID TODO: also used as request id, where to we place other request ids as for facilities
            BlackMisc::PhysicalQuantities::CTime m_syncTimeOffset;   //!< Time offset (if synchronized)
            QHash<BlackMisc::Aviation::CCallsign, CSimConnectObject> m_simConnectObjects;
            QFutureWatcher<bool> m_watcherConnect;
        };
    }

} // namespace BlackCore

#endif // guard
