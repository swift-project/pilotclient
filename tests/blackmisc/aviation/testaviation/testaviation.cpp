// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

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
#include "blackmisc/aviation/waketurbulencecategory.h"
#include "blackmisc/mixin/mixincompare.h"
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
#include "test.h"

#include <QDateTime>
#include <QString>
#include <QTest>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;

namespace BlackMiscTest
{
    //! Aviation classes basic tests
    class CTestAviation : public QObject
    {
        Q_OBJECT

    private slots:
        //! Basic unit tests for physical units
        void headingBasics();

        //! COM and NAV units
        void comAndNav();

        //! Same frequency in aviation context
        void sameAviationFrequency();

        //! COM frequency rounding
        void comFrequencyRounding();

        //! Transponder
        void transponder();

        //! Callsigns and callsign containers
        void callsignWithContainers();

        //! Testing copying and equality of objects
        void copyAndEqual();

        //! CAltitude tests
        void altitude();

        //! Test some of the guessing functions
        void testGuessing();

        //! Test wake turbulence categories
        void testWakeTurbulenceCategories();
    };

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

        const CHeading copy(h1);
        h1 -= copy;
        QVERIFY2(h1.isZeroEpsilonConsidered(), "Expect zero value");
        QCOMPARE(h1.value(), 0.0);

