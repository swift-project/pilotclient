// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_FSXCOMMON_SIMCONNECT_DATADEFINITION_H
#define BLACKSIMPLUGIN_FSXCOMMON_SIMCONNECT_DATADEFINITION_H

#include "plugins/simulator/fsxcommon/fsxcommonexport.h"
#include "plugins/simulator/fsxcommon/simconnectwindows.h"
#include "misc/aviation/aircraftlights.h"

#include <algorithm>
#include <QtGlobal>
#include <QString>

namespace swift::misc::aviation
{
    class CAircraftParts;
}
namespace BlackSimPlugin::FsxCommon
{
    //! Data struct of our own aircraft
    //! \sa SimConnect variables http://msdn.microsoft.com/en-us/library/cc526981.aspx
    //! \sa SimConnect events http://msdn.microsoft.com/en-us/library/cc526980.aspx
    struct DataDefinitionOwnAircraft
    {
        double latitudeDeg; //!< Latitude (deg)
        double longitudeDeg; //!< Longitude (deg)
        double altitudeFt; //!< Altitude (ft)
        double altitudeAGLFt; //!< Altitude above ground (ft)
        double pressureAltitudeM; //!< Pressure altitude (m)
        double cgToGroundFt; //!< Static CG to ground (ft)
        double trueHeadingDeg; //!< True heading (deg)
        double pitchDeg; //!< Pitch (deg)
        double bankDeg; //!< Bank (deg)
        double velocity; //!< Ground velocity
        double elevationFt; //!< Elevation (ft)
        double simOnGround; //!< Is aircraft on ground?
        // 12
        double lightStrobe; //!< Is strobe light on?
        double lightLanding; //!< Is landing light on?
        double lightTaxi; //!< Is taxi light on?
        double lightBeacon; //!< Is beacon light on?
        double lightNav; //!< Is nav light on?
        double lightLogo; //!< Is logo light on?
        // 18
        double transponderCode; //!< Transponder Code
        double com1ActiveMHz; //!< COM1 active frequency
        double com2ActiveMHz; //!< COM2 active frequency
        double com1StandbyMHz; //!< COM1 standby frequency
        double com2StandbyMHz; //!< COM2 standby frequency
        double comTransmit1; //!< COM1 transmit, means also receiving
        double comTransmit2; //!< COM2 transmit, means also receiving
        double comReceiveAll; //!< all COMs receiving, or COM:x transmitting or receiving
        double comTest1; //!< COM1 test
        double comTest2; //!< COM2 test
        double comStatus1; //!< COM1 status
        double comStatus2; //!< COM2 status
        // 30
        double flapsHandlePosition; //!< Flaps handle position in percent
        double spoilersHandlePosition; //!< Spoilers out? (flag)
        double gearHandlePosition; //!< Gear handle position (flag)
        // 33
        double numberOfEngines; //!< Number of engines
        double engine1Combustion; //!< Engine 1 combustion flag
        double engine2Combustion; //!< Engine 2 combustion flag
        double engine3Combustion; //!< Engine 3 combustion flag
        double engine4Combustion; //!< Engine 4 combustion flag
        // 38
        double velocityWorldX; //!< Velocity World X
        double velocityWorldY; //!< Velocity World Y
        double velocityWorldZ; //!< Velocity World Z
        double rotationVelocityBodyX; //!< Rotation Velocity Body X
        double rotationVelocityBodyY; //!< Rotation Velocity Body Y
        double rotationVelocityBodyZ; //!< Rotation Velocity Body Z
        // 44
        double altitudeCalibratedFt; //!< Altitude without temperature effect (ft, FS2020)
        // 45
    };

    //! Data struct of aircraft position
    struct DataDefinitionOwnAircraftModel
    {
        char title[256]; //!< Aircraft model string
    };

    //! Data struct of aircraft model data
    struct DataDefinitionRemoteAircraftModel
    {
        double cgToGroundFt; //!< Static CG to ground (ft)
        char atcType[32]; //!< type
        char atcModel[32]; //!< model
        char atcId[32]; //!< id
        char atcAirlineNumber[64]; //!< airline number
        char atcFlightNumber[8]; //!< flight number (168)
        char title[256]; //!< Aircraft model string
    };

