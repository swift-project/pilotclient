// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc

#include <QTest>

#include "test.h"

#include "misc/aviation/flightplan.h"
#include "misc/aviation/selcal.h"
#include "misc/network/voicecapabilities.h"
#include "misc/pq/literals.h"

using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::physical_quantities;

namespace MiscTest
{
    //! Flightplan unit tests
    class CTestFlightPlan : public QObject
    {
        Q_OBJECT

    private slots:
        //! Flight plan remarks (parsing)
        void flightPlanRemarks();

        //! Flight plan altitude
        void flightPlanAltitude();

        //! Import flightplan from vpilot vfp format
        void importVpilotFlightplan();
    };

    void CTestFlightPlan::flightPlanRemarks()
    {
        QString remarks;
        CFlightPlanRemarks fpRemarks(remarks);
        QVERIFY2(fpRemarks.isEmpty(), "Expect NULL flight plan remarks");

        remarks = QStringLiteral("RT/KESTREL OPR/MYTRAVEL REG/G-DAJC SEL/FP-ES PER/C NAV/RNP10");
        fpRemarks = CFlightPlanRemarks(remarks);
        QVERIFY2(fpRemarks.getSelcalCode().getCode() == "FPES", "Wrong SELCAL code");
        QVERIFY2(fpRemarks.getFlightOperator() == "MYTRAVEL", "Wrong flight operator");
        QVERIFY2(fpRemarks.getRegistration().asString() == "GDAJC", "Wrong registration");

        remarks = QStringLiteral("OPR/UAL CALLSIGN/UNITED");
        fpRemarks = CFlightPlanRemarks(remarks);
        QVERIFY2(fpRemarks.getAirlineIcao().getDesignator() == "UAL", "Wrong airline, expect UAL");
        QVERIFY2(fpRemarks.getFlightOperator().isEmpty(), "Expect to operator, should be in airline");
        QVERIFY2(fpRemarks.getRegistration().isEmpty(), "Expect no registration");
        QVERIFY2(fpRemarks.getRadioTelephony() == "UNITED", "Expect telephony");

        remarks = QStringLiteral("/v/FPL-VIR9-IS-A346/DEP/S-EGLL/ARR/KJFK/REG/G-VGAS/TCAS RVR/200 OPR/VIRGIN AIRLINES");
        fpRemarks = CFlightPlanRemarks(remarks);
        QVERIFY2(fpRemarks.getRegistration().asString() == "GVGAS", "Wrong registration");
        QVERIFY2(fpRemarks.getFlightOperator() == "VIRGIN AIRLINES", "Wrong operator");
        QVERIFY2(fpRemarks.getVoiceCapabilities().getCapabilities() == CVoiceCapabilities::Voice,
                 "Wrong airline, expect UAL");

        remarks =
            QStringLiteral("/v/FPL-VIR9-IS-A346/ OPR/VIRGIN AIRLINES DEP/S-EGLL/ARR/KJFK/REG/G-VGAS/TCAS RVR/200");
        fpRemarks = CFlightPlanRemarks(remarks);
        QVERIFY2(fpRemarks.getRegistration().asString() == "GVGAS", "Wrong registration");
        QVERIFY2(fpRemarks.getFlightOperator() == "VIRGIN AIRLINES", "Wrong operator");
        QVERIFY2(fpRemarks.getVoiceCapabilities().getCapabilities() == CVoiceCapabilities::Voice,
                 "Wrong airline, expect UAL");
    }

