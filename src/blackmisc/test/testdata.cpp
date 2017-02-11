/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testdata.h"
#include "blackmisc/aviation/atcstationlist.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Test
    {
        const CCoordinateGeodetic &CTestData::coordinateMunichTower()
        {
            static const CCoordinateGeodetic c = CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", { 1487, CLengthUnit::ft() });
            return c;
        }

        const CCoordinateGeodetic &CTestData::coordinateFrankfurtTower()
        {
            static const CCoordinateGeodetic c = CCoordinateGeodetic::fromWgs84("50° 1' 18.38″ N", "8° 33' 23.24″ E", { 355, CLengthUnit::ft() });
            return c;
        }

        const CAtcStationList &CTestData::getTowerStations()
        {
            static const CAtcStationList stations(
            {
                CAtcStation(CCallsign("EDDF_TWR"), CUser("654321", "Joe Bar"),
                CFrequency(118.7, CFrequencyUnit::MHz()), CTestData::coordinateMunichTower(), CLength(50, CLengthUnit::km())),
                CAtcStation(CCallsign("EDDM_TWR"), CUser("654321", "John Doe"),
                CFrequency(119.9, CFrequencyUnit::MHz()), CTestData::coordinateFrankfurtTower(), CLength(50, CLengthUnit::km()))
            });
            return stations;
        }

        const CAtcStation &CTestData::getAtcStation()
        {
            return getTowerStations()[0];
        }

        const CCallsign &CTestData::getRandomAtcCallsign()
        {
            static const QList<CCallsign> callsigns(
            {
                CCallsign("EDDM_TWR"), CCallsign("EDDM_APP"), CCallsign("EDDM_GND"),
                CCallsign("EDDF_TWR"), CCallsign("EDDF_APP"),
                CCallsign("EDDF_GND")
            });
            int i = (rand() % (callsigns.size()));
            return callsigns[i];
        }

        const CCallsign &CTestData::getRandomAircraftCallsign()
        {
            static const QList<CCallsign> callsigns(
            {
                CCallsign("DEMBZ"), CCallsign("DLH123"), CCallsign("RYR635L"),
                CCallsign("LGL974"), CCallsign("AUI129"),
                CCallsign("CLX756")
            });
            int i = (rand() % (callsigns.size()));
            return callsigns[i];
        }
    } // ns
} // ns
