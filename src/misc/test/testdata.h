// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_TEST_TESTDATA_H
#define SWIFT_MISC_TEST_TESTDATA_H

#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    class CVariantList;
    class CPropertyIndexVariantMap;

    namespace aviation
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
    namespace geo
    {
        class CCoordinateGeodetic;
    }
    namespace network
    {
        class CUser;
        class CUserList;
        class CServer;
    }
    namespace simulation
    {
        class CAircraftModel;
        class CSimulatedAircraft;
    }

    namespace test
    {
        /*!
         * Testdata for unit tests/data
         * \remark only use forward declared types in CTestData
         */
        class SWIFT_MISC_EXPORT CTestData
        {
        public:
            //! Traffic server (dummy)
            static const swift::misc::network::CServer &getTrafficServer();

            //! Get a variant list with various data
            static const swift::misc::CVariantList &getCVariantList();

            //! Get a variant map
            static const swift::misc::CPropertyIndexVariantMap &getCPropertyIndexVariantMap();

            //! Munich tower coordinate
            static const swift::misc::geo::CCoordinateGeodetic &getCoordinateMunichTower();

            //! Frankfurt tower coordinate
            static const swift::misc::geo::CCoordinateGeodetic &getCoordinateFrankfurtTower();

            //! Munich tower
            static const swift::misc::aviation::CAtcStation &getMunichTower();

            //! Munich approach
            static const swift::misc::aviation::CAtcStation &getMunichApproach();

            //! Frankfurt tower
            static const swift::misc::aviation::CAtcStation &getFrankfurtTower();

            //! Tower stations
            static const swift::misc::aviation::CAtcStationList &getAtcStations();

            //! Get ATC station
            static const swift::misc::aviation::CAtcStation &getRandomAtcStation();

            //! Get aircraft callsigns
            static const swift::misc::aviation::CCallsignSet &getPilotCallsigns();

            //! Get a random callsign (aircraft)
            static const swift::misc::aviation::CCallsign &getRandomPilotCallsign();

            //! Get controller callsigns
            static const swift::misc::aviation::CCallsignSet &getControllerCallsigns();

            //! Get a random callsign (ATC)
            static const swift::misc::aviation::CCallsign &getRandomControllerCallsign();

            //! Get a random pilot
            static const swift::misc::network::CUserList &getPilots();

            //! Get a random pilot user
            static const swift::misc::network::CUser &getRandomPilot();

            //! Get a random controller
            static const swift::misc::network::CUserList &getControllers();

            //! Get a random controller user
            static const swift::misc::network::CUser &getRandomController();

            //! Aircraft situation (Munich)
            static const swift::misc::aviation::CAircraftSituation &getAircraftSituationAboveMunichTower();

            //! Aircraft situation (Frankfurt)
            static const swift::misc::aviation::CAircraftSituation &getAircraftSituationAboveFrankfurtTower();

            //! B737 ICAO code (as from DB)
            static const swift::misc::aviation::CAircraftIcaoCode &getDBAircraftIcaoB737();

            //! C172 ICAO code (as from DB)
            static const swift::misc::aviation::CAircraftIcaoCode &getDBAircraftIcaoC172();

            //! DLH ICAO code (as from DB)
            static const swift::misc::aviation::CAirlineIcaoCode &getDbAirlineIcaoDLH();

            //! BAW ICAO code (as from DB)
            static const swift::misc::aviation::CAirlineIcaoCode &getDbAirlineIcaoBAW();

            //! Aircraft model
            static const swift::misc::aviation::CLivery &getDbLiveryDLHStarAlliance();

            //! FSX aircraft model, a C172 Skyhawk
            static const swift::misc::simulation::CAircraftModel &getDbAircraftModelFsxA2AC172Skyhawk();

            //! FSX aircraft model, an Airbus A320
            static const swift::misc::simulation::CAircraftModel &getDbAircraftModelFsxAerosoftA320();

            //! C172 simulated aircraft, a C172
            static const swift::misc::simulation::CSimulatedAircraft &getC172Aircraft();

            //! A320 simulated aircraft, a A
            static const swift::misc::simulation::CSimulatedAircraft &getA320Aircraft();

            //! Prefilled flightplan from EDDM-EDDF
            static const swift::misc::aviation::CFlightPlan &getFlightPlan();
        };
    } // ns
} // ns

#endif
