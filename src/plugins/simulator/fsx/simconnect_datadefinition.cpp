/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "simconnect_datadefinition.h"

namespace BlackSimPlugin
{
    namespace Fsx
    {

        CSimConnectDataDefinition::CSimConnectDataDefinition() {  }

        HRESULT CSimConnectDataDefinition::initDataDefinitions(const HANDLE hSimConnect)
        {
            HRESULT hr = S_OK;
            hr += initOwnAircraft(hSimConnect);
            hr += initRemoteAircraftSituation(hSimConnect);
            hr += initGearHandlePosition(hSimConnect);
            hr += initSimulatorEnvironment(hSimConnect);
            return hr;
        }

        HRESULT CSimConnectDataDefinition::initOwnAircraft(const HANDLE hSimConnect)
        {
            HRESULT hr = S_OK;
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "Plane Latitude", "Degrees");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "Plane Longitude", "Degrees");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "Plane Altitude", "Feet");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "Plane Heading Degrees True", "Degrees");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "Plane Pitch Degrees", "Degrees");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "Plane Bank Degrees", "Degrees");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "GROUND VELOCITY", "knots");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "SIM ON GROUND", "bool");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "TRANSPONDER CODE:1", NULL);
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "COM ACTIVE FREQUENCY:1", "MHz");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "COM ACTIVE FREQUENCY:2", "MHz");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "COM STANDBY FREQUENCY:1", "MHz");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "COM STANDBY FREQUENCY:2", "MHz");
            hr +=  SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraftTitle, "TITLE", NULL, SIMCONNECT_DATATYPE_STRING256);
            if (hr != S_OK) {
                qFatal("initOwnAircraft");
            }
            return hr;
        }

        HRESULT CSimConnectDataDefinition::initRemoteAircraftSituation(const HANDLE hSimConnect)
        {
            HRESULT hr = S_OK;
            hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataRemoteAircraftSituation, "Initial Position", "", SIMCONNECT_DATATYPE_INITPOSITION);
            if (hr != S_OK) {
                qFatal("initRemoteAircraftSituation");
            }
            return hr;
        }

        HRESULT CSimConnectDataDefinition::initGearHandlePosition(const HANDLE hSimConnect)
        {
            HRESULT hr = S_OK;
            hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataGearHandlePosition, "GEAR HANDLE POSITION", "BOOL", SIMCONNECT_DATATYPE_INT32);
            if (hr != S_OK) {
                qFatal("initGearHandlePosition");
            }
            return hr;
        }

        HRESULT CSimConnectDataDefinition::initSimulatorEnvironment(const HANDLE hSimConnect)
        {
            HRESULT hr = S_OK;
            hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataSimEnvironment, "ZULU TIME", "", SIMCONNECT_DATATYPE_INT32);
            hr += SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataSimEnvironment, "LOCAL TIME", "", SIMCONNECT_DATATYPE_INT32);
            if (hr != S_OK) {
                qFatal("initSimulatorEnvironment");
            }
            return hr;
        }
    }
}
