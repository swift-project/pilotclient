/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testaviation.h"
#include "blackmisc/pq/constants.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/aviation/informationmessage.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircrafticao.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/navsystem.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/network/user.h"
#include "blackmisc/network/server.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;

namespace BlackMiscTest
{

    /*
     * Constructor
     */
    CTestAviation::CTestAviation(QObject *parent): QObject(parent)
    {
        // void
    }

    /*
     * Basic tests
     */
    void CTestAviation::headingBasics()
    {
        CHeading h1(180, CHeading::Magnetic, CAngleUnit::deg());
        CHeading h2(180, CHeading::True, CAngleUnit::deg());
        CHeading h3(181, CHeading::Magnetic, CAngleUnit::deg());
        CAngle a1(200, CAngleUnit::deg());
        CHeading h4;
        h4 = h1;
        QVERIFY2(h1 != h2, "Magnetic and true heading are not the same");
        QVERIFY2(h1 < h3, "180deg are less than 181deg");
        QVERIFY2(h3 > h1, "181deg are more than 181deg");
        QVERIFY2(a1 > h3, "200deg are more than 181deg");
        QVERIFY2(h4 == h1, "Values shall be equal");

        h1 -= h1;
        QVERIFY2(h1.value() == 0, "Value shall be 0");

        // h4 = h1 + h2; does not work, because misleading
        h2 += h2; // add just angle
        QVERIFY2(h2.value() == 360, "Value shall be 360");
    }

    /*
     * COM and NAV units
     */
    void CTestAviation::comAndNav()
    {
        CComSystem c1 = CComSystem::getCom1System(122.8);
        CComSystem c2 = CComSystem::getCom2System(122.8);
        QVERIFY2(c1 != c2, "COM system shall not be equal");
        c1 = c2;
        QVERIFY2(c1 == c2, "COM system shall be equal");
        QVERIFY2(CNavSystem::isValidCivilNavigationFrequency(CFrequency(110.0, CFrequencyUnit::MHz())), "Expect valid nav frequency");
        QVERIFY2(!CNavSystem::isValidCivilNavigationFrequency(CFrequency(200.0, CFrequencyUnit::MHz())), "Expect invalid nav frequency");
    }

    /*
     * COM and NAV units
     */
    void CTestAviation::transponder()
    {
        CTransponder t1 = CTransponder::getStandardTransponder(7000, CTransponder::StateStandby);
        CTransponder t2 = t1;
        QVERIFY2(t1 == t2, "Transponders shall be equal");
        t2.setTransponderMode(CTransponder::ModeC);
        QVERIFY2(t1 != t2, "Transponders shall not be equal");
        CTransponder tv = CTransponder(8888, CTransponder::StateStandby);
        QVERIFY2(!tv.validValues(), "No valid transponder");
        tv = CTransponder(781, CTransponder::StateStandby);
        QVERIFY2(!tv.validValues(), "No valid transponder");
        tv = CTransponder("0781", CTransponder::StateStandby);
        QVERIFY2(!tv.validValues(), "No valid transponder");
        tv = CTransponder("7211", CTransponder::StateStandby);
        QVERIFY2(tv.validValues(), "No valid transponder");
        tv = CTransponder("schnitzel", CTransponder::StateStandby);
        QVERIFY2(!tv.validValues(), "No valid transponder");
    }

    /*
     * Callsign
     */
    void CTestAviation::callsignWithContainers()
    {
        CCallsign cs1("EDDm_twr");
        CCallsign cs2("eddm_TWR");
        CCallsign cs3("EDDm_app", "München Radar");
        QVERIFY2(cs1 == cs2, "Callsigns shall be equal");
        QVERIFY2(cs1 != cs3, "Callsigns shall not be equal");

        CCallsignSet list;
        list.push_back(cs1);
        QVERIFY2(list.size() == 1, "List shall be 1");
        QVERIFY2(list.contains(cs1), "Callsign is in list");
        QVERIFY2(list.contains(cs2), "Callsign is in list");
        list.remove(cs1);
        QVERIFY2(cs1 == cs1, "Callsign is the same, shall be equal");
        QVERIFY2(list.size() == 0, "List shall be 0 after removal");
        list.push_back(cs1);
        if (!list.contains(cs2)) list.push_back(cs2);
        QVERIFY2(list.size() == 1, "Duplicates shall not be added");
        list.push_back(cs3);
        QVERIFY2(list.size() == 2, "2 different callsigns");
        list.remove(cs1);
        QVERIFY2(list.size() == 1, "Only one should be left in list");
        list.removeIf(&CCallsign::getTelephonyDesignator, "München Radar");
        QVERIFY2(list.size() == 0, "Last should be gone");
    }

    /*
     * Copy and compare
     */
    void CTestAviation::copyAndEqual()
    {
        CFrequency f1(123.45, CFrequencyUnit::MHz());
        CFrequency f2(f1);
        QVERIFY2(f1 == f2, "frequencies shall be equal");

        CCallsign c1("EABCD");
        CCallsign c2(c1);
        QVERIFY2(c1 == c2, "callsigns shall be equal");

        CInformationMessage im1(CInformationMessage::METAR, "I am a metar");
        CInformationMessage im2(im1);
        QVERIFY2(im1 == im2, "information shall be equal");

        CUser user1("112233dd", "Joe", "", "secret");
        CUser user2(user1);
        QVERIFY2(user1 == user2, "information shall be equal");

        CServer server1("development", "VATSIM dev.server", "vatsim-germany.org", 6809, user1);
        CServer server2(server1);
        QVERIFY2(server1 == server2, "server shall be equal");

        CAircraftSituation situation1(
            CCoordinateGeodetic(
                CLatitude::fromWgs84("N 049° 18' 17"),
                CLongitude::fromWgs84("E 008° 27' 05"),
                CLength(0, CLengthUnit::m())),
            CAltitude(312, CAltitude::MeanSeaLevel, CLengthUnit::ft())
        );
        CAircraftSituation situation2(situation1);
        QVERIFY2(situation1 == situation2, "situations shall be equal");

        CAircraftIcao icao1(CAircraftIcao("C172", "L1P", "GA", "GA", "0000ff"));
        CAircraftIcao icao2(icao1);
        QVERIFY2(icao1 == icao2, "ICAOs shall be equal");

        CCallsign call1("EDDS_N_APP");
        CCallsign call2("edds_n_app");
        QVERIFY2(call1 == call2, "Callsigns shall be equal");

        CAtcStation atc1(c1, user1, f1, situation1.getPosition(), CLength(), false, QDateTime(), QDateTime(), CInformationMessage(CInformationMessage::ATIS, "foo"));
        CAtcStation atc2(c1, user1, f1, situation1.getPosition(), CLength(), false, QDateTime(), QDateTime(), CInformationMessage(CInformationMessage::ATIS, "foo"));
        CAtcStation atc3(c1, user1, f1, situation1.getPosition(), CLength(), false, QDateTime(), QDateTime(), CInformationMessage(CInformationMessage::ATIS, "bar"));
        QVERIFY2(atc1 == atc2, "ATC stations shall be equal");
        QVERIFY2(atc1 != atc3, "ATC stations shall not be equal");
    }

} // namespace
