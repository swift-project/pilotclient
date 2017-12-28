/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include "testaviation.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/aviation/informationmessage.h"
#include "blackmisc/aviation/navsystem.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/compare.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/pq/units.h"

#include <QDateTime>
#include <QString>
#include <QTest>

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

        CCallsignSet set;
        set.push_back(cs1);
        QVERIFY2(set.size() == 1, "List shall be 1");
        QVERIFY2(set.contains(cs1), "Callsign is in list");
        QVERIFY2(set.contains(cs2), "Callsign is in list");
        set.remove(cs1);
        QVERIFY2(cs1 == cs1, "Callsign is the same, shall be equal");
        QVERIFY2(set.size() == 0, "List shall be 0 after removal");
        set.push_back(cs1);
        if (!set.contains(cs2)) set.push_back(cs2);
        QVERIFY2(set.size() == 1, "Duplicates shall not be added");
        set.push_back(cs3);
        QVERIFY2(set.size() == 2, "2 different callsigns");
        set.remove(cs1);
        QVERIFY2(set.size() == 1, "Only one should be left in list");
        set.removeIf(&CCallsign::getTelephonyDesignator, "München Radar");
        QVERIFY2(set.size() == 0, "Last should be gone");
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

        CServer server1 = CServer::swiftFsdTestServer();
        CServer server2(server1);
        QVERIFY2(server1 == server2, "server shall be equal");

        CAircraftSituation situation1(
            CCoordinateGeodetic(
                CLatitude::fromWgs84("N 049° 18' 17"),
                CLongitude::fromWgs84("E 008° 27' 05"),
                CAltitude(312, CLengthUnit::ft()))
        );
        CAircraftSituation situation2(situation1);
        QVERIFY2(situation1 == situation2, "situations shall be equal");

        CAircraftIcaoCode aircraftIcao1("C172", "L1P");
        CAircraftIcaoCode aircraftIcao2(aircraftIcao1);
        QVERIFY2(aircraftIcao1 == aircraftIcao2, "aircraft ICAOs shall be equal");

        CAirlineIcaoCode airlineIcao1("GA");
        CAirlineIcaoCode airlineIcao2(airlineIcao1);
        QVERIFY2(airlineIcao1 == airlineIcao2, "airline ICAOs shall be equal");

        CCallsign call1("EDDS_N_APP", CCallsign::Atc);
        CCallsign call2("edds_n_app", CCallsign::Atc);
        QVERIFY2(call1 == call2, "Callsigns shall be equal");

        CAtcStation atc1(c1, user1, f1, situation1.getPosition(), CLength(), false, QDateTime(), QDateTime(), CInformationMessage(CInformationMessage::ATIS, "foo"));
        CAtcStation atc2(c1, user1, f1, situation1.getPosition(), CLength(), false, QDateTime(), QDateTime(), CInformationMessage(CInformationMessage::ATIS, "foo"));
        CAtcStation atc3(c1, user1, f1, situation1.getPosition(), CLength(), false, QDateTime(), QDateTime(), CInformationMessage(CInformationMessage::ATIS, "bar"));
        QVERIFY2(atc1 == atc2, "ATC stations shall be equal");
        QVERIFY2(atc1 != atc3, "ATC stations shall not be equal");
    }

} // namespace

//! \endcond
