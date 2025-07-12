// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simconnectdatadefinition.h"

#include <tuple>

#include <QStringBuilder>

#include "../fscommon/simulatorfscommonfunctions.h"
#include "simconnectsymbols.h"

#include "misc/aviation/aircraftenginelist.h"
#include "misc/aviation/aircraftparts.h"
#include "misc/logmessage.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;
using namespace swift::simplugin::fscommon;

namespace swift::simplugin::fsxcommon
{
    const QString &CSimConnectDefinitions::requestToString(Request request)
    {
        static const QString ownAircraft("RequestOwnAircraft");
        static const QString title("RequestOwnAircraftTitle");
        static const QString sbData("RequestSbData");
        static const QString facility("RequestFacility");
        static const QString end("<end>");
        static const QString unknown("unknown");

        switch (request)
        {
        case RequestOwnAircraft: return ownAircraft;
        case RequestOwnAircraftTitle: return title;
        case RequestSbData: return sbData;
        case RequestFacility: return facility;
        case RequestEndMarker: return end;
        default: break;
        }
        return unknown;
    }

    const QString &CSimConnectDefinitions::simObjectRequestToString(SimObjectRequest simObjectRequest)
    {
        static const QString baseId("base id");
        static const QString add("add");
        static const QString remove("remove");
        static const QString lights("lights");
        static const QString pos("position");
        static const QString model("model");
        static const QString misc("misc");
        static const QString end("<end>");
        static const QString unknown("unknown");

        switch (simObjectRequest)
        {
        case SimObjectBaseId: return baseId;
        case SimObjectAdd: return add;
        case SimObjectRemove: return remove;
        case SimObjectLights: return lights;
        case SimObjectPositionData: return pos;
        case SimObjectModel: return model;
        case SimObjectMisc: return misc;
        case SimObjectEndMarker: return end;
        default: break;
        }
        return unknown;
    }

    CSimConnectDefinitions::CSimConnectDefinitions() {}

    HRESULT CSimConnectDefinitions::initDataDefinitionsWhenConnected(const HANDLE hSimConnect,
                                                                     const CSimulatorInfo &simInfo)
    {
        HRESULT hr = s_ok();
        hr += initOwnAircraft(hSimConnect);
        hr += initRemoteAircraft(hSimConnect);
        hr += initRemoteAircraftSimData(hSimConnect);
        hr += initRemoteAircraftSimDataSet(hSimConnect);
        hr += initSimulatorEnvironment(hSimConnect);
        hr += initSbDataArea(hSimConnect);
        if (simInfo.isMSFS() || simInfo.isMSFS2024()) { hr += initMSFSTransponder(hSimConnect); }
        return hr;
    }

