/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIMPLUGIN_SIMULATOR_FSX_H
#define BLACKSIMPLUGIN_SIMULATOR_FSX_H

#include "simconnect_datadefinition.h"
#include "simconnect_object.h"
#include "fsuipc.h"
#include "blackcore/simulator.h"
#include "blackcore/interpolator_linear.h"
#include "blackmisc/avaircraft.h"
#include "blackmisc/avairportlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/nwaircraftmodel.h"
#include "blacksim/simulatorinfo.h"

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
        //! Factory implementation to create CSimulatorFsx instances
        class Q_DECL_EXPORT CSimulatorFsxFactory : public QObject, public BlackCore::ISimulatorFactory
        {
            Q_OBJECT
            // TODO: @RW, move this string into CProject please
            Q_PLUGIN_METADATA(IID "net.vatsim.PilotClient.BlackCore.SimulatorInterface")
            Q_INTERFACES(BlackCore::ISimulatorFactory)

        public:
            //! \copydoc BlackCore::ISimulatorFactory::create()
            virtual BlackCore::ISimulator *create(QObject *parent) override;

            //! Simulator info
            virtual BlackSim::CSimulatorInfo getSimulatorInfo() const override;
        };

        //! SimConnect Event IDs
        enum EventIds
        {
            EventSimStatus,
            EventObjectAdded,
            EventObjectRemoved,
            EventSlewOn,
            EventFrame,
            EventFreezeLat,
            EventFreezeAlt,
            EventFreezeAtt,
            EventSetCom1Active,
            EventSetCom2Active,
            EventSetCom1Standby,
            EventSetCom2Standby,
            EventSetTransponderCode,
            EventTextMessage
        };

        //! FSX Simulator Implementation
        class CSimulatorFsx : public BlackCore::ISimulator
        {
            Q_OBJECT
        public:
            //! Constructor
            CSimulatorFsx(QObject *parent = nullptr);

            virtual ~CSimulatorFsx();

            //! \copydoc ISimulator::isConnected()
            virtual bool isConnected() const override;

            //! FSUIPC connected?
            bool isFsuipcConnected() const;

            //! \copydoc ISimulator::canConnect()
            virtual bool canConnect() override;

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
            virtual void addRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign, const QString &type, const BlackMisc::Aviation::CAircraftSituation &initialSituation) override;

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

            //! \brief Called when simulat has been started

            //! \copydoc ISimulator::getAircraftModel()
            virtual BlackMisc::Network::CAircraftModel getAircraftModel() const override { return m_aircraftModel; }

            //! \copydoc ISimulator::getAirportsInRange
            virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const override;


            //! \brief Called when sim has started
            void onSimRunning();

            //! \brief Called when sim has stopped
            void onSimStopped();

            //! \brief Slot called every visual frame
            void onSimFrame();

            //! Called when data about our own aircraft are received
            void updateOwnAircraftFromSim(DataDefinitionOwnAircraft simulatorOwnAircraft);

            /*!
             * \brief Set ID of a SimConnect object
             * \param requestID
             * \param objectID
             */
            void setSimconnectObjectID(DWORD requestID, DWORD objectID);

            //! \private
            void onSimExit();

            //! \private
            void setAircraftModel(const BlackMisc::Network::CAircraftModel &model);

        protected:
            //! Timer event
            virtual void timerEvent(QTimerEvent *event);

        private slots:

            //! Dispatch SimConnect messages
            void dispatch();

            //! Called when asynchronous connection to Simconnect has finished
            void connectToFinished();

        private:

            //! Remove a remote aircraft
            void removeRemoteAircraft(const CSimConnectObject &simObject);

            //! Initialize SimConnect system events
            HRESULT initEvents();

            //! Initialize SimConnect data definitions
            HRESULT initDataDefinitions();

            //! Update what?
            void update(); // TODO: @RW, please rename, update is meaningless: what is updated?

            static const int SkipUpdateCyclesForCockpit = 10; //!< skip x cycles before updating cockpit again
            bool    m_isConnected; //!< Is simulator connected?
            bool    m_simRunning;  //!< Simulator running?
            HANDLE  m_hSimConnect; //!< Handle to SimConnect object
            uint    m_nextObjID; //!< object ID TODO: also used as request id, where to we place other request ids as for facilities
            QString simulatorDetails;
            BlackSim::CSimulatorInfo m_simulatorInfo;
            BlackMisc::Aviation::CAircraft m_ownAircraft; //!< Object representing our own aircraft from simulator
            QHash<BlackMisc::Aviation::CCallsign, CSimConnectObject> m_simConnectObjects;
            BlackMisc::Network::CAircraftModel m_aircraftModel;
            BlackMisc::Aviation::CAirportList m_airportsInRange;

            int m_simconnectTimerId;
            int m_skipCockpitUpdateCycles; //!< Skip some update cycles to allow changes in simulator cockpit to be set
            QFutureWatcher<bool> m_watcherConnect;
            QScopedPointer<CFsuipc> m_fsuipc;
        };
    }

} // namespace BlackCore

#endif // guard
