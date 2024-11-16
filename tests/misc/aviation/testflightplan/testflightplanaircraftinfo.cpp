//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc

#include <QTest>

#include "test.h"

#include "misc/aviation/flightplanaircraftinfo.h"

using namespace swift::misc::aviation;

namespace MiscTest
{
    //! Flightplan unit tests
    class CTestFlightPlanAircraftInfo : public QObject
    {
        Q_OBJECT

    private slots:
        //! Convert ICAO equipment code to CFlightPlanAircraftInfo and vice versa
        void icaoConvert();

        //! Convert FAA equipment code to CFlightPlanAircraftInfo and vice versa
        void faaConvert();

        //! Convert non FAA/ICAO equipment code to CFlightPlanAircraftInfo and vice versa
        void nonValidConvert();

    private:
        struct TestEntry
        {
            QString m_icaoEquipment;
            QString m_faaEquipment;
            QString m_aircraftIcao;
            CWakeTurbulenceCategory m_wtc;
            CComNavEquipment m_equipment;
            CSsrEquipment m_ssrEquipment;
        };

        QList<TestEntry> m_testEntries {
            { "B737/M-SDE2E3FGHIRWXY/LB1", "B737/L", "B737", CWakeTurbulenceCategory::MEDIUM,
              CComNavEquipment(CComNavEquipment::ComNavEquipment(
                                   CComNavEquipment::Standard | CComNavEquipment::Dme | CComNavEquipment::DFisAcars |
                                   CComNavEquipment::PdcAcars | CComNavEquipment::Adf | CComNavEquipment::Gnss |
                                   CComNavEquipment::HfRtf | CComNavEquipment::InertiaNavigation |
                                   CComNavEquipment::Pbn | CComNavEquipment::Rvsm | CComNavEquipment::Mnps |
                                   CComNavEquipment::Vhf833),
                               CComNavEquipment::CpdlcSatcomEquipment()),
              CSsrEquipment::SSrEquipment(CSsrEquipment::ModeSTypeL | CSsrEquipment::AdsBB1) },
            { "B748/H-SDE3FGHIM1M2RWXY/LB1", "H/B748/L", "B748", CWakeTurbulenceCategory::HEAVY,
              CComNavEquipment(CComNavEquipment::ComNavEquipment(
                                   CComNavEquipment::Standard | CComNavEquipment::Dme | CComNavEquipment::PdcAcars |
                                   CComNavEquipment::Adf | CComNavEquipment::Gnss | CComNavEquipment::HfRtf |
                                   CComNavEquipment::InertiaNavigation | CComNavEquipment::Pbn |
                                   CComNavEquipment::Rvsm | CComNavEquipment::Mnps | CComNavEquipment::Vhf833),
                               CComNavEquipment::CpdlcSatcomEquipment(CComNavEquipment::AtcSatvoiceInmarsat |
                                                                      CComNavEquipment::AtcSatvoiceMtsat)),
              CSsrEquipment::SSrEquipment(CSsrEquipment::ModeSTypeL | CSsrEquipment::AdsBB1) },
            { "C172/L-DGV/C", "C172/G", "C172", CWakeTurbulenceCategory::LIGHT,
              CComNavEquipment(CComNavEquipment::ComNavEquipment(CComNavEquipment::VhfRtf | CComNavEquipment::Gnss |
                                                                 CComNavEquipment::Dme),
                               CComNavEquipment::CpdlcSatcomEquipment()),
              CSsrEquipment::SSrEquipment(CSsrEquipment::ModeAC) },
            { "A388/J-SADE2E3FGHIJ3J4J5M1RWXY/LB1D1", "J/A388/L", "A388", CWakeTurbulenceCategory::SUPER,
              CComNavEquipment(CComNavEquipment::ComNavEquipment(
                                   CComNavEquipment::Standard | CComNavEquipment::Gbas | CComNavEquipment::Dme |
                                   CComNavEquipment::DFisAcars | CComNavEquipment::PdcAcars | CComNavEquipment::Adf |
                                   CComNavEquipment::Gnss | CComNavEquipment::HfRtf |
                                   CComNavEquipment::InertiaNavigation | CComNavEquipment::Pbn |
                                   CComNavEquipment::Rvsm | CComNavEquipment::Mnps | CComNavEquipment::Vhf833),
                               CComNavEquipment::CpdlcSatcomEquipment(
                                   CComNavEquipment::CpdlcFansVdlA | CComNavEquipment::CpdlcFansVdl2 |
                                   CComNavEquipment::CpdlcFansSatcomInmarsat | CComNavEquipment::AtcSatvoiceInmarsat)),
              CSsrEquipment::SSrEquipment(CSsrEquipment::ModeSTypeL | CSsrEquipment::AdsBB1 | CSsrEquipment::AdsCD1) }
        };
    };