    HRESULT CSimConnectDefinitions::initOwnAircraft(const HANDLE hSimConnect)
    {
        // FSX vars: https://docs.microsoft.com/en-us/previous-versions/microsoft-esp/cc526981(v%3dmsdn.10)
        // all the VARS here should be FSX/P3D
        HRESULT hr = s_ok();
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "PLANE LATITUDE",
                                             "Degrees");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "PLANE LONGITUDE",
                                             "Degrees");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "PLANE ALTITUDE",
                                             "Feet");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "PLANE ALT ABOVE GROUND", "Feet");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "PRESSURE ALTITUDE",
                                             "Meters");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "STATIC CG TO GROUND", "Feet");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "PLANE HEADING DEGREES TRUE", "Degrees");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "PLANE PITCH DEGREES", "Degrees");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "PLANE BANK DEGREES",
                                             "Degrees");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "GROUND VELOCITY",
                                             "Knots");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "GROUND ALTITUDE",
                                             "Feet");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "SIM ON GROUND",
                                             "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "LIGHT STROBE",
                                             "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "LIGHT LANDING",
                                             "Bool");
        hr +=
            SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "LIGHT TAXI", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "LIGHT BEACON",
                                             "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "LIGHT NAV", "Bool");
        hr +=
            SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "LIGHT LOGO", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "TRANSPONDER CODE:1",
                                             nullptr);
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "COM ACTIVE FREQUENCY:1", "MHz");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "COM ACTIVE FREQUENCY:2", "MHz");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "COM STANDBY FREQUENCY:1", "MHz");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "COM STANDBY FREQUENCY:2", "MHz");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "COM TRANSMIT:1",
                                             "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "COM TRANSMIT:2",
                                             "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "COM RECEIVE ALL",
                                             "Bool");
        hr +=
            SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "COM TEST:1", "Bool");
        hr +=
            SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "COM TEST:2", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "COM STATUS:1",
                                             "Enum");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "COM STATUS:2",
                                             "Enum");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "FLAPS HANDLE PERCENT", "Percent Over 100");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "SPOILERS HANDLE POSITION", "Percent Over 100");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "GEAR HANDLE POSITION", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "NUMBER OF ENGINES",
                                             "Number");
        // Simconnect supports index 1 - 4
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "GENERAL ENG COMBUSTION:1", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "GENERAL ENG COMBUSTION:2", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "GENERAL ENG COMBUSTION:3", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "GENERAL ENG COMBUSTION:4", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "VELOCITY WORLD X",
                                             "Feet per second");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "VELOCITY WORLD Y",
                                             "Feet per second");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "VELOCITY WORLD Z",
                                             "Feet per second");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "ROTATION VELOCITY BODY X", "Radians per second");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "ROTATION VELOCITY BODY Y", "Radians per second");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "ROTATION VELOCITY BODY Z", "Radians per second");
        // MSFS 2020/2024 only
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "INDICATED ALTITUDE CALIBRATED", "Feet");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "TRANSPONDER STATE:1", "Enum");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft,
                                             "TRANSPONDER IDENT:1", "Bool");

        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraftTitle, "TITLE",
                                             nullptr, SIMCONNECT_DATATYPE_STRING256);
        if (isFailure(hr))
        {
            CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr)).error(u"SimConnect error: initOwnAircraft %1")
                << hr;
        }
        return hr;
    }

    HRESULT CSimConnectDefinitions::initRemoteAircraft(const HANDLE hSimConnect)
    {
        HRESULT hr = s_ok();
        // Position
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftSetPosition,
                                             "Initial Position", nullptr, SIMCONNECT_DATATYPE_INITPOSITION);

        // Hint: "Bool" and "Percent .." are units name
        // default data type is SIMCONNECT_DATATYPE_FLOAT64 -> double

        // Flaps
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftPartsWithoutLights,
                                             "LEADING EDGE FLAPS LEFT PERCENT", "Percent Over 100");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftPartsWithoutLights,
                                             "LEADING EDGE FLAPS RIGHT PERCENT", "Percent Over 100");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftPartsWithoutLights,
                                             "TRAILING EDGE FLAPS LEFT PERCENT", "Percent Over 100");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftPartsWithoutLights,
                                             "TRAILING EDGE FLAPS RIGHT PERCENT", "Percent Over 100");

        // Gear & Spoiler
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftPartsWithoutLights,
                                             "GEAR HANDLE POSITION", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftPartsWithoutLights,
                                             "SPOILERS HANDLE POSITION", "Percent Over 100");

        // Engines
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftPartsWithoutLights,
                                             "GENERAL ENG COMBUSTION:1", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftPartsWithoutLights,
                                             "GENERAL ENG COMBUSTION:2", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftPartsWithoutLights,
                                             "GENERAL ENG COMBUSTION:3", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftPartsWithoutLights,
                                             "GENERAL ENG COMBUSTION:4", "Bool");

        // Lights (other definition)
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraftTitle, "TITLE",
                                             nullptr, SIMCONNECT_DATATYPE_STRING256);

        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftLights,
                                             "LIGHT STROBE", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftLights,
                                             "LIGHT LANDING", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftLights,
                                             "LIGHT TAXI", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftLights,
                                             "LIGHT BEACON", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftLights, "LIGHT NAV",
                                             "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftLights,
                                             "LIGHT LOGO", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftLights, "LIGHT NAV",
                                             "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftLights,
                                             "LIGHT RECOGNITION", "Bool");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftLights,
                                             "LIGHT CABIN", "Bool");

        if (isFailure(hr))
        {
            CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr))
                    .error(u"SimConnect error: initRemoteAircraftSituation %1")
                << hr;
        }
        return hr;
    }

    HRESULT CSimConnectDefinitions::initRemoteAircraftSimData(const HANDLE hSimConnect)
    {
        HRESULT hr = s_ok();
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftGetPosition,
                                             "PLANE LATITUDE", "degrees");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftGetPosition,
                                             "PLANE LONGITUDE", "degrees");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftGetPosition,
                                             "PLANE ALTITUDE", "Feet");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftGetPosition,
                                             "GROUND ALTITUDE", "Feet");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftGetPosition,
                                             "STATIC CG TO GROUND", "Feet");
        if (isFailure(hr))
        {
            CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr))
                    .error(u"SimConnect error: initRemoteAircraftSimData DataRemoteAircraftGetPosition %1")
                << hr;
        }

        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftModelData,
                                             "STATIC CG TO GROUND", "Feet");
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftModelData,
                                             "ATC TYPE", nullptr, SIMCONNECT_DATATYPE_STRING32);
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftModelData,
                                             "ATC MODEL", nullptr, SIMCONNECT_DATATYPE_STRING32);
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftModelData, "ATC ID",
                                             nullptr, SIMCONNECT_DATATYPE_STRING32);
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftModelData,
                                             "ATC AIRLINE", nullptr, SIMCONNECT_DATATYPE_STRING64);
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftModelData,
                                             "ATC FLIGHT NUMBER", nullptr, SIMCONNECT_DATATYPE_STRING8);
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftModelData, "TITLE",
                                             nullptr, SIMCONNECT_DATATYPE_STRING256);
        if (isFailure(hr))
        {
            CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr))
                    .error(u"SimConnect error: initRemoteAircraftSimData DataRemoteAircraftModelData %1")
                << hr;
        }
        return hr;
    }

    HRESULT CSimConnectDefinitions::initRemoteAircraftSimDataSet(const HANDLE hSimConnect)
    {
        HRESULT hr = s_ok();

        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftSetData, "ATC ID",
                                             nullptr, SIMCONNECT_DATATYPE_STRING32);
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftSetData,
                                             "ATC AIRLINE", nullptr, SIMCONNECT_DATATYPE_STRING64);
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftSetData,
                                             "ATC FLIGHT NUMBER", nullptr, SIMCONNECT_DATATYPE_STRING8);
        if (isFailure(hr))
        {
            CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr))
                    .error(u"SimConnect error: initRemoteAircraftSimDataSet DataRemoteAircraftModelData %1")
                << hr;
        }
        return hr;
    }

    HRESULT CSimConnectDefinitions::initSimulatorEnvironment(const HANDLE hSimConnect)
    {
        HRESULT hr = s_ok();
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataSimEnvironment, "ZULU TIME", "",
                                             SIMCONNECT_DATATYPE_INT32);
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataSimEnvironment, "ZULU YEAR", "",
                                             SIMCONNECT_DATATYPE_INT32);
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataSimEnvironment,
                                             "ZULU MONTH OF YEAR", "", SIMCONNECT_DATATYPE_INT32);
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataSimEnvironment,
                                             "ZULU DAY OF MONTH", "", SIMCONNECT_DATATYPE_INT32);
        if (isFailure(hr))
        {
            CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr))
                    .error(u"SimConnect error: initSimulatorEnvironment %1")
                << hr;
        }
        return hr;
    }

    HRESULT CSimConnectDefinitions::initSbDataArea(const HANDLE hSimConnect)
    {
        HRESULT hr = s_ok();
        const DWORD sbSize = sizeof(DataDefinitionClientAreaSb);

        // We need to know the client area 'name' and map it to a client ID
        hr += SimConnect_MapClientDataNameToID(hSimConnect, "SquawkBox Data", ClientAreaSquawkBox);
        if (isFailure(hr))
        {
            CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr))
                    .error(u"SimConnect error: SimConnect_MapClientDataNameToID %1")
                << hr;
            return hr;
        }

        // Mapping needs to be first
        hr += SimConnect_CreateClientData(hSimConnect, ClientAreaSquawkBox, sbSize,
                                          SIMCONNECT_CREATE_CLIENT_DATA_FLAG_DEFAULT);
        if (isFailure(hr))
        {
            CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr))
                    .error(u"SimConnect error: SimConnect_CreateClientData %1")
                << hr;
            return hr;
        }

        //  data definitions
        hr += SimConnect_AddToClientDataDefinition(hSimConnect, CSimConnectDefinitions::DataClientAreaSb, 0,
                                                   sbSize); // whole area
        hr += SimConnect_AddToClientDataDefinition(hSimConnect, CSimConnectDefinitions::DataClientAreaSbStandby, 17,
                                                   1); // standby
        hr += SimConnect_AddToClientDataDefinition(hSimConnect, CSimConnectDefinitions::DataClientAreaSbIdent, 19,
                                                   1); // ident
        hr += SimConnect_AddToClientDataDefinition(hSimConnect, CSimConnectDefinitions::DataClientAreaSbConnected, 1,
                                                   1); // network connected
        hr += SimConnect_AddToClientDataDefinition(hSimConnect, CSimConnectDefinitions::DataClientAreaSbRunning, 0,
                                                   1); // SB running

        if (isFailure(hr))
        {
            CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr))
                    .error(u"SimConnect error: SB data area data definitions %1")
                << hr;
            return hr;
        }

        // write a default client area so we are not suddenly squawking ident or so
        DataDefinitionClientAreaSb sbArea;
        byte sbRunning = 1;
        sbArea.setDefaultValues();
        hr += SimConnect_SetClientData(hSimConnect, ClientAreaSquawkBox, CSimConnectDefinitions::DataClientAreaSb,
                                       SIMCONNECT_CLIENT_DATA_SET_FLAG_DEFAULT, 0, sbSize, &sbArea);
        hr +=
            SimConnect_SetClientData(hSimConnect, ClientAreaSquawkBox, CSimConnectDefinitions::DataClientAreaSbRunning,
                                     SIMCONNECT_CLIENT_DATA_SET_FLAG_DEFAULT, 0, 1, &sbRunning);
        if (isFailure(hr))
        {
            CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr))
                    .error(u"SimConnect error: SimConnect_SetClientData %1")
                << hr;
        }
        return hr;
    }

    HRESULT CSimConnectDefinitions::initMSFSTransponder(const HANDLE hSimConnect)
    {
        HRESULT hr = s_ok();
        hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataTransponderModeMSFS,
                                             "TRANSPONDER STATE:1", "Enum");

        if (isFailure(hr))
        {
            CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr))
                    .error(u"SimConnect error: MSFS transponder data definitions %1")
                << hr;
        }
        return hr;
    }

    DataDefinitionRemoteAircraftPartsWithoutLights::DataDefinitionRemoteAircraftPartsWithoutLights()
    {
        this->resetToInvalid();
    }

    DataDefinitionRemoteAircraftPartsWithoutLights::DataDefinitionRemoteAircraftPartsWithoutLights(
        const CAircraftParts &parts)
    {
        this->initFromParts(parts);
    }

    bool DataDefinitionRemoteAircraftPartsWithoutLights::operator==(
        const DataDefinitionRemoteAircraftPartsWithoutLights &rhs) const
    {
        return std::tie(flapsLeadingEdgeLeftPercent, flapsLeadingEdgeRightPercent, flapsTrailingEdgeLeftPercent,
                        flapsTrailingEdgeRightPercent, gearHandlePosition, spoilersHandlePosition, engine1Combustion,
                        engine2Combustion, engine3Combustion, engine4Combustion) ==
               std::tie(rhs.flapsLeadingEdgeLeftPercent, rhs.flapsLeadingEdgeRightPercent,
                        rhs.flapsTrailingEdgeLeftPercent, rhs.flapsTrailingEdgeRightPercent, rhs.gearHandlePosition,
                        rhs.spoilersHandlePosition, rhs.engine1Combustion, rhs.engine2Combustion, rhs.engine3Combustion,
                        rhs.engine4Combustion);
    }

    void DataDefinitionRemoteAircraftPartsWithoutLights::setAllEngines(bool on)
    {
        engine1Combustion = on ? 1 : 0;
        engine2Combustion = on ? 1 : 0;
        engine3Combustion = on ? 1 : 0;
        engine4Combustion = on ? 1 : 0;
    }

    void DataDefinitionRemoteAircraftPartsWithoutLights::setEngine(int number1based, bool on)
    {
        double v = on ? 1.0 : 0.0;
        switch (number1based)
        {
        case 1: engine1Combustion = v; break;
        case 2: engine2Combustion = v; break;
        case 3: engine3Combustion = v; break;
        case 4: engine4Combustion = v; break;
        default: break;
        }
    }

    void DataDefinitionRemoteAircraftPartsWithoutLights::resetAllFlaps()
    {
        flapsLeadingEdgeLeftPercent = 0.0;
        flapsLeadingEdgeRightPercent = 0.0;
        flapsTrailingEdgeLeftPercent = 0.0;
        flapsTrailingEdgeRightPercent = 0.0;
    }

    void DataDefinitionRemoteAircraftPartsWithoutLights::resetSpoilers() { spoilersHandlePosition = 0.0; }

    void DataDefinitionRemoteAircraftPartsWithoutLights::resetToInvalid()
    {
        flapsLeadingEdgeLeftPercent = -1;
        flapsLeadingEdgeRightPercent = -1;
        flapsTrailingEdgeLeftPercent = -1;
        flapsTrailingEdgeRightPercent = -1;
        gearHandlePosition = -1;
        spoilersHandlePosition = -1;
        engine1Combustion = -1;
        engine2Combustion = -1;
        engine3Combustion = -1;
        engine4Combustion = -1;
    }

    void DataDefinitionRemoteAircraftPartsWithoutLights::initFromParts(const CAircraftParts &parts)
    {
        gearHandlePosition = parts.isFixedGearDown() ? 1.0 : 0.0;
        const double trail = parts.getFlapsPercent() / 100.0;
        const double lead = trail;
        flapsTrailingEdgeLeftPercent = trail;
        flapsTrailingEdgeRightPercent = trail;
        flapsLeadingEdgeLeftPercent = lead;
        flapsLeadingEdgeRightPercent = lead;
        spoilersHandlePosition = parts.isSpoilersOut() ? 1.0 : 0.0;
        this->setAllEngines(false); // init

        int e = 1;
        for (const CAircraftEngine &engine : parts.getEngines()) { this->setEngine(e++, engine.isOn()); }
    }

    CAircraftLights DataDefinitionRemoteAircraftLights::toLights() const
    {
        return CAircraftLights(dtb(lightStrobe), dtb(lightLanding), dtb(lightTaxi), dtb(lightBeacon), dtb(lightNav),
                               dtb(lightLogo), dtb(lightRecognition), dtb(lightCabin));
    }

    QString DataDefinitionClientAreaSb::toQString() const
    {
        return u"0 (running): " % QString::number(data[0]) % u" 1 (connected): " % QString::number(data[1]) %
               u" 17 (standby): " % QString::number(data[17]) % u" 19 (ident): " % QString::number(data[19]);
    }

} // namespace swift::simplugin::fsxcommon
