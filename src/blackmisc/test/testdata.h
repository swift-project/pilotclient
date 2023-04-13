/* Copyright (C) 2017
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_TEST_TESTDATA_H
#define BLACKMISC_TEST_TESTDATA_H

#include "blackmisc/blackmiscexport.h"

namespace BlackMisc
{
    class CVariantList;
    class CPropertyIndexVariantMap;

    namespace Aviation
    {
        class CAtcStation;
        class CAtcStationList;
        class CCallsign;
        class CCallsignSet;
        class CAircraftSituation;
        class CAircraftIcaoCode;
        class CAirlineIcaoCode;
        class CLivery;
        class CFlightPlan;
    }
    namespace Geo
    {
        class CCoordinateGeodetic;
    }
    namespace Network
    {
        class CUser;
        class CUserList;
        class CServer;
    }
    namespace Simulation
    {
        class CAircraftModel;
        class CSimulatedAircraft;
    }

    namespace Test
    {
        /*!
         * Testdata for unit tests/data
         * \remark only use forward declared types in CTestData
         */
        class BLACKMISC_EXPORT CTestData
        {
        public:
            //! Traffic server (dummy)
            static const BlackMisc::Network::CServer &getTrafficServer();

            //! Get a variant list with various data
            static const BlackMisc::CVariantList &getCVariantList();

            //! Get a variant map
            static const BlackMisc::CPropertyIndexVariantMap &getCPropertyIndexVariantMap();

            //! Munich tower coordinate
            static const BlackMisc::Geo::CCoordinateGeodetic &getCoordinateMunichTower();

            //! Frankfurt tower coordinate
            static const BlackMisc::Geo::CCoordinateGeodetic &getCoordinateFrankfurtTower();

            //! Munich tower
            static const BlackMisc::Aviation::CAtcStation &getMunichTower();

            //! Munich approach
            static const BlackMisc::Aviation::CAtcStation &getMunichApproach();

            //! Frankfurt tower
            static const BlackMisc::Aviation::CAtcStation &getFrankfurtTower();

            //! Tower stations
            static const BlackMisc::Aviation::CAtcStationList &getAtcStations();

            //! Get ATC station
            static const BlackMisc::Aviation::CAtcStation &getRandomAtcStation();

            //! Get aircraft callsigns
            static const BlackMisc::Aviation::CCallsignSet &getPilotCallsigns();

            //! Get a random callsign (aircraft)
            static const BlackMisc::Aviation::CCallsign &getRandomPilotCallsign();

            //! Get controller callsigns
            static const BlackMisc::Aviation::CCallsignSet &getControllerCallsigns();

            //! Get a random callsign (ATC)
            static const BlackMisc::Aviation::CCallsign &getRandomControllerCallsign();

            //! Get a random pilot
            static const BlackMisc::Network::CUserList &getPilots();

            //! Get a random pilot user
            static const BlackMisc::Network::CUser &getRandomPilot();

            //! Get a random controller
            static const BlackMisc::Network::CUserList &getControllers();

            //! Get a random controller user
            static const BlackMisc::Network::CUser &getRandomController();

            //! Aircraft situation (Munich)
            static const BlackMisc::Aviation::CAircraftSituation &getAircraftSituationAboveMunichTower();

            //! Aircraft situation (Frankfurt)
            static const BlackMisc::Aviation::CAircraftSituation &getAircraftSituationAboveFrankfurtTower();

            //! B737 ICAO code (as from DB)
            static const BlackMisc::Aviation::CAircraftIcaoCode &getDBAircraftIcaoB737();

            //! C172 ICAO code (as from DB)
            static const BlackMisc::Aviation::CAircraftIcaoCode &getDBAircraftIcaoC172();

            //! DLH ICAO code (as from DB)
            static const BlackMisc::Aviation::CAirlineIcaoCode &getDbAirlineIcaoDLH();

            //! BAW ICAO code (as from DB)
            static const BlackMisc::Aviation::CAirlineIcaoCode &getDbAirlineIcaoBAW();

            //! Aircraft model
            static const BlackMisc::Aviation::CLivery &getDbLiveryDLHStarAlliance();

            //! FSX aircraft model, a C172 Skyhawk
            static const BlackMisc::Simulation::CAircraftModel &getDbAircraftModelFsxA2AC172Skyhawk();

            //! FSX aircraft model, an Airbus A320
            static const BlackMisc::Simulation::CAircraftModel &getDbAircraftModelFsxAerosoftA320();

            //! C172 simulated aircraft, a C172
            static const BlackMisc::Simulation::CSimulatedAircraft &getC172Aircraft();

            //! A320 simulated aircraft, a A
            static const BlackMisc::Simulation::CSimulatedAircraft &getA320Aircraft();

            //! Prefilled flightplan from EDDM-EDDF
            static const BlackMisc::Aviation::CFlightPlan &getFlightPlan();
        };
    } // ns
} // ns

#endif // guard