    void CTestFlightPlanAircraftInfo::icaoConvert()
    {
        for (const TestEntry &entry : m_testEntries)
        {
            CFlightPlanAircraftInfo info(entry.m_icaoEquipment);

            QVERIFY2(info.getAircraftIcao().hasDesignator(), "Should have designator");
            QVERIFY2(info.getAircraftIcao().getDesignator() == entry.m_aircraftIcao,
                     "Should have same aircraft ICAO code");
            QCOMPARE(info.getWtc(), entry.m_wtc);
            QCOMPARE(info.getComNavEquipment(), entry.m_equipment);
            QCOMPARE(info.getSsrEquipment(), entry.m_ssrEquipment);
            QCOMPARE(info.asIcaoString(), entry.m_icaoEquipment);
        }
    }

    void CTestFlightPlanAircraftInfo::faaConvert()
    {
        for (const TestEntry &entry : m_testEntries)
        {
            CFlightPlanAircraftInfo info(entry.m_faaEquipment);

            QVERIFY2(info.getAircraftIcao().hasDesignator(), "Should have designator");
            QCOMPARE(info.getAircraftIcao().getDesignator(), entry.m_aircraftIcao);

            // FAA code only contains information about heavy and super wake turbulence category
            if (entry.m_wtc.isCategory(CWakeTurbulenceCategory::HEAVY) ||
                entry.m_wtc.isCategory(CWakeTurbulenceCategory::SUPER))
            {
                QCOMPARE(info.getWtc(), entry.m_wtc);
            }
            else { QVERIFY2(info.getWtc().isUnknown(), "Should have UNKNOWN wake turbulence category"); }

            // Cannot compare COM/NAV equipment and SSR equipment as the FAA code does not contain that much detail

            QCOMPARE(info.asFaaString(), entry.m_faaEquipment);
        }
    }

