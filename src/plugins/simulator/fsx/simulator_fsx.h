/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIMPLUGIN_SIMULATOR_FSX_H
#define BLACKSIMPLUGIN_SIMULATOR_FSX_H

#include "simconnect_datadefinition.h"
#include "blackcore/simulator.h"
#include "blackcore/interpolator_linear.h"
#include "blackmisc/avaircraft.h"
#include "blacksim/simulatorinfo.h"
#include <simconnect/SimConnect.h>
#include <QObject>
#include <QtPlugin>
#include <QHash>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace BlackSimPlugin
{
    namespace Fsx
    {
        //! \brief Factory implementation to create CSimulatorFsx instances
        class Q_DECL_EXPORT CSimulatorFsxFactory : public QObject, public BlackCore::ISimulatorFactory
        {
            Q_OBJECT
            Q_PLUGIN_METADATA(IID "net.vatsim.PilotClient.BlackCore.SimulatorInterface")
            Q_INTERFACES(BlackCore::ISimulatorFactory)

        public:
            //! \copydoc BlackCore::ISimulatorFactory::create()
            virtual BlackCore::ISimulator *create(QObject *parent) override;
        };

        //! \brief SimConnect Event ID's
        enum EVENT_ID
        {
            EVENT_SIM_STATUS,
            EVENT_OBJECT_ADDED,
            EVENT_OBJECT_REMOVED,
            EVENT_SLEW_ON,
            EVENT_FRAME,
            EVENT_FREEZELAT,
            EVENT_FREEZEALT,
            EVENT_FREEZEATT
        };

        //! \brief FSX Simulator Implementation
        class CSimulatorFsx : public BlackCore::ISimulator
        {
            Q_OBJECT
        public:
            //! \brief Constructor
            CSimulatorFsx(QObject *parent = nullptr);

            //! \copydoc ISimulator::isConnected()
            virtual bool isConnected() const override;

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

            /*!
             * \brief SimConnect Callback
             * \param pData
             * \param cbData
             * \param pContext
             */
            static void CALLBACK SimConnectProc(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext);

            //! \brief Called when sim has started
            void onSimRunning();

            //! \brief Called when sim has stopped
            void onSimStopped();

            //! \brief Slot called every visual frame
            void onSimFrame();

            /*!
             * \brief Called when data about our own aircraft is received
             * \param aircraft
             */
            void setOwnAircraft(DataDefinitionOwnAircraft aircraft);

            /*!
             * \brief Set ID of a SimConnect object
             * \param requestID
             * \param objectID
             */
            void setSimconnectObjectID(DWORD requestID, DWORD objectID);

            //! \private
            void onSimExit();

        protected:
            //! \brief Timer event
            virtual void timerEvent(QTimerEvent *event);

        private slots:
            //! \brief Try to connect
            void checkConnection();

            //! \brief Dispatch SimConnect messages
            void dispatch();

        private:

            struct SimConnectObject
            {
                BlackMisc::Aviation::CCallsign m_callsign;
                BlackCore::CInterpolatorLinear m_interpolator;
                int m_requestId;
                int m_objectId;
            };

            /*!
             * \brief Initialize SimConnect system events
             * \return
             */
            HRESULT initSystemEvents();

            /*!
             * \brief Initialize SimConnect data definitions
             * \return
             */
            HRESULT initDataDefinitions();

            void update();

            bool    m_isConnected; //!< Is sim connected
            bool    m_simRunning; //!< Sim running.
            HANDLE  m_hSimConnect; //!< Handle to SimConnect object
            uint    m_nextObjID;
            BlackSim::CSimulatorInfo m_simulatorInfo;
            BlackMisc::Aviation::CAircraft m_ownAircraft; //!< Object representing our own aircraft from simulator
            QHash<BlackMisc::Aviation::CCallsign, SimConnectObject> m_simConnectObjects;
        };
    }

} // namespace BlackCore

#endif // BLACKSIMPLUGIN_SIMULATOR_FSX_H