    //! Data struct of aircraft data (setable)
    struct DataDefinitionRemoteAtc
    {
        // length here is from SimConnect_AddToDataDefinition
        char atcId[32]; //!< ID used by ATC
        char atcAirline[64]; //!< Airline used by ATC
        char atcFlightNumber[8]; //!< Flight Number used by ATC

        //! @{
        //! Copy the strings, length from docu
        void copyAtcId(const char *c)
        {
            strncpy_s(atcId, c, 10);
            atcId[9] = 0;
        }
        void copyAtcAirline(const char *c)
        {
            strncpy_s(atcAirline, c, 50);
            atcAirline[49] = 0;
        }
        void copyFlightNumber(const char *c)
        {
            strncpy_s(atcFlightNumber, c, 6);
            atcFlightNumber[5] = 0;
        }
        //! @}

        //! Set default values
        void setDefaultValues()
        {
            std::fill(atcId, atcId + 10, static_cast<byte>(0));
            std::fill(atcAirline, atcAirline + 50, static_cast<byte>(0));
            std::fill(atcFlightNumber, atcFlightNumber + 6, static_cast<byte>(0));
        }
    };

    //! Data struct of remote aircraft parts
    struct FSXCOMMON_EXPORT DataDefinitionRemoteAircraftPartsWithoutLights
    {
        double flapsLeadingEdgeLeftPercent; //!< Leading edge  left in percent  0..1
        double flapsLeadingEdgeRightPercent; //!< Leading edge  right in percent 0..1
        double flapsTrailingEdgeLeftPercent; //!< Trailing edge left in percent  0..1
        double flapsTrailingEdgeRightPercent; //!< Trailing edge right in percent 0..1
        double gearHandlePosition; //!< Gear handle position
        double spoilersHandlePosition; //!< Spoilers out?
        double engine1Combustion; //!< Engine 1 combustion flag
        double engine2Combustion; //!< Engine 2 combustion flag
        double engine3Combustion; //!< Engine 3 combustion flag
        double engine4Combustion; //!< Engine 4 combustion flag

        //! Ctor
        DataDefinitionRemoteAircraftPartsWithoutLights();

        //! Ctor
        DataDefinitionRemoteAircraftPartsWithoutLights(const swift::misc::aviation::CAircraftParts &parts);

        //! Equal to other parts
        bool operator==(const DataDefinitionRemoteAircraftPartsWithoutLights &rhs) const;

        //! All engines on/off
        void setAllEngines(bool on);

        //! Set given engine
        void setEngine(int number1based, bool on);

        //! Reset all flaps
        void resetAllFlaps();

        //! Reset spoilers
        void resetSpoilers();

        //! Reset to invalid values
        void resetToInvalid();

        //! Init from parts
        void initFromParts(const swift::misc::aviation::CAircraftParts &parts);
    };

    //! Data for aircraft lighs
    struct FSXCOMMON_EXPORT DataDefinitionRemoteAircraftLights
    {
        double lightStrobe; //!< Is strobe light on?
        double lightLanding; //!< Is landing light on?
        double lightTaxi; //!< Is taxi light on?
        double lightBeacon; //!< Is beacon light on?
        double lightNav; //!< Is nav light on?
        double lightLogo; //!< Is logo light on?
        double lightRecognition; //!< Is recognition light on
        double lightCabin; //!< Is cabin light on

        //! Convert to lights
        swift::misc::aviation::CAircraftLights toLights() const;
    };

    //! Data for AI object and probe sent back from simulator
    struct DataDefinitionPosData
    {
        double latitudeDeg; //!< Latitude (deg)
        double longitudeDeg; //!< Longitude (deg)
        double altitudeFt; //!< Altitude (ft)
        double elevationFt; //!< Elevation (ft)
        double cgToGroundFt; //!< Static CG to ground (ft)

        //! Above ground ft
        double aboveGroundFt() const { return altitudeFt - elevationFt; }

        //! Above ground ft
        bool isOnGround() const { return this->aboveGroundFt() < 1.0; }
    };

    //! Data struct simulator environment
    struct DataDefinitionSimEnvironment
    {
        qint32 zuluTimeSeconds; //!< Simulator zulu (GMT) time in secs.
        qint32 zuluYear; //!< Simulator zulu (GMT) year.
        qint32 zuluMonth; //!< Simulator zulu (GMT) month.
        qint32 zuluDayOfMonth; //!< Simulator zulu (GMT) day of the month.
    };

