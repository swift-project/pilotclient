/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FSX_SIMCONNECT_DATADEFINITION_H
#define BLACKSIMPLUGIN_FSX_SIMCONNECT_DATADEFINITION_H

#include <QtGlobal>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <simconnect/SimConnect.h>
#include <windows.h>
#include <algorithm>
#include <QString>

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
            double simOnGround;     //!< Is aircraft on ground?

            double lightStrobe;         //!< Is strobe light on?
            double lightLanding;        //!< Is landing light on?
            double lightTaxi;           //!< Is taxi light on?
            double lightBeacon;         //!< Is beacon light on?
            double lightNav;            //!< Is nav light on?
            double lightLogo;           //!< Is logo light on?

            double transponderCode; //!< Transponder Code
            double com1ActiveMHz;   //!< COM1 active frequency
            double com2ActiveMHz;   //!< COM2 active frequency
            double com1StandbyMHz;  //!< COM1 standby frequency
            double com2StandbyMHz;  //!< COM1 standby frequency

            double flapsHandlePosition;     //!< Flaps handle position in percent
            double spoilersHandlePosition;  //!< Spoilers out?
            double gearHandlePosition;      //!< Gear handle position

            double numberOfEngines;     //!< Number of engines
            double engine1Combustion;   //!< Engine 1 combustion flag
            double engine2Combustion;   //!< Engine 2 combustion flag
            double engine3Combustion;   //!< Engine 3 combustion flag
            double engine4Combustion;   //!< Engine 4 combustion flag
        };

        //! Data struct of aircraft position
        struct DataDefinitionOwnAircraftModel
        {
            char title[256]; //!< Aircraft model string
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

        //! Data struct simulator environment
        struct DataDefinitionSimEnvironment
        {
            qint32 zuluTimeSeconds;  //!< Simulator zulu (GMT) ime in secs.
            qint32 localTimeSeconds; //!< Simulator local time in secs.
        };

        //! The whole SB data area
        struct DataDefinitionClientAreaSb
        {
            byte data[128] {}; //!< 128 bytes of data, offsets http://www.squawkbox.ca/doc/sdk/fsuipc.php

            //! Standby = 1, else 0
            byte getTransponderMode() const { return data[17]; }

            //! Ident = 1, else 0
            byte getIdent() const { return data[19]; }

            //! Ident?
            bool isIdent() const { return getIdent() != 0; }

            //! Standby
            bool isStandby() const { return getTransponderMode() != 0; }

            //! Set default values
            void setDefaultValues()
            {
                std::fill(data, data + 128, 0);
                data[17] = 1; // standby
                data[19] = 0; // no ident
            }
        };

        //! Client areas
        enum ClientAreaId
        {
            ClientAreaSquawkBox
        };

        //! Handles SimConnect data definitions
        class CSimConnectDefinitions
        {
        public:

            //! SimConnect definiton IDs
            enum DataDefiniton
            {
                DataOwnAircraft,
                DataOwnAircraftTitle,
                DataRemoteAircraftSituation,
                DataGearHandlePosition,
                DataSimEnvironment,
                DataClientAreaSb,       //!< whole SB area
                DataClientAreaSbIdent,  //!< ident single value
                DataClientAreaSbStandby //!< standby
            };

            //! SimConnect request IDs
            enum Request
            {
                RequestOwnAircraft,
                RequestRemoveAircraft,
                RequestOwnAircraftTitle,
                RequestSimEnvironment,
                RequestSbData,  // SB client area / XPDR mode ..
            };

            //! Constructor
            CSimConnectDefinitions();

            //! Initialize all data definitions
            static HRESULT initDataDefinitionsWhenConnected(const HANDLE hSimConnect);

            //! Log message category
            static QString getMessageCategory() { return "swift.fsx.simconnect"; }

        private:

            //! Initialize data definition for our own aircraft
            static HRESULT initOwnAircraft(const HANDLE hSimConnect);

            //! Initialize data definition for remote aircrafts
            static HRESULT initRemoteAircraftSituation(const HANDLE hSimConnect);

            //! Initialize data definition for remote aircraft configuration
            static HRESULT initGearHandlePosition(const HANDLE hSimConnect);

            //! Initialize data definition for Simulator environment
            static HRESULT initSimulatorEnvironment(const HANDLE hSimConnect);

            //! Initialize the SB data are
            static HRESULT initSbDataArea(const HANDLE hSimConnect);
        };
    } // namespace
} // namespace

#endif // guard
