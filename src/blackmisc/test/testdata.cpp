/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/test/testdata.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/network/server.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/variantlist.h"
#include "blackmisc/math/mathutils.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;
using namespace BlackMisc::Network;
using namespace BlackMisc::Audio;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

//! \cond
namespace BlackMisc::Test
{
    int randomIndex(int size)
    {
        return CMathUtils::randomInteger(0, size - 1);
    }

    const CServer &CTestData::getTrafficServer()
    {
        static const CServer trafficServer("fooserver", "a foo server", "localhost", 1234,
                                            CUser("112233", "Some real name", "email@xyz.com", "secret"),
                                            CFsdSetup(), CVoiceSetup(), CEcosystem(CEcosystem::VATSIM), CServer::FSDServerVatsim);
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

    CPropertyIndexVariantMap getCPropertyIndexVariantMapImpl()
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
        static const CFlightPlan fp(CCallsign("DAMBZ", CCallsign::Aircraft),
                                    "T/A320/F", "EDDF", "EDDM", "EDDN",
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
        static const QString json = CFileUtils::readFileToString(CSwiftDirectories::testFilesDirectory(), "DBAircraftIcaoB737Boeing.json");
        static const CAircraftIcaoCode icao(CAircraftIcaoCode::fromJson(json));
        Q_ASSERT(icao.hasValidDbKey());
        return icao;
    }

    const CAircraftIcaoCode &CTestData::getDBAircraftIcaoC172()
    {
        static const QString json = CFileUtils::readFileToString(CSwiftDirectories::testFilesDirectory(), "DBAircraftIcaoC172Cessna.json");
        static const CAircraftIcaoCode icao(CAircraftIcaoCode::fromJson(json));
        Q_ASSERT(icao.hasValidDbKey());
        return icao;
    }

    const CAirlineIcaoCode &CTestData::getDbAirlineIcaoDLH()
    {
        static const QString json = CFileUtils::readFileToString(CSwiftDirectories::testFilesDirectory(), "DBAirlineIcaoDLH.json");
        static const CAirlineIcaoCode icao(CAirlineIcaoCode::fromJson(json));
        Q_ASSERT(icao.hasValidDbKey());
        return icao;
    }

    const CAirlineIcaoCode &CTestData::getDbAirlineIcaoBAW()
    {
        static const QString json = CFileUtils::readFileToString(CSwiftDirectories::testFilesDirectory(), "DBAirlineIcaoBAW.json");
        static const CAirlineIcaoCode icao(CAirlineIcaoCode::fromJson(json));
        Q_ASSERT(icao.hasValidDbKey());
        return icao;
    }

    const CLivery &CTestData::getDbLiveryDLHStarAlliance()
    {
        static const QString json = CFileUtils::readFileToString(CSwiftDirectories::testFilesDirectory(), "DBLiveryDLHStarAlliance.json");
        static const CLivery livery(CLivery::fromJson(json));
        Q_ASSERT(livery.hasValidDbKey());
        return livery;
    }

    const CAircraftModel &CTestData::getDbAircraftModelFsxA2AC172Skyhawk()
    {
        static const QString json = CFileUtils::readFileToString(CSwiftDirectories::testFilesDirectory(), "DBModelFSXA2ACessnaC172.json");
        static const CAircraftModel model(CAircraftModel::fromDatabaseJson(Json::jsonObjectFromString(json)));
        Q_ASSERT(model.hasValidDbKey());
        Q_ASSERT(!model.getModelString().isEmpty());
        return model;
    }

    const CAircraftModel &CTestData::getDbAircraftModelFsxAerosoftA320()
    {
        static const QString json = CFileUtils::readFileToString(CSwiftDirectories::testFilesDirectory(), "DBModelFSXAerosoftA320.json");
        static const CAircraftModel model(CAircraftModel::fromDatabaseJson(Json::jsonObjectFromString(json)));
        Q_ASSERT(model.hasValidDbKey());
        Q_ASSERT(!model.getModelString().isEmpty());
        return model;
    }

    CSimulatedAircraft getC172AircraftImpl()
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

    CSimulatedAircraft getA320AircraftImpl()
    {
        CUser pilot = CTestData::getRandomPilot();
        CSimulatedAircraft aircraft(pilot.getCallsign(), CTestData::getDbAircraftModelFsxAerosoftA320(), pilot, CTestData::getAircraftSituationAboveFrankfurtTower());
        const CTransponder t = CTransponder(7000, CTransponder::ModeC);
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
//! \endcond