    //! The whole SB data area
    //! \remark vPilot SB area https://forums.vatsim.net/viewtopic.php?p=519580
    //! \remark SB offsets http://www.squawkbox.ca/doc/sdk/fsuipc.php
    struct DataDefinitionClientAreaSb
    {
        byte data[128] {}; //!< 128 bytes of data, offsets

        //! Standby = 1, else 0
        byte getTransponderMode() const { return data[17]; }

        //! Ident = 1, else 0
        byte getIdent() const { return data[19]; }

        //! Ident?
        bool isIdent() const { return getIdent() != 0; }

        //! Standby
        bool isStandby() const { return getTransponderMode() != 0; }

        //! SB is running
        void setRunning(bool running) { data[0] = running ? 1 : 0; }

        //! Mark as connected with network
        void setConnected(bool connected) { data[1] = connected ? 1 : 0; }

        //! Set default values
        void setDefaultValues()
        {
            std::fill(data, data + 128, static_cast<byte>(0));
            data[0] = 1; // SB running, indicates the client is running as external app, 0..not running, 1..external app, 2..FS module
            data[1] = 0; // SB connected to FSD, 0..not connected, 1..connected
            data[17] = 1; // 1..standby, 0..mode C
            data[19] = 0; // no ident
        }

        //! Values
        QString toQString() const;
    };

    //! Client areas
    enum ClientAreaId
    {
        ClientAreaSquawkBox
    };

    //! Handles SimConnect data definitions
    class FSXCOMMON_EXPORT CSimConnectDefinitions
    {
    public:
        //! SimConnect definiton IDs
        enum DataDefiniton
        {
            DataOwnAircraft,
            DataOwnAircraftTitle,
            DataRemoteAircraftLights,
            DataRemoteAircraftPartsWithoutLights,
            DataRemoteAircraftSetPosition, //!< the position which will be set
            DataRemoteAircraftGetPosition, //!< get position to evaluate altitude / AGL
            DataRemoteAircraftModelData, //!< model data eventually used and reported back from simulator
            DataRemoteAircraftSetData, //!< set model data such as airline
            DataSimEnvironment,
            DataClientAreaSb, //!< whole SB area, see http://squawkbox.ca/doc/sdk/fsuipc.php
            DataClientAreaSbIdent, //!< SB ident single value 0x7b93/19
            DataClientAreaSbStandby, //!< SB standby 0x7b91/17
            DataClientAreaSbConnected, //!< SB connected with network 0x7b81/1
            DataClientAreaSbRunning //!< SB running 0x7b80/0
        };

        //! SimConnect request IDs
        enum Request
        {
            RequestOwnAircraft,
            RequestOwnAircraftTitle,
            RequestSimEnvironment,
            RequestSbData, //!< SB client area / XPDR mode
            RequestFacility,
            RequestEndMarker //!< free request ids can start here
        };

        //! SimObject requests used for AI aircraft and probes
        enum SimObjectRequest
        {
            SimObjectBaseId, //!< base id without specific request
            SimObjectAdd,
            SimObjectRemove,
            SimObjectPositionData,
            SimObjectLights,
            SimObjectModel,
            SimObjectMisc,
            SimObjectEndMarker //!< end marker, do NOT remove, also means invalid
        };

        //! Request to string
        static const QString &requestToString(Request request);

        //! Request to string
        static const QString &simObjectRequestToString(SimObjectRequest simObjectRequest);

        //! Constructor
        CSimConnectDefinitions();

        //! Initialize all data definitions
        static HRESULT initDataDefinitionsWhenConnected(const HANDLE hSimConnect);

    private:
        //! Initialize data definition for our own aircraft
        static HRESULT initOwnAircraft(const HANDLE hSimConnect);

        //! Initialize data definition for remote aircraft
        static HRESULT initRemoteAircraft(const HANDLE hSimConnect);

        //! Initialize data for setting remote aircraft airline etc.
        static HRESULT initRemoteAircraftSimData(const HANDLE hSimConnect);

        //! Initialize data for remote aircraft queried from simulator
        static HRESULT initRemoteAircraftSimDataSet(const HANDLE hSimConnect);

        //! Initialize data definition for Simulator environment
        static HRESULT initSimulatorEnvironment(const HANDLE hSimConnect);

        //! Initialize the SB data are
        static HRESULT initSbDataArea(const HANDLE hSimConnect);
    };
} // namespace

#endif // guard