    void CTestFlightPlan::flightPlanAltitude()
    {
        CAltitude a;
        a.parseFromFpAltitudeString("FL125");
        QVERIFY2(a == CAltitude(12500, CAltitude::FlightLevel, CLengthUnit::ft()), "Wrong altitude FL125");
        a.parseFromFpAltitudeString("FL126");
        QVERIFY2(a == CAltitude(12600, CAltitude::FlightLevel, CLengthUnit::ft()), "Wrong altitude FL126");
        a.parseFromFpAltitudeString("A122");
        QVERIFY2(a == CAltitude(12200, CAltitude::MeanSeaLevel, CLengthUnit::ft()), "Wrong altitude A122");
        a.parseFromFpAltitudeString("123ft");
        QVERIFY2(a == CAltitude(123, CAltitude::MeanSeaLevel, CLengthUnit::ft()), "Wrong altitude 123ft");
        a.parseFromFpAltitudeString("1234m");
        QVERIFY2(a == CAltitude(1234, CAltitude::MeanSeaLevel, CLengthUnit::m()), "Wrong altitude 1234m");
        a.parseFromFpAltitudeString("S0666");
        QVERIFY2(a == CAltitude(6660, CAltitude::FlightLevel, CLengthUnit::m()), "Wrong altitude S0666");
        a.parseFromFpAltitudeString("M456");
        QVERIFY2(a == CAltitude(4560, CAltitude::MeanSeaLevel, CLengthUnit::m()), "Wrong altitude M456");

        // some values which are supposed to be wrong
        CAltitude faulty(a);
        faulty.makeNegative();
        QVERIFY2(!faulty.isValidFpAltitude(), "Negative values not allowed");

        // as string
        a = CAltitude(12500, CAltitude::FlightLevel, CLengthUnit::ft());
        QVERIFY2(a.asFpICAOAltitudeString() == "FL125", "Expect FL125");
        QVERIFY2(a.asFpVatsimAltitudeString() == "FL125", "Expect FL125");
        a = CAltitude(15000, CAltitude::MeanSeaLevel, CLengthUnit::ft());
        QVERIFY2(a.asFpICAOAltitudeString() == "A150", "Expect A150");
        // QVERIFY2(a.asFpVatsimAltitudeString() == "15000ft", "Expect 15000");
        QVERIFY2(a.asFpVatsimAltitudeString() == "15000", "Expect 15000");
        a = CAltitude(1500, CAltitude::FlightLevel, CLengthUnit::m());
        QVERIFY2(a.asFpICAOAltitudeString() == "S0150", "Expect S0150");
        a = CAltitude(1600, CAltitude::MeanSeaLevel, CLengthUnit::m());
        QVERIFY2(a.asFpICAOAltitudeString() == "M0160", "Expect M0160");
    }

    void CTestFlightPlan::importVpilotFlightplan()
    {
        using namespace swift::misc::physical_quantities::Literals;

        const CFlightPlan fp = CFlightPlan::fromVPilotFormat(QStringLiteral(
            "<?xml version=\"1.0\" encoding=\"utf-8\"?><FlightPlan "
            "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" "
            "FlightType=\"VFR\" Equipment=\"G\" CruiseAltitude=\"5500\" CruiseSpeed=\"115\" DepartureAirport=\"EDRY\" "
            "DestinationAirport=\"EDDL\" AlternateAirport=\"EDDK\" Route=\"WRB DOMUX\" "
            "Remarks=\"TEST remark\" IsHeavy=\"false\" EquipmentPrefix=\"\" EquipmentSuffix=\"\" "
            "DepartureTime=\"1744\" "
            "DepartureTimeAct=\"1744\" EnrouteHours=\"2\" EnrouteMinutes=\"28\" FuelHours=\"3\" FuelMinutes=\"45\" "
            "VoiceType=\"Full\" />"));

        QCOMPARE_EQ(fp.getFlightRules(), CFlightPlan::VFR);
        QCOMPARE_EQ(fp.getCruiseTrueAirspeed(), 115_kts);
        QCOMPARE_EQ(fp.getOriginAirportIcao().getIcaoCode(), "EDRY");
        QCOMPARE_EQ(fp.getDestinationAirportIcao().getIcaoCode(), "EDDL");
        QCOMPARE_EQ(fp.getAlternateAirportIcao().getIcaoCode(), "EDDK");
        QCOMPARE_EQ(fp.getRemarks(), "/V/ TEST remark");
        QCOMPARE_EQ(fp.getRoute(), "WRB DOMUX");
        QCOMPARE_EQ(fp.getFuelTime(), CTime(3, 45, 0));
        QCOMPARE_EQ(fp.getEnrouteTime(), CTime(2, 28, 0));
        QCOMPARE_EQ(fp.getCruiseAltitude().asFpVatsimAltitudeString(), "FL055");
        QCOMPARE_EQ(fp.getTakeoffTimePlannedHourMin(), "17:44");

        // Should not read aircraft type from vfp
        QCOMPARE_EQ(fp.getAircraftInfo().getAircraftIcao().getAircraftType(), "");
    }
} // namespace MiscTest

//! main
SWIFTTEST_APPLESS_MAIN(MiscTest::CTestFlightPlan);

#include "testflightplan.moc"

//! \endcond
