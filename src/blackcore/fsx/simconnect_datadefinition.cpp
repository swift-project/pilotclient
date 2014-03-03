/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "simconnect_datadefinition.h"
#include "simconnect/SimConnect.h"

namespace BlackCore
{
    namespace FSX
    {

        CSimConnectDataDefinition::CSimConnectDataDefinition()
        {
        }

        HRESULT CSimConnectDataDefinition::initDataDefinitions(const HANDLE hSimConnect)
        {
            HRESULT hr = S_OK;
            hr = initOwnAircraft(hSimConnect);
            return hr;
        }

        HRESULT CSimConnectDataDefinition::initOwnAircraft(const HANDLE hSimConnect)
        {
            HRESULT hr = S_OK;
            hr = SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "Plane Latitude", "Degrees");
            hr = SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "Plane Longitude", "Degrees");
            hr = SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "Plane Altitude", "Feet");
            hr = SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "Plane Heading Degrees True", "Degrees");
            hr = SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "Plane Pitch Degrees", "Degrees");
            hr = SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "Plane Bank Degrees", "Degrees");
            hr = SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "GROUND VELOCITY", "knots");
            hr = SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "SIM ON GROUND", "bool");
            hr = SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "TRANSPONDER CODE:1", NULL);
            hr = SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "COM ACTIVE FREQUENCY:1", "MHz");
            hr = SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "COM ACTIVE FREQUENCY:2", "MHz");
            hr = SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "COM STANDBY FREQUENCY:1", "MHz");
            hr = SimConnect_AddToDataDefinition(hSimConnect, CSimConnectDataDefinition::DataOwnAircraft, "COM STANDBY FREQUENCY:2", "MHz");

            return hr;
        }
    }
}