    void CTestFlightPlanAircraftInfo::nonValidConvert()
    {
        // Missing WTC and equipment code
        CFlightPlanAircraftInfo info("A388");
        QCOMPARE(info.asFaaString(), "A388/X");
        QCOMPARE(info.asIcaoString(), "A388/M-S/N");

        // FAA format: Missing equipment code
        info = CFlightPlanAircraftInfo("J/A388");
        QCOMPARE(info.asFaaString(), "J/A388/X");
        QCOMPARE(info.asIcaoString(), "A388/J-S/N");

        // FAA format: Wrong aircraft ICAO (but correct length according to CAircraftIcaoCode::isValidDesignator)
        info = CFlightPlanAircraftInfo("H/A1/W");
        QCOMPARE(info.asFaaString(), "H/A1/W");
        QCOMPARE(info.asIcaoString(), "A1/H-W/C");

        // FAA format: Missing aircraft ICAO with WTC
        info = CFlightPlanAircraftInfo("H//W");
        QCOMPARE(info.asFaaString(), "H//W");
        QCOMPARE(info.asIcaoString(), "/H-W/C");

        // FAA format: Equipment code only
        info = CFlightPlanAircraftInfo("/W");
        QCOMPARE(info.asFaaString(), "/W");
        QCOMPARE(info.asIcaoString(), "/M-W/C");

        // Wrong aircraft ICAO (too short) without WTC and equipment
        info = CFlightPlanAircraftInfo("X");
        QCOMPARE(info.asFaaString(), "/X");
        QCOMPARE(info.asIcaoString(), "/M-S/N");

        // Wrong aircraft ICAO (too long) without WTC and equipment
        info = CFlightPlanAircraftInfo("ABCDEFGHIJKL");
        QCOMPARE(info.asFaaString(), "/X");
        QCOMPARE(info.asIcaoString(), "/M-S/N");

        // Empty
        info = CFlightPlanAircraftInfo("");
        QCOMPARE(info.asFaaString(), "/X");
        QCOMPARE(info.asIcaoString(), "/M-S/N");

        // FAA format: Lower case (all)
        info = CFlightPlanAircraftInfo("h/b744/L");
        QCOMPARE(info.asFaaString(), "H/B744/L");
        QCOMPARE(info.asIcaoString(), "B744/H-GW/C");

        // FAA format: Lower case without WTC
        info = CFlightPlanAircraftInfo("b738/w");
        QCOMPARE(info.asFaaString(), "B738/W");
        QCOMPARE(info.asIcaoString(), "B738/M-W/C");

        // Lower case without WTC and equipment
        info = CFlightPlanAircraftInfo("dh8d");
        QCOMPARE(info.asFaaString(), "DH8D/X");
        QCOMPARE(info.asIcaoString(), "DH8D/M-S/N");

        // FAA format: Invalid WTC
        info = CFlightPlanAircraftInfo("Q/A346");
        QCOMPARE(info.asFaaString(), "A346/X");
        QCOMPARE(info.asIcaoString(), "A346/M-S/N");

        // FAA format: Leading whitespace
        info = CFlightPlanAircraftInfo(" H/B748/L");
        QCOMPARE(info.asFaaString(), "H/B748/L");
        QCOMPARE(info.asIcaoString(), "B748/H-GW/C");

        // FAA format: Trailing whitespace
        info = CFlightPlanAircraftInfo("H/B748/L ");
        QCOMPARE(info.asFaaString(), "H/B748/L");
        QCOMPARE(info.asIcaoString(), "B748/H-GW/C");

        // FAA format: Whitespaces in between
        info = CFlightPlanAircraftInfo("H / B7 48 /L");
        QCOMPARE(info.asFaaString(), "H/B748/L");
        QCOMPARE(info.asIcaoString(), "B748/H-GW/C");

        // ICAO format: Invalid WTC
        info = CFlightPlanAircraftInfo("A339/?-S/N");
        QCOMPARE(info.asFaaString(), "A339/X");
        QCOMPARE(info.asIcaoString(), "A339/M-S/N");

        // ICAO format: Missing SSR equipment code
        info = CFlightPlanAircraftInfo("A339/H-S");
        QCOMPARE(info.asFaaString(), "A339/X");
        QCOMPARE(info.asIcaoString(), "A339/M-S/N");

        // ICAO format: Lowercase
        info = CFlightPlanAircraftInfo("b737/m-sde2e3fghirwxy/lb1");
        QCOMPARE(info.asFaaString(), "B737/L");
        QCOMPARE(info.asIcaoString(), "B737/M-SDE2E3FGHIRWXY/LB1");

        // ICAO format: Leading whitespace
        info = CFlightPlanAircraftInfo(" B737/M-SDE2E3FGHIRWXY/LB1");
        QCOMPARE(info.asFaaString(), "B737/L");
        QCOMPARE(info.asIcaoString(), "B737/M-SDE2E3FGHIRWXY/LB1");

        // ICAO format: Trailing whitespace
        info = CFlightPlanAircraftInfo("B737/M-SDE2E3FGHIRWXY/LB1 ");
        QCOMPARE(info.asFaaString(), "B737/L");
        QCOMPARE(info.asIcaoString(), "B737/M-SDE2E3FGHIRWXY/LB1");

        // ICAO format: Whitespaces in between
        info = CFlightPlanAircraftInfo("B737/ M - SDE2E3FGH IRWXY / LB1");
        QCOMPARE(info.asFaaString(), "B737/L");
        QCOMPARE(info.asIcaoString(), "B737/M-SDE2E3FGHIRWXY/LB1");
    }

} // namespace MiscTest

//! main
SWIFTTEST_APPLESS_MAIN(MiscTest::CTestFlightPlanAircraftInfo);

#include "testflightplanaircraftinfo.moc"

//! \endcond