        // h4 = h1 + h2; does not work, because misleading
        h2 += h2; // add just angle
        QCOMPARE(h2.value(), 360.0);
    }

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

    void CTestAviation::sameAviationFrequency()
    {
        {
            CFrequency freq1(129620000, CFrequencyUnit::Hz());
            CFrequency freq2(129620000, CFrequencyUnit::Hz());
            QVERIFY2(CComSystem::isSameFrequency(freq1, freq2), "Frequencies should be the same");
        }

        {
            CFrequency freq1(122.8, CFrequencyUnit::MHz());
            CFrequency freq2(122.8, CFrequencyUnit::MHz());
            QVERIFY2(CComSystem::isSameFrequency(freq1, freq2), "Frequencies should be the same");
        }

        {
            // Should be the same despite small rounding error in Hz range
            CFrequency freq1(123450001, CFrequencyUnit::Hz());
            CFrequency freq2(123450, CFrequencyUnit::kHz());
            QVERIFY2(CComSystem::isSameFrequency(freq1, freq2), "Frequencies should be the same");
        }

        {
            // Ending with 20/70 is treated the same as 25/75 (correct value) as old radios only had 2 digits available after the decimal point
            CFrequency freq1(118.325, CFrequencyUnit::MHz());
            CFrequency freq2(118.320, CFrequencyUnit::MHz());
            CFrequency freq3(132.770, CFrequencyUnit::MHz());
            CFrequency freq4(132.775, CFrequencyUnit::MHz());
            CFrequency freq5(132.765, CFrequencyUnit::MHz());
            CFrequency freq6(132.780, CFrequencyUnit::MHz());

            QVERIFY2(CComSystem::isSameFrequency(freq1, freq2), "Frequencies should be the same");
            QVERIFY2(CComSystem::isSameFrequency(freq3, freq4), "Frequencies should be the same");
            QVERIFY2(!CComSystem::isSameFrequency(freq1, freq3), "Frequencies should not be the same");
            QVERIFY2(!CComSystem::isSameFrequency(freq1, freq4), "Frequencies should not be the same");
            QVERIFY2(!CComSystem::isSameFrequency(freq2, freq3), "Frequencies should not be the same");
            QVERIFY2(!CComSystem::isSameFrequency(freq2, freq4), "Frequencies should not be the same");
            QVERIFY2(!CComSystem::isSameFrequency(freq3, freq5), "Frequencies should not be the same");
            QVERIFY2(!CComSystem::isSameFrequency(freq3, freq6), "Frequencies should not be the same");
            QVERIFY2(!CComSystem::isSameFrequency(freq4, freq5), "Frequencies should not be the same");
            QVERIFY2(!CComSystem::isSameFrequency(freq4, freq6), "Frequencies should not be the same");
        }

        {
            CFrequency freq1(129620000, CFrequencyUnit::Hz());
            CFrequency freq2(132025000, CFrequencyUnit::Hz());
            QVERIFY2(!CComSystem::isSameFrequency(freq1, freq2), "Frequencies should not be the same");
        }

        // 8.33 kHz frequencies
        {
            CFrequency freq1(118.305, CFrequencyUnit::MHz());
            CFrequency freq2(118305, CFrequencyUnit::kHz());
            QVERIFY2(CComSystem::isSameFrequency(freq1, freq2), "Frequencies should be the same");
        }

        {
            CFrequency freq1(118.310, CFrequencyUnit::MHz());
            CFrequency freq2(118305, CFrequencyUnit::kHz());
            QVERIFY2(!CComSystem::isSameFrequency(freq1, freq2), "Frequencies should not be the same");
        }

        {
            CFrequency freq1(135.660, CFrequencyUnit::MHz());
            CFrequency freq2(135665, CFrequencyUnit::kHz());
            QVERIFY2(!CComSystem::isSameFrequency(freq1, freq2), "Frequencies should not be the same");
        }
    }

    void CTestAviation::comFrequencyRounding()
    {
        const CFrequency f1 = CFrequency(122.8, CFrequencyUnit::MHz());
        const CFrequency f2 = CFrequency(122.795, CFrequencyUnit::MHz());
        const CFrequency f3 = CFrequency(122.805, CFrequencyUnit::MHz());
        const CFrequency f4 = CFrequency(122.225, CFrequencyUnit::MHz());
        const CFrequency f5 = CFrequency(122.220, CFrequencyUnit::MHz());

        QVERIFY2(f1 == f1, "Ups, how can this fail");
        QVERIFY2(f1 != f2, "Ups, how can this fail");
        QVERIFY2(f1 != f3, "Ups, how can this fail");
        QVERIFY2(f4 != f5, "Ups, how can this fail");

        CFrequency up(f2);
        CComSystem::roundToChannelSpacing(up, CComSystem::ChannelSpacing25KHz);
        QVERIFY2(up == f1, "Expect rounding up");

        CFrequency down(f3);
        CComSystem::roundToChannelSpacing(down, CComSystem::ChannelSpacing25KHz);
        QVERIFY2(down == f1, "Expect rounding up");

        CFrequency same(f3);
        CComSystem::roundToChannelSpacing(same, CComSystem::ChannelSpacing8_33KHz);
        QVERIFY2(same != f1, "Expect no rounding");
        QVERIFY2(same == f3, "Expect no rounding");

        CFrequency up2(f5);
        CComSystem::roundToChannelSpacing(up2, CComSystem::ChannelSpacing8_33KHz);
        QVERIFY2(up2 == f4, "Expect rounding up");
    }

    void CTestAviation::transponder()
    {
        const CTransponder t1 = CTransponder::getStandardTransponder(7000, CTransponder::StateStandby);
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

    void CTestAviation::callsignWithContainers()
    {
        const CCallsign cs1("EDDm_twr");
        const CCallsign cs2("eddm_TWR");
        const CCallsign cs3("EDDm_app", "München Radar");
        QVERIFY2(cs1 == cs2, "Callsigns shall be equal");
        QVERIFY2(cs1 != cs3, "Callsigns shall not be equal");

        const CCallsign pilot("DLH123");
        const CCallsign copilot1("DLH123A");
        const CCallsign copilot2("DLH1233");
        const CCallsign copilot3("DLH12");
        QVERIFY(copilot1.isMaybeCopilotCallsign(pilot));
        QVERIFY(!pilot.isMaybeCopilotCallsign(pilot));
        QVERIFY(!copilot2.isMaybeCopilotCallsign(pilot));
        QVERIFY(!copilot3.isMaybeCopilotCallsign(pilot));

        QString flightnumber;
        QString identifier;

        QString prefix = pilot.getAirlinePrefix(flightnumber, identifier);
        QVERIFY(prefix == "DLH");
        QVERIFY(flightnumber == "123");
        QVERIFY(flightnumber == identifier);
        prefix = copilot1.getAirlinePrefix(flightnumber, identifier);
        QVERIFY(prefix == "DLH");
        QVERIFY(flightnumber == "123");
        QVERIFY(flightnumber != identifier);
        prefix = copilot3.getAirlinePrefix(flightnumber);
        QVERIFY(prefix == "DLH");
        QVERIFY(flightnumber == "12");

        const CCallsign pilotX1("DLHFOO");
        prefix = pilotX1.getAirlinePrefix(flightnumber);
        QVERIFY(prefix.isEmpty()); // no prefix
        QVERIFY(flightnumber.isEmpty());

        const CCallsign pilotX2("DLH1WP");
        prefix = pilotX2.getAirlinePrefix(flightnumber, identifier);
        QVERIFY(prefix == "DLH"); // no prefix
        QVERIFY(flightnumber == "1");
        QVERIFY(identifier == "1WP");

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

    void CTestAviation::copyAndEqual()
    {
        const CFrequency f1(123.45, CFrequencyUnit::MHz());
        const CFrequency f2(f1);
        QVERIFY2(f1 == f2, "frequencies shall be equal");

        const CCallsign c1("EABCD");
        const CCallsign c2(c1);
        QVERIFY2(c1 == c2, "callsigns shall be equal");

        const CInformationMessage im1(CInformationMessage::METAR, "I am a metar");
        const CInformationMessage im2(im1);
        QVERIFY2(im1 == im2, "information shall be equal");

        const CUser user1("112233dd", "Joe", "", "secret");
        const CUser user2(user1);
        QVERIFY2(user1 == user2, "information shall be equal");

        const CServer server1 = CServer("Testserver", "Client project testserver", "localhost", 6809,
                                        CUser("111111", "My Name", "", "123"),
                                        CFsdSetup(), CVoiceSetup(), CEcosystem(CEcosystem::swiftTest()), CServer::FSDServerVatsim);
        const CServer server2(server1);
        QVERIFY2(server1 == server2, "server shall be equal");

        const CAircraftSituation situation1(
            CCoordinateGeodetic(
                CLatitude::fromWgs84("N 049° 18' 17"),
                CLongitude::fromWgs84("E 008° 27' 05"),
                CAltitude(312, CLengthUnit::ft())));
        const CAircraftSituation situation2(situation1);
        QVERIFY2(situation1 == situation2, "situations shall be equal");

        const CAircraftIcaoCode aircraftIcao1("C172", "L1P");
        const CAircraftIcaoCode aircraftIcao2(aircraftIcao1);
        QVERIFY2(aircraftIcao1 == aircraftIcao2, "aircraft ICAOs shall be equal");

        const CAirlineIcaoCode airlineIcao1("GA");
        const CAirlineIcaoCode airlineIcao2(airlineIcao1);
        QVERIFY2(airlineIcao1 == airlineIcao2, "airline ICAOs shall be equal");

        const CCallsign call1("EDDS_N_APP", CCallsign::Atc);
        const CCallsign call2("edds_n_app", CCallsign::Atc);
        QVERIFY2(call1 == call2, "Callsigns shall be equal");

        const CAtcStation atc1(c1, user1, f1, situation1.getPosition(), CLength(), false, QDateTime(), QDateTime(), CInformationMessage(CInformationMessage::ATIS, "foo"));
        const CAtcStation atc2(c1, user1, f1, situation1.getPosition(), CLength(), false, QDateTime(), QDateTime(), CInformationMessage(CInformationMessage::ATIS, "foo"));
        const CAtcStation atc3(c1, user1, f1, situation1.getPosition(), CLength(), false, QDateTime(), QDateTime(), CInformationMessage(CInformationMessage::ATIS, "bar"));
        QVERIFY2(atc1 == atc2, "ATC stations shall be equal");
        QVERIFY2(atc1 != atc3, "ATC stations shall not be equal");
    }

    void CTestAviation::altitude()
    {
        CAltitude altitude1(448, CAltitude::MeanSeaLevel, CLengthUnit::ft());
        CPressure seaLevelPressure1(1025, CPressureUnit::mbar());
        CAltitude pressureAltitude1 = altitude1.toPressureAltitude(seaLevelPressure1);
        QCOMPARE(pressureAltitude1.value(CLengthUnit::ft()), 95.5);

        CAltitude altitude2(500, CAltitude::MeanSeaLevel, CLengthUnit::m());
        CPressure seaLevelPressure2(29.56, CPressureUnit::inHg());
        CAltitude pressureAltitude2 = altitude2.toPressureAltitude(seaLevelPressure2);
        QCOMPARE(qRound(pressureAltitude2.value(CLengthUnit::m())), 612);

        CLength offset(10, CLengthUnit::m());
        altitude2 += offset;
        QCOMPARE(qRound(altitude2.value(CLengthUnit::m())), 510);
    }

    void CTestAviation::testGuessing()
    {
        const CAircraftIcaoCode icao172("C172", "L1P");
        const CAircraftIcaoCode icaoB737("B737", "L2J");
        const CAircraftIcaoCode icaoB747("B747", "L4J");

        CSpeed s172, sB737, sB747;
        s172 = sB737 = sB747 = CSpeed::null();
        CLength cg172, cgB737, cgB747;
        cg172 = cgB737 = cgB747 = CLength::null();
        icao172.guessModelParameters(cg172, s172);
        icaoB737.guessModelParameters(cgB737, sB737);
        icaoB747.guessModelParameters(cgB747, sB747);

        QVERIFY(cg172 < cgB737);
        QVERIFY(cgB737 < cgB747);

        QVERIFY(s172 < sB747);
        QVERIFY(sB737 < sB747);
    }

    void CTestAviation::testWakeTurbulenceCategories()
    {
        const CWakeTurbulenceCategory catLight1('L');
        const CWakeTurbulenceCategory catLight2('l');
        const CWakeTurbulenceCategory catLight3(CWakeTurbulenceCategory::LIGHT);

        const CWakeTurbulenceCategory catMedium1('M');
        const CWakeTurbulenceCategory catMedium2('m');
        const CWakeTurbulenceCategory catMedium3(CWakeTurbulenceCategory::MEDIUM);

        const CWakeTurbulenceCategory catHeavy1('H');
        const CWakeTurbulenceCategory catHeavy2('h');
        const CWakeTurbulenceCategory catHeavy3(CWakeTurbulenceCategory::HEAVY);

        const CWakeTurbulenceCategory catSuper1('J');
        const CWakeTurbulenceCategory catSuper2('j');
        const CWakeTurbulenceCategory catSuper3(CWakeTurbulenceCategory::SUPER);

        const CWakeTurbulenceCategory catUnknown1('-');
        const CWakeTurbulenceCategory catUnknown2('A');
        const CWakeTurbulenceCategory catUnknown3('x');
        const CWakeTurbulenceCategory catUnknown4(CWakeTurbulenceCategory::UNKNOWN);

        QVERIFY(catLight1.isCategory(BlackMisc::Aviation::CWakeTurbulenceCategory::LIGHT));
        QVERIFY(catLight2.isCategory(BlackMisc::Aviation::CWakeTurbulenceCategory::LIGHT));
        QVERIFY(catLight3.isCategory(BlackMisc::Aviation::CWakeTurbulenceCategory::LIGHT));
        QCOMPARE(catLight1.toQString(), "L");

        QVERIFY(catMedium1.isCategory(BlackMisc::Aviation::CWakeTurbulenceCategory::MEDIUM));
        QVERIFY(catMedium2.isCategory(BlackMisc::Aviation::CWakeTurbulenceCategory::MEDIUM));
        QVERIFY(catMedium3.isCategory(BlackMisc::Aviation::CWakeTurbulenceCategory::MEDIUM));
        QCOMPARE(catMedium1.toQString(), "M");

        QVERIFY(catHeavy1.isCategory(BlackMisc::Aviation::CWakeTurbulenceCategory::HEAVY));
        QVERIFY(catHeavy2.isCategory(BlackMisc::Aviation::CWakeTurbulenceCategory::HEAVY));
        QVERIFY(catHeavy3.isCategory(BlackMisc::Aviation::CWakeTurbulenceCategory::HEAVY));
        QCOMPARE(catHeavy1.toQString(), "H");

        QVERIFY(catSuper1.isCategory(BlackMisc::Aviation::CWakeTurbulenceCategory::SUPER));
        QVERIFY(catSuper2.isCategory(BlackMisc::Aviation::CWakeTurbulenceCategory::SUPER));
        QVERIFY(catSuper3.isCategory(BlackMisc::Aviation::CWakeTurbulenceCategory::SUPER));
        QCOMPARE(catSuper1.toQString(), "J");

        QVERIFY(catUnknown1.isCategory(BlackMisc::Aviation::CWakeTurbulenceCategory::UNKNOWN));
        QVERIFY(catUnknown1.isUnknown());
        QVERIFY(catUnknown2.isCategory(BlackMisc::Aviation::CWakeTurbulenceCategory::UNKNOWN));
        QVERIFY(catUnknown2.isUnknown());
        QVERIFY(catUnknown3.isCategory(BlackMisc::Aviation::CWakeTurbulenceCategory::UNKNOWN));
        QVERIFY(catUnknown3.isUnknown());
        QVERIFY(catUnknown4.isCategory(BlackMisc::Aviation::CWakeTurbulenceCategory::UNKNOWN));
        QVERIFY(catUnknown4.isUnknown());
        QCOMPARE(catUnknown1.toQString(), "-");
    }

} // namespace

//! main
BLACKTEST_APPLESS_MAIN(BlackMiscTest::CTestAviation);

#include "testaviation.moc"

//! \endcond
