/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIMPLUGIN_FSX_SIMCONNECT_DATADEFINITION_H
#define BLACKSIMPLUGIN_FSX_SIMCONNECT_DATADEFINITION_H

#include <QtGlobal>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <simconnect/SimConnect.h>
#include <windows.h>

namespace BlackSimPlugin
{
    namespace Fsx
    {
        //! \brief Data struct of our own aircraft
        //! \sa SimConnect variables http://msdn.microsoft.com/en-us/library/cc526981.aspx
        //! \sa SimConnect events http://msdn.microsoft.com/en-us/library/cc526980.aspx
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

        //! Data struct of aircraft position
        struct DataDefinitionRemoteAircraftSituation
        {
            SIMCONNECT_DATA_INITPOSITION position;  //!< Position struct
        };

        //! Data struct of aircraft position
        struct DataDefinitionGearHandlePosition
        {
            qint32 gearHandlePosition;  //!< Bool, 1 if gear handle is applied otherwise 0
        };

        //! Handles SimConnect data definitions
        class CSimConnectDataDefinition
        {
        public:

            //! SimConnect definiton IDs
            enum DataDefiniton
            {
                DataOwnAircraft,
                DataDefinitionRemoteAircraftSituation,
                DataDefinitionGearHandlePosition
            };

            //! SimConnect Event IDs
            enum Events {
                EventSetCom1Active = 100, // not overlapping with DataDefinition
                EventSetCom2Active,
                EventSetCom1Standby,
                EventSetCom2Standby,
                EventSetTransponderCode
            };

            //! SimConnect request IDs
            enum Requests
            {
                RequestOwnAircraft = 1000,
                RequestRemoveAircraft = 2000
            };


            //! Constructor
            CSimConnectDataDefinition();

            //! Initialize all data definitions
            static HRESULT initDataDefinitions(const HANDLE hSimConnect);

            //! Initialize data definition for our own aircraft
            static HRESULT initOwnAircraft(const HANDLE hSimConnect);

            //! Initialize data definition for remote aircrafts
            static HRESULT initRemoteAircraftSituation(const HANDLE hSimConnect);

            //! Initialize data definition for remote aircraft configuration
            static HRESULT initGearHandlePosition(const HANDLE hSimConnect);

            //! Initialize events required setting cockpit values
            static HRESULT initSetCockpitEvents(const HANDLE hSimConnect);

        };
    }
}

#endif // guard
