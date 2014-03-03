/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_FSX_SIMCONNECT_DATADEFINITION_H
#define BLACKCORE_FSX_SIMCONNECT_DATADEFINITION_H

#include <windows.h>

namespace BlackCore
{
    namespace FSX
    {
        //! \brief Data struct of our own aircraft
        struct OwnAircraft
        {
            double latitude;        //!< Latitude
            double longitude;       //!< Longitude
            double altitude;        //!< Altitude
            double trueHeading;     //!< True heading
            double pitch;           //!< Pitch
            double bank;            //!< Bank
            double velocity;        //!< Ground velocity
            double simOnGround;     //!< Is sim on ground?
            double transponderCode; //!< Transponder Code
            double com1ActiveMHz;   //!< COM1 active frequency
            double com2ActiveMHz;   //!< COM2 active frequency
            double com1StandbyMHz;  //!< COM1 standby frequency
            double com2StandbyMHz;  //!< COM1 standby frequency
        };

        //! \brief Handles SimConnect data definitions
        class CSimConnectDataDefinition
        {
        public:

            //! \brief SimConnect definiton ID's
            enum DataDefiniton {
                DataOwnAircraft,
            };

            //! \brief SimConnect request ID's
            enum Requests {
                RequestOwnAircraft = 1000
            };

            //! \brief Constructor
            CSimConnectDataDefinition();

            /*!
             * \brief Initialize all data definitions
             * \param hSimConnect
             * \return
             */
            static HRESULT initDataDefinitions(const HANDLE hSimConnect);

            /*!
             * \brief Initialize data definition for our own aircraft
             * \param hSimConnect
             * \return
             */
            static HRESULT initOwnAircraft(const HANDLE hSimConnect);
        };
    }
}

#endif // BLACKCORE__FSX_SIMCONNECT_DATADEFINITION_H
