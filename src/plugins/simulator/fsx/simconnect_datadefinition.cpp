/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simconnect_datadefinition.h"
#include "blackmisc/logmessage.h"

using namespace BlackMisc;

namespace BlackSimPlugin
{
    namespace Fsx
    {

        CSimConnectDefinitions::CSimConnectDefinitions() {  }

        HRESULT CSimConnectDefinitions::initDataDefinitionsWhenConnected(const HANDLE hSimConnect)
        {
            HRESULT hr = S_OK;
            hr += initOwnAircraft(hSimConnect);
            hr += initRemoteAircraftSituation(hSimConnect);
            hr += initGearHandlePosition(hSimConnect);
            hr += initSimulatorEnvironment(hSimConnect);
            hr += initSbDataArea(hSimConnect);
            return hr;
        }

        HRESULT CSimConnectDefinitions::initOwnAircraft(const HANDLE hSimConnect)
        {
            HRESULT hr = S_OK;
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "Plane Latitude", "Degrees");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "Plane Longitude", "Degrees");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "Plane Altitude", "Feet");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "Plane Heading Degrees True", "Degrees");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "Plane Pitch Degrees", "Degrees");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "Plane Bank Degrees", "Degrees");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "GROUND VELOCITY", "knots");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "SIM ON GROUND", "bool");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "TRANSPONDER CODE:1", NULL);
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "COM ACTIVE FREQUENCY:1", "MHz");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "COM ACTIVE FREQUENCY:2", "MHz");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "COM STANDBY FREQUENCY:1", "MHz");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraft, "COM STANDBY FREQUENCY:2", "MHz");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataOwnAircraftTitle, "TITLE", NULL, SIMCONNECT_DATATYPE_STRING256);
            if (hr != S_OK)
            {
                CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr)).error("SimConnect error: initOwnAircraft %1") << hr;
            }
            return hr;
        }

        HRESULT CSimConnectDefinitions::initRemoteAircraftSituation(const HANDLE hSimConnect)
        {
            HRESULT hr = S_OK;
            hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataRemoteAircraftSituation, "Initial Position", "", SIMCONNECT_DATATYPE_INITPOSITION);
            if (hr != S_OK)
            {
                CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr)).error("SimConnect error: initRemoteAircraftSituation %1") << hr;
            }
            return hr;
        }

        HRESULT CSimConnectDefinitions::initGearHandlePosition(const HANDLE hSimConnect)
        {
            HRESULT hr = S_OK;
            hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataGearHandlePosition, "GEAR HANDLE POSITION", "BOOL", SIMCONNECT_DATATYPE_INT32);
            if (hr != S_OK)
            {
                CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr)).error("SimConnect error: initGearHandlePosition %1") << hr;
            }
            return hr;
        }

        HRESULT CSimConnectDefinitions::initSimulatorEnvironment(const HANDLE hSimConnect)
        {
            HRESULT hr = S_OK;
            hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataSimEnvironment, "ZULU TIME", "", SIMCONNECT_DATATYPE_INT32);
            hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDefinitions::DataSimEnvironment, "LOCAL TIME", "", SIMCONNECT_DATATYPE_INT32);
            if (hr != S_OK)
            {
                CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr)).error("SimConnect error: initSimulatorEnvironment %1") << hr;
            }
            return hr;
        }

        HRESULT CSimConnectDefinitions::initSbDataArea(const HANDLE hSimConnect)
        {
            HRESULT hr = S_OK;
            DWORD sbSize = sizeof(DataDefinitionClientAreaSb);

            // We need to know the client area 'name' and map it to a client ID
            hr += SimConnect_MapClientDataNameToID(hSimConnect, "SquawkBox Data", ClientAreaSquawkBox);
            if (hr != S_OK)
            {
                CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr)).error("SimConnect error: SimConnect_MapClientDataNameToID %1") << hr;
                return hr;
            }

            // Mapping needs to be first
            hr += SimConnect_CreateClientData(hSimConnect, ClientAreaSquawkBox, sbSize, SIMCONNECT_CREATE_CLIENT_DATA_FLAG_DEFAULT);
            if (hr != S_OK)
            {
                CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr)).error("SimConnect error: SimConnect_CreateClientData %1") << hr;
                return hr;
            }

            hr += SimConnect_AddToClientDataDefinition(hSimConnect, CSimConnectDefinitions::DataClientAreaSb, 0, sbSize);
            hr += SimConnect_AddToClientDataDefinition(hSimConnect, CSimConnectDefinitions::DataClientAreaSbStandby, 17, 1);
            hr += SimConnect_AddToClientDataDefinition(hSimConnect, CSimConnectDefinitions::DataClientAreaSbIdent, 19, 1);
            if (hr != S_OK)
            {
                CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr)).error("SimConnect error: SB data area data definitions %1") << hr;
                return hr;
            }

            // write a default client area so we are not suddenly squawking ident or so
            DataDefinitionClientAreaSb sbArea;
            sbArea.setDefaultValues();
            hr += SimConnect_SetClientData(hSimConnect, ClientAreaSquawkBox, CSimConnectDefinitions::DataClientAreaSb, NULL, 0, sbSize, &sbArea);
            if (hr != S_OK)
            {
                CLogMessage(static_cast<CSimConnectDefinitions *>(nullptr)).error("SimConnect error: SimConnect_SetClientData %1") << hr;
            }
            return hr;
        }

    } // namespace
} // namespace
