/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_SIMULATOR_FSX_H
#define BLACKCORE_SIMULATOR_FSX_H

#include <blackcore/simulator.h>
#include "simconnect_datadefinition.h"
#include "blackmisc/avaircraft.h"
#include <QObject>
#include "simconnect/SimConnect.h"
#include <windows.h>

namespace BlackCore
{
    namespace FSX
    {
        //! \brief SimConnect Event ID's
        enum EVENT_ID {
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
        class CSimulatorFSX : public ISimulator
        {
            Q_OBJECT
        public:
            //! \brief Constructor
            CSimulatorFSX(QObject *parent = nullptr);

            //! \copydoc ISimulator::isConnected()
            virtual bool isConnected() const override;

            //! \copydoc ISimulator::getOwnAircraft()
            virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const override { return m_ownAircraft; }

            /*!
             * \brief SimConnect Callback
             * \param pData
             * \param cbData
             * \param pContext
             */
            static void CALLBACK SimConnectProc(SIMCONNECT_RECV* pData, DWORD cbData, void *pContext);

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
            void setOwnAircraft(OwnAircraft aircraft);

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

            bool    m_isConnected; //!< Is sim connected
            bool    m_simRunning; //!< Sim running.
            HANDLE  m_hSimConnect; //!< Handle to SimConnect object

            BlackMisc::Aviation::CAircraft m_ownAircraft; //!< Object representing our own aircraft from simulator

        };
    }

} // namespace BlackCore

#endif // BLACKCORE_SIMULATOR_FSX_H
