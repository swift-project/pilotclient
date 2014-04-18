/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIMPLUGIN_FSX_SIMCONNECT_DATADEFINITION_H
#define BLACKSIMPLUGIN_FSX_SIMCONNECT_DATADEFINITION_H

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace BlackSimPlugin
{
    namespace Fsx
    {
        //! \brief Data struct of our own aircraft
        struct DataDefinitionOwnAircraft
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

        //! \brief Data struct of aircraft position
        struct DataDefinitionAircraftPosition
        {
            double latitude;        //!< Latitude
            double longitude;       //!< Longitude
            double altitude;        //!< Altitude
            double trueHeading;     //!< True heading
            double pitch;           //!< Pitch
            double bank;            //!< Bank
        };

        //! \brief Data struct of aircraft position
        struct DataDefinitionAircraftConfiguration
        {
            double gearCenter;          //!< Gear center
            double gearLeft;            //!< Gear left
            double gearRight;           //!< Gear right
            double gearTail;            //!< Gear tail
            double gearAux;             //!< Gear aux
        };

        //! \brief Handles SimConnect data definitions
        class CSimConnectDataDefinition
        {
        public:

            //! \brief SimConnect definiton ID's
            enum DataDefiniton {
                DataOwnAircraft,
                DataAircraftPosition,
                DataAircraftConfiguration
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

            /*!
             * \brief Initialize data definition for remote aircrafts
             * \param hSimConnect
             * \return
             */
            static HRESULT initAircraftPosition(const HANDLE hSimConnect);

            /*!
             * \brief Initialize data definition for remote aircraft configuration
             * \param hSimConnect
             * \return
             */
            static HRESULT initAircraftConfiguration(const HANDLE hSimConnect);
        };
    }
}

#endif // BLACKSIMPLUGIN_FSX_SIMCONNECT_DATADEFINITION_H
