/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testdata.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/network/server.h"
#include "blackmisc/variantlist.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackMisc
{
    namespace Test
    {
        int randomIndex(int size)
        {
            return qrand() % size;
        }

        const CServer &CTestData::getTrafficServer()
        {
            static const CServer trafficServer("fooserver", "a foo server", "localhost", 1234,
                                               CUser("112233", "Some real name", "email@xyz.com", "secret"));
            return trafficServer;
        }

        const CVariantList &CTestData::getCVariantList()
        {
            static const CVariantList vl(
            {
                CVariant::from(CLength(100, CLengthUnit::m())),
                CVariant::from(CSpeed(200, CSpeedUnit::m_s())),
                CVariant::from(CTestData::getCoordinateMunichTower())
            });
            return vl;
        }

        const CPropertyIndexVariantMap getCPropertyIndexVariantMapImpl()
        {
            CPropertyIndexVariantMap valueMap;
            valueMap.addValue(1000, 111.222);
            valueMap.addValue(2000, CTestData::getRandomControllerCallsign());
            valueMap.addValue(3000, CTestData::getCoordinateFrankfurtTower());
            valueMap.addValue(4000, QDateTime::currentDateTime().addDays(1));
            valueMap.addValue(5000, QString("foobar"));
            return valueMap;
        }

        const CPropertyIndexVariantMap &CTestData::getCPropertyIndexVariantMap()
        {
            static const CPropertyIndexVariantMap valueMap(getCPropertyIndexVariantMapImpl());
            return valueMap;
        }

        const CCoordinateGeodetic &CTestData::getCoordinateMunichTower()
        {
            static const CCoordinateGeodetic c = CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", { 1487, CLengthUnit::ft() });
            return c;
        }

        const CCoordinateGeodetic &CTestData::getCoordinateFrankfurtTower()
        {
            static const CCoordinateGeodetic c = CCoordinateGeodetic::fromWgs84("50° 1' 18.38″ N", "8° 33' 23.24″ E", { 355, CLengthUnit::ft() });
            return c;
        }

        const CFlightPlan &CTestData::getFlightPlan()
        {
            static const CFlightPlan fp("T/A320/F", "EDDF", "EDDM", "EDDN",
                                        QDateTime::currentDateTimeUtc(), QDateTime::currentDateTime().addSecs(600),
                                        CTime(1.0, CTimeUnit::h()), CTime(2.0, CTimeUnit::h()),
                                        CAltitude(10000, CAltitude::MeanSeaLevel, CLengthUnit::ft()), CSpeed(400, CSpeedUnit::kts()), CFlightPlan::IFR,
                                        "DKB T104 ANORA", "Testing flight");
            return fp;
        }

        const CAtcStation &CTestData::getMunichTower()
        {
            static const CAtcStation t(CCallsign("EDDM_TWR"), CUser("654321", "John Doe"),
                                       CFrequency(119.9, CFrequencyUnit::MHz()), CTestData::getCoordinateFrankfurtTower(), CLength(50, CLengthUnit::km()),
                                       false, QDateTime::currentDateTimeUtc().addSecs(7200), QDateTime::currentDateTimeUtc().addSecs(9500));
            return t;
        }

        const CAtcStation &CTestData::getFrankfurtTower()
        {
            static const CAtcStation t(CCallsign("EDDF_TWR"), CUser("654321", "Joe Bar"),
                                       CFrequency(118.7, CFrequencyUnit::MHz()), CTestData::getCoordinateMunichTower(), CLength(50, CLengthUnit::km()),
                                       true, QDateTime::currentDateTimeUtc().addSecs(-3600), QDateTime::currentDateTimeUtc().addSecs(2400));
            return t;
        }

        const CAtcStation &CTestData::getMunichApproach()
        {
            static const CAtcStation t(CCallsign("eddm_app"), CUser("654321", "Jen Doe"),
                                       CFrequency(120.7, CFrequencyUnit::MHz()),
                                       CTestData::getCoordinateMunichTower(), CLength(100, CLengthUnit::km()),
                                       false, QDateTime::currentDateTimeUtc().addSecs(1000), QDateTime::currentDateTimeUtc().addSecs(2000));
            return t;
        }

        const CAtcStationList &CTestData::getAtcStations()
        {
            static const CAtcStationList stations(
            {
                CTestData::getFrankfurtTower(),
                CTestData::getMunichTower(),
                CTestData::getMunichApproach()
            });
            return stations;
        }

        const CAtcStation &CTestData::getRandomAtcStation()
        {
            static const int s = CTestData::getAtcStations().size();
            return getAtcStations()[randomIndex(s)];
        }

        const CCallsignSet &CTestData::getPilotCallsigns()
        {
            static const CCallsignSet callsigns(
            {
                CCallsign("DEMBZ"), CCallsign("DLH123"), CCallsign("RYR635L"),
                CCallsign("LGL974"), CCallsign("AUI129"), CCallsign("CLX756")
            });
            return callsigns;
        }

        const CCallsign &CTestData::getRandomPilotCallsign()
        {
            static const QList<CCallsign> callsigns(CTestData::getPilotCallsigns().toQList());
            return callsigns[randomIndex(callsigns.size())];
        }

        const CCallsignSet &CTestData::getControllerCallsigns()
        {
            static const CCallsignSet callsigns(
            {
                CCallsign("EDDM_TWR"), CCallsign("EDDM_APP"), CCallsign("EDDM_GND"),
                CCallsign("EDDF_TWR"), CCallsign("EDDF_APP"), CCallsign("EDDF_GND")
            });
            return callsigns;
        }

        const CCallsign &CTestData::getRandomControllerCallsign()
        {
            static const QList<CCallsign> callsigns(CTestData::getControllerCallsigns().toQList());
            return callsigns[randomIndex(callsigns.size())];
        }

        const CUserList &CTestData::getPilots()
        {
            static const QList<CCallsign> callsigns(CTestData::getPilotCallsigns().toQList());
            static const CUserList pilots(
            {
                CUser("100100", "Joe Doe", callsigns[0]),
                CUser("100101", "Jane Foo", callsigns[1]),
                CUser("100200", "Frank Sky", callsigns[2]),
                CUser("100300", "Richard F. Frings", callsigns[3]),
                CUser("100301", "Torben Frey", callsigns[4]),
                CUser("100400", "Kay Dòresn", callsigns[5]),
            });
            return pilots;
        }

        const CUser &CTestData::getRandomPilot()
        {
            static const int s = CTestData::getPilots().size();
            return CTestData::getPilots()[randomIndex(s)];
        }

        const CUserList &CTestData::getControllers()
        {
            static const QList<CCallsign> callsigns(CTestData::getControllerCallsigns().toQList());
            static const CUserList controllers(
            {
                CUser("300100", "Jeff Doe", callsigns[0]),
                CUser("300101", "Foo Mister", callsigns[1]),
                CUser("300200", "Sky Du", callsigns[2]),
                CUser("300300", "F. Frings", callsigns[3]),
                CUser("300301", "Ralf Smith", callsigns[4]),
                CUser("300400", "Doreen Jump", callsigns[5]),
            });
            return controllers;
        }

        const CUser &CTestData::getRandomController()
        {
            static const int s = CTestData::getControllers().size();
            return CTestData::getControllers()[randomIndex(s)];
        }

        CAircraftSituation getAircraftSituationAboveMunichTowerImpl()
        {
            CAircraftSituation situation(CTestData::getCoordinateMunichTower(), CHeading(10, CHeading::True, CAngleUnit::deg()),
                                         CAngle(12, CAngleUnit::deg()), CAngle(5, CAngleUnit::deg()),
                                         CSpeed(111, CSpeedUnit::km_h()));
            situation.setAltitude({ 2500, CLengthUnit::ft() });
            situation.setCallsign(CTestData::getRandomPilotCallsign());
            return situation;
        }

        const CAircraftSituation &CTestData::getAircraftSituationAboveMunichTower()
        {
            static const CAircraftSituation situation(getAircraftSituationAboveMunichTowerImpl());
            return situation;
        }

        CAircraftSituation getAircraftSituationAboveFrankfurtTowerImpl()
        {
            CAircraftSituation situation(CTestData::getCoordinateFrankfurtTower(), CHeading(290, CHeading::True, CAngleUnit::deg()),
                                         CAngle(4, CAngleUnit::deg()), CAngle(10, CAngleUnit::deg()),
                                         CSpeed(200, CSpeedUnit::km_h()));
            situation.setAltitude({ 4000, CLengthUnit::ft() });
            situation.setCallsign(CTestData::getRandomPilotCallsign());
            return situation;
        }

        const CAircraftSituation &CTestData::getAircraftSituationAboveFrankfurtTower()
        {
            static const CAircraftSituation situation(getAircraftSituationAboveFrankfurtTowerImpl());
            return situation;
        }

        const CAircraftIcaoCode &CTestData::getDBAircraftIcaoB737()
        {
            static const QString json = "{\"combinedType\": \"L2J\", \"dbKey\": 1777, \"designator\": \"B739\", \"family\": \"B737\", \"iataCode\": \"739\", \"legacy\": false, \"manufacturer\": \"BOEING\", \"military\": false, \"modelDescription\": \"737-900 BBJ3\", \"rank\": 10, \"realWorld\": true, \"timestampMSecsSinceEpoch\": 1454013308000, \"wtc\": \"M\" }";
            static const CAircraftIcaoCode icao(CAircraftIcaoCode::fromJson(json));
            return icao;
        }

        const CAircraftIcaoCode &CTestData::getDBAircraftIcaoC172()
        {
            static const QString json = "{ \"combinedType\": \"L1P\", \"dbKey\": 2279, \"designator\": \"C172\", \"family\": \"\", \"iataCode\": \"CN1\", \"legacy\": false, \"manufacturer\": \"CESSNA\", \"military\": false, \"modelDescription\": \"172 Skyhawk\", \"rank\": 1, \"realWorld\": true, \"timestampMSecsSinceEpoch\": 1476144089000, \"wtc\": \"L\" }";
            static const CAircraftIcaoCode icao(CAircraftIcaoCode::fromJson(json));
            return icao;
        }

        const CAirlineIcaoCode &CTestData::getDbAirlineIcaoDLH()
        {
            static const QString json = "{ \"country\": { \"alias1\": \"\", \"alias2\": \"\", \"dbKey\": \"DE\", \"historic\": false, \"iso3\": \"\", \"loadedFromDb\": false, \"name\": \"Germany\", \"simplifiedName\": \"\", \"timestampMSecsSinceEpoch\": -1 }, \"dbKey\": 3221, \"designator\": \"DLH\", \"isMilitary\": false, \"isOperating\": true, \"isVa\": false, \"name\": \"Lufthansa\", \"telephonyDesignator\": \"LUFTHANSA\", \"timestampMSecsSinceEpoch\": 1454179410000 }";
            static const CAirlineIcaoCode icao(CAirlineIcaoCode::fromJson(json));
            Q_ASSERT(icao.hasValidDbKey());
            return icao;
        }

        const CAirlineIcaoCode &CTestData::getDbAirlineIcaoBAW()
        {
            static const QString json = "{ \"country\": { \"alias1\": \"\", \"alias2\": \"\", \"dbKey\": \"GB\", \"historic\": false, \"iso3\": \"\", \"loadedFromDb\": false, \"name\": \"United Kingdom\", \"simplifiedName\": \"\", \"timestampMSecsSinceEpoch\": -1 }, \"dbKey\": 1299, \"designator\": \"BAW\", \"isMilitary\": false, \"isOperating\": true, \"isVa\": false, \"name\": \"British Airways\", \"telephonyDesignator\": \"SPEEDBIRD\", \"timestampMSecsSinceEpoch\": 1433786773000 }";
            static const CAirlineIcaoCode icao(CAirlineIcaoCode::fromJson(json));
            Q_ASSERT(icao.hasValidDbKey());
            return icao;
        }

        const CLivery &CTestData::getDbLiveryDLH()
        {
            static const QString json = "{ \"airline\": { \"country\": { \"alias1\": \"\", \"alias2\": \"\", \"dbKey\": \"DE\", \"historic\": false, \"iso3\": \"\", \"loadedFromDb\": false, \"name\": \"Germany\", \"simplifiedName\": \"\", \"timestampMSecsSinceEpoch\": -1 }, \"dbKey\": 3221, \"designator\": \"DLH\", \"isMilitary\": false, \"isOperating\": true, \"isVa\": false, \"name\": \"Lufthansa\", \"telephonyDesignator\": \"LUFTHANSA\", \"timestampMSecsSinceEpoch\": 1454179410000 }, \"colorFuselage\": { \"b\": 255, \"g\": 255, \"r\": 255 }, \"colorTail\": { \"b\": 102, \"g\": 0, \"r\": 1 }, \"combinedCode\": \"DLH.STAR\", \"dbKey\": 7097, \"description\": \"Lufthansa Star Alliance\", \"military\": false, \"timestampMSecsSinceEpoch\": 1481304168000 }";
            static const CLivery livery(CLivery::fromJson(json));
            Q_ASSERT(livery.hasValidDbKey());
            return livery;
        }

        const CAircraftModel &CTestData::getDbAircraftModelFsxA2AC172Skyhawk()
        {
            static const QString json = "{\"mod_id\": 8227, \"mod_modelstring\": \"C172R N990CP\", \"mod_name\": null, \"mod_description\": \"A2A Cessna C172R Skyhawk\", \"mod_simfsx\": \"Y\", \"mod_simp3d\": \"N\", \"mod_simfs9\": \"N\", \"mod_simxplane\": \"N\", \"mod_enabled\": \"Y\", \"mod_mode\": \"I\", \"mod_created\": \"2016-09-02 22:13:28\", \"mod_lastupdated\": \"2016-09-02 22:13:28\", \"ac_id\": 2279, \"ac_designator\": \"C172\", \"ac_family\": null, \"ac_manufacturer\": \"CESSNA\", \"ac_model\": \"172 Skyhawk\", \"ac_rank\": 1, \"ac_type\": \"L\", \"ac_engine\": \"P\", \"ac_enginecount\": 1, \"ac_wtc\": \"L\", \"ac_realworld\": \"Y\", \"ac_legacy\": \"N\", \"ac_military\": \"N\", \"ac_created\": \"2013-07-31 11:38:42\", \"ac_lastupdated\": \"2016-10-11 00:01:29\", \"dist_id\": \"A2A\", \"dist_description\": \"A2A Simulations\", \"dist_alias1\": \"ACCUSIM\", \"dist_alias2\": null, \"dist_simfsx\": \"Y\", \"dist_simp3d\": \"N\", \"dist_simfs9\": \"N\", \"dist_simxplane\": \"N\", \"dist_created\": \"2016-08-26 15:26:10\", \"dist_lastupdated\": \"2016-08-26 15:26:10\", \"liv_id\": 7076, \"liv_idairlineicao\": null, \"liv_combinedcode\": \"_CC_NOCOLOR\", \"liv_description\": \"Temp: No color, no airline\", \"liv_colorfuselage\": null, \"liv_colortail\": null, \"liv_military\": \"N\", \"liv_created\": \"2016-01-30 17:43:15\", \"liv_lastupdated\": \"2016-01-30 17:43:15\", \"al_id\": null, \"al_designator\": null, \"al_callsign\": null, \"al_name\": null, \"al_country\": null, \"al_countryname\": null, \"al_va\": null, \"al_military\": null, \"al_operating\": null, \"al_created\": null, \"al_lastupdated\": null}";
            static const CAircraftModel model(CAircraftModel::fromDatabaseJson(Json::jsonObjectFromString(json)));
            Q_ASSERT(model.hasValidDbKey());
            return model;
        }

        const CAircraftModel &CTestData::getDbAircraftModelFsxAerosoftA320()
        {
            static const QString json = "{\"mod_id\": 6651, \"mod_modelstring\": \"AIRBUS A321 LUFTHANSA D-AISH\", \"mod_name\": \"A321\", \"mod_description\": \"Airbus A321-231 IAE\", \"mod_simfsx\": \"Y\", \"mod_simp3d\": \"N\", \"mod_simfs9\": \"N\", \"mod_simxplane\": \"N\", \"mod_enabled\": \"Y\", \"mod_mode\": \"I\", \"mod_created\": \"2016-08-14 00:18:19\", \"mod_lastupdated\": \"2016-12-03 21:45:54\", \"ac_id\": 673, \"ac_designator\": \"A321\", \"ac_family\": \"A320\", \"ac_manufacturer\": \"AIRBUS\", \"ac_model\": \"A-321\", \"ac_rank\": 0, \"ac_type\": \"L\", \"ac_engine\": \"J\", \"ac_enginecount\": 2, \"ac_wtc\": \"M\", \"ac_realworld\": \"Y\", \"ac_legacy\": \"N\", \"ac_military\": \"N\", \"ac_created\": \"2013-07-31 11:38:42\", \"ac_lastupdated\": \"2016-01-28 20:41:09\", \"dist_id\": \"AEROSOFT\", \"dist_description\": \"Aerosoft\", \"dist_alias1\": \"AS\", \"dist_alias2\": \"AERO\", \"dist_simfsx\": \"Y\", \"dist_simp3d\": \"N\", \"dist_simfs9\": \"N\", \"dist_simxplane\": \"N\", \"dist_created\": \"2016-01-23 20:22:06\", \"dist_lastupdated\": \"2016-01-23 20:22:06\", \"liv_id\": 1928, \"liv_idairlineicao\": 3221, \"liv_combinedcode\": \"DLH._STD\", \"liv_description\": \"Standard Lufthansa\", \"liv_colorfuselage\": \"FFFFFF\", \"liv_colortail\": \"010066\", \"liv_military\": \"N\", \"liv_created\": \"2015-08-21 00:39:49\", \"liv_lastupdated\": \"2015-08-31 19:00:16\", \"al_id\": 3221, \"al_designator\": \"DLH\", \"al_callsign\": \"LUFTHANSA\", \"al_name\": \"Lufthansa\", \"al_country\": \"DE\", \"al_countryname\": \"Germany\", \"al_va\": \"N\", \"al_military\": \"N\", \"al_operating\": \"Y\", \"al_created\": \"2013-07-29 23:59:43\", \"al_lastupdated\": \"2016-01-30 18:43:30\"}";
            static const CAircraftModel model(CAircraftModel::fromDatabaseJson(Json::jsonObjectFromString(json)));
            Q_ASSERT(model.hasValidDbKey());
            return model;
        }

        const CSimulatedAircraft getC172AircraftImpl()
        {
            CUser pilot = CTestData::getRandomPilot();
            CSimulatedAircraft aircraft(pilot.getCallsign(), CTestData::getDbAircraftModelFsxA2AC172Skyhawk(), pilot, CTestData::getAircraftSituationAboveMunichTower());
            CTransponder t = CTransponder(7000, CTransponder::ModeC);
            aircraft.setTransponder(t);
            aircraft.setCom1ActiveFrequency(CTestData::getRandomAtcStation().getFrequency());
            aircraft.setCom2ActiveFrequency(CTestData::getRandomAtcStation().getFrequency());
            return aircraft;
        }

        const CSimulatedAircraft &CTestData::getC172Aircraft()
        {
            static const CSimulatedAircraft aircraft(getC172AircraftImpl());
            return aircraft;
        }

        const CSimulatedAircraft getA320AircraftImpl()
        {
            CUser pilot = CTestData::getRandomPilot();
            CSimulatedAircraft aircraft(pilot.getCallsign(), CTestData::getDbAircraftModelFsxAerosoftA320(), pilot, CTestData::getAircraftSituationAboveFrankfurtTower());
            CTransponder t = CTransponder(7000, CTransponder::ModeC);
            aircraft.setTransponder(t);
            aircraft.setCom1ActiveFrequency(CTestData::getRandomAtcStation().getFrequency());
            aircraft.setCom2ActiveFrequency(CTestData::getRandomAtcStation().getFrequency());
            return aircraft;
        }

        const CSimulatedAircraft &CTestData::getA320Aircraft()
        {
            static const CSimulatedAircraft aircraft(getA320AircraftImpl());
            return aircraft;
        }
    } // ns
} // ns
