/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

#include "blackmisc/math/mathutils.h"
#include "blackmisc/pq/acceleration.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/constants.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/literals.h"
#include "blackmisc/pq/mass.h"
#include "blackmisc/pq/measurementunit.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/pq/pqstring.h"
#include "blackmisc/pq/pressure.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/temperature.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/variant.h"
#include "test.h"

#include <QString>
#include <QtGlobal>
#include <QTest>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Math;

namespace BlackMiscTest
{
    //! Physical quantities, basic tests
    class CTestPhysicalQuantities : public QObject
    {
        Q_OBJECT

    private slots:
        //! Basic unit tests for physical units
        void unitsBasics();

        //! Basic tests around length
        void lengthBasics();

        //! Basic tests about speed
        void speedBasics();

        //! Frequency tests
        void frequencyTests();

        //! Testing angles (degrees / radians)
        void angleTests();

        //! Testing mass
        void massTests();

        //! Testing pressure
        void pressureTests();

        //! Testing temperature
        void temperatureTests();

        //! Testing time
        void timeTests();

        //! Testing acceleration
        void accelerationTests();

        //! Testing construction / destruction in memory
        void memoryTests();

        //! Test parsing on PQs
        void parserTests();

        //! Basic arithmetic such as +/-
        void basicArithmetic();

        //! Test user-defined literals
        void literalsTest();
    };

    void CTestPhysicalQuantities::unitsBasics()
    {
        // some tests on units
        CLengthUnit du1 = CLengthUnit::m(); // Copy
        CLengthUnit du2 = CLengthUnit::m(); // Copy
        QVERIFY2(du1 == du2, "Compare by value 1");
        du1 = CLengthUnit::m(); // Copy
        du2 = CLengthUnit::m(); // Copy
        QVERIFY2(du1 == du2, "Compare by value 2");
        QVERIFY2(CLengthUnit::m() == CLengthUnit::m(), "Compare by value");

        CFrequencyUnit fu1 = CFrequencyUnit::Hz();
        QVERIFY2(fu1 != du1, "Hz must not be meter");

        // Unary expressions
        CTemperature temp1(5, CTemperatureUnit::C());
        QVERIFY2(-temp1 == CTemperature(-5, CTemperatureUnit::C()), "Temperatures must be the same");
        CTemperature temp2(-8.7, CTemperatureUnit::K());
        QVERIFY2(-temp2 == CTemperature(8.7, CTemperatureUnit::K()), "Temperatures must be the same");

        // null comparisons
        const CLength null(CLength::null());
        const CLength nonNull(1, CLengthUnit::m());
        QVERIFY2(null == CLength::null(), "null is equal to null");
        QVERIFY2(!(null < CLength::null()), "null is equivalent to null");
        QVERIFY2(null != nonNull, "null is not equal to non-null");
        QVERIFY2((null < nonNull) != (null > nonNull), "null is ordered wrt non-null");
    }

    void CTestPhysicalQuantities::lengthBasics()
    {
        CLength d1(1, CLengthUnit::m()); // 1m
        CLength d2(100, CLengthUnit::cm());
        CLength d3(1.852 * 1000, CLengthUnit::m()); // 1852m
        CLength d4(1, CLengthUnit::NM());
        QVERIFY2(d1 == d2, "1meter shall be 100cm");
        QVERIFY2(d3 == d4, "1852meters shall be 1NM");
        QVERIFY2(d1 * 2 == 2 * d1, "Commutative multiplication");

        d3 *= 2; // SI value
        d4 *= 2.0; // SI value !
        QVERIFY2(d3 == d4, "2*1852meters shall be 2NM");

        // less / greater
        QVERIFY2(!(d1 < d2), "Nothing shall be less / greater");
        QVERIFY2(!(d1 > d2), "Nothing shall be less / greater");

        // epsilon tests
        d1 = d2; // both in same unit
        d1.addValueSameUnit(d1.getUnit().getEpsilon() / 2.0); // this should be still the same
        QVERIFY2(d1 == d2, "Epsilon: 100cm + epsilon shall be 100cm");
        QVERIFY2(!(d1 != d2), "Epsilon: 100cm + epsilon shall be still 100cm");
        QVERIFY2(!(d1 > d2), "d1 shall not be greater");

        d1.addValueSameUnit(d1.getUnit().getEpsilon()); // now over epsilon threshold
        QVERIFY2(d1 != d2, "Epsilon exceeded: 100 cm + 2 epsilon shall not be 100cm");
        QVERIFY2(d1 > d2, "d1 shall be greater");
    }

    void CTestPhysicalQuantities::speedBasics()
    {
        CSpeed s1(100, CSpeedUnit::km_h());
        CSpeed s2(1000, CSpeedUnit::ft_min());
        QVERIFY2(CMathUtils::epsilonEqual(s1.valueRounded(CSpeedUnit::NM_h(), 0), 54), qPrintable(QStringLiteral("100km/h is not %1 NM/h").arg(s1.valueRounded(CSpeedUnit::NM_h(), 0))));
        QVERIFY2(CMathUtils::epsilonEqual(s2.valueRounded(CSpeedUnit::m_s(), 1), 5.1), qPrintable(QStringLiteral("1000ft/min is not %1 m/s").arg(s2.valueRounded(CSpeedUnit::m_s(), 1))));
    }

    void CTestPhysicalQuantities::frequencyTests()
    {
        CFrequency f1(1, CFrequencyUnit::MHz());
        QCOMPARE(f1.valueRounded(CFrequencyUnit::kHz(), 2), 1000.0); // "Mega is 1000kHz
        QCOMPARE(f1.value(), 1.0); // 1MHz
        QCOMPARE(f1.value(CFrequencyUnit::defaultUnit()), 1000000.0); // 1E6 Hz
        CFrequency f2(1e+6, CFrequencyUnit::Hz()); // 1 Megahertz
        QVERIFY2(f1 == f2, "MHz is 1E6 Hz");
    }

    void CTestPhysicalQuantities::angleTests()
    {
        CAngle a1(180, CAngleUnit::deg());
        CAngle a2(1.5 * CAngle::PI(), CAngleUnit::rad());
        CAngle a3(35.4336, CAngleUnit::sexagesimalDeg()); // 35.72666
        CAngle a4(35.436, CAngleUnit::sexagesimalDegMin()); // 35.72666
        CAngle a5(-60.3015, CAngleUnit::sexagesimalDeg()); // negative angles = west longitude or south latitude
        a2.switchUnit(CAngleUnit::deg());
        QVERIFY2(CMathUtils::epsilonEqual(a1.piFactor(), 1.00), qPrintable(QStringLiteral("Pi should be 1PI, not %1").arg(a1.piFactor())));
        QVERIFY2(CMathUtils::epsilonEqual(a3.valueRounded(CAngleUnit::deg()), 35.73), "Expecting 35.73");
        QVERIFY2(CMathUtils::epsilonEqual(a4.valueRounded(CAngleUnit::deg()), 35.73), "Expecting 35.73");
        QVERIFY2(CMathUtils::epsilonEqual(a5.valueRounded(CAngleUnit::deg(), 4), -60.5042), "Expecting -60.5042");

        // issue #552
        CAngle a6(48.07063588, CAngleUnit::sexagesimalDeg());
        CAngle a7(a6);
        a7.switchUnit(CAngleUnit::rad());
        QVERIFY2(CMathUtils::epsilonEqual(a6.value(CAngleUnit::sexagesimalDeg()), a7.value(CAngleUnit::sexagesimalDeg())), "Conversion via radians yields same answer");
        CAngle a8(48.07063588, CAngleUnit::sexagesimalDegMin());
        CAngle a9(a8);
        a9.switchUnit(CAngleUnit::rad());
        QVERIFY2(CMathUtils::epsilonEqual(a8.value(CAngleUnit::sexagesimalDegMin()), a9.value(CAngleUnit::sexagesimalDegMin())), "Conversion via radians yields same answer");
    }

    void CTestPhysicalQuantities::massTests()
    {
        CMass w1(1000, CMassUnit::kg());
        CMass w2(w1.value(), CMassUnit::kg());
        w2.switchUnit(CMassUnit::tonne());
        QCOMPARE(w2.value(), 1.0); // 1tonne shall be 1000kg
        w2.switchUnit(CMassUnit::lb());
        QVERIFY2(CMathUtils::epsilonEqual(w2.valueRounded(2), 2204.62), "1tonne shall be 2204pounds");
        QVERIFY2(w1 == w2, "Masses shall be equal");
    }

    void CTestPhysicalQuantities::pressureTests()
    {
        const CPressure p1(CPhysicalQuantitiesConstants::ISASeaLevelPressure());
        const CPressure p2(29.92, CPressureUnit::inHg());
        CPressure p4(p1);
        p4.switchUnit(CPressureUnit::mbar());

        // does not match exactly
        QVERIFY2(p1 != p2, "Standard pressure test little difference");
        QCOMPARE(p1.value(), p4.value()); // mbar/hPa test

        // Unit substract test
        const CPressure seaLevelPressure(918.0, CPressureUnit::mbar());
        const CPressure standardPressure(1013.2, CPressureUnit::mbar());
        const CPressure delta = (standardPressure - seaLevelPressure);
        const double expected = 95.2;
        const double deltaV = delta.value(CPressureUnit::mbar());
        QCOMPARE(deltaV, expected);
    }

    void CTestPhysicalQuantities::temperatureTests()
    {
        CTemperature t1(0, CTemperatureUnit::C()); // 0C
        CTemperature t2(1, CTemperatureUnit::F()); // 1F
        CTemperature t3(220.15, CTemperatureUnit::F());
        CTemperature t4(10, CTemperatureUnit::F());
        QVERIFY2(CMathUtils::epsilonEqual(t1.valueRounded(CTemperatureUnit::K()), 273.15), qPrintable(QStringLiteral("0C shall be 273.15K, not %1 K").arg(t1.valueRounded(CTemperatureUnit::K()))));
        QVERIFY2(CMathUtils::epsilonEqual(t2.valueRounded(CTemperatureUnit::C()), -17.22), qPrintable(QStringLiteral("1F shall be -17.22C, not %1 C").arg(t2.valueRounded(CTemperatureUnit::C()))));
        QVERIFY2(CMathUtils::epsilonEqual(t3.valueRounded(CTemperatureUnit::C()), 104.53), qPrintable(QStringLiteral("220.15F shall be 104.53C, not %1 C").arg(t3.valueRounded(CTemperatureUnit::C()))));
        QVERIFY2(CMathUtils::epsilonEqual(t4.valueRounded(CTemperatureUnit::K()), 260.93), qPrintable(QStringLiteral("10F shall be 260.93K, not %1 K").arg(t4.valueRounded(CTemperatureUnit::K()))));
    }

    void CTestPhysicalQuantities::timeTests()
    {
        CTime t1(1, CTimeUnit::h());
        CTime t2(1.5, CTimeUnit::h());
        CTime t3(1.25, CTimeUnit::min());
        CTime t4(1.0101, CTimeUnit::hms());
        CTime t5(26, 35, 40);
        CTime t6(3661, CTimeUnit::s());
        CTime t7;
        QVERIFY2(CMathUtils::epsilonEqual(t1.value(CTimeUnit::defaultUnit()), 3600), "1hour shall be 3600s");
        QVERIFY2(CMathUtils::epsilonEqual(t2.value(CTimeUnit::hrmin()), 1.3), "1.5hour shall be 1h30m");
        QVERIFY2(CMathUtils::epsilonEqual(t3.value(CTimeUnit::minsec()), 1.15), "1.25min shall be 1m15s");
        QVERIFY2(CMathUtils::epsilonEqual(t4.value(CTimeUnit::s()), 3661), "1h01m01s shall be 3661s");
        QVERIFY2(CMathUtils::epsilonEqual(t5.value(CTimeUnit::s()), 95740), "Time greater than 24h failed");
        QVERIFY2(t6.formattedHrsMinSec() == "01:01:01", "Formatted output hh:mm:ss failed");
        QVERIFY2(t6.formattedHrsMin() == "01:01", "Formatted output hh:mm failed");
        t6.switchUnit(CTimeUnit::hms());
        QVERIFY2(CMathUtils::epsilonEqual(t6.value(), 1.0101), "Switching the unit produced a wrong a value");
        t7.parseFromString("27:30:55");
        QVERIFY2(t7.formattedHrsMinSec() == "27:30:55", "Parsed time greater than 24h failed");

        CTime t8(7680, CTimeUnit::s());
        t8.switchUnit(CTimeUnit::hrmin());
        qDebug() << t8.valueRoundedWithUnit();
        QVERIFY2(t8.valueRoundedWithUnit() == "02h08.0m", "valueRoundedWithUnit in hrmin correctly formatted");
    }

    void CTestPhysicalQuantities::accelerationTests()
    {
        CLength oneMeter(1, CLengthUnit::m());
        double ftFactor = oneMeter.switchUnit(CLengthUnit::ft()).value();

        CAcceleration a1(10.0, CAccelerationUnit::m_s2());
        CAcceleration a2(a1);
        a1.switchUnit(CAccelerationUnit::ft_s2());
        QVERIFY2(a1 == a2, "Accelerations should be similar");
        QVERIFY2(CMathUtils::epsilonEqual(BlackMisc::Math::CMathUtils::round(a2.value() * ftFactor, 6),
                                          a1.valueRounded(6)),
                 "Numerical values should be equal");
    }

    void CTestPhysicalQuantities::memoryTests()
    {
        CLength *c = new CLength(100, CLengthUnit::m());
        c->switchUnit(CLengthUnit::NM());
        QVERIFY2(c->getUnit() == CLengthUnit::NM() && CLengthUnit::defaultUnit() == CLengthUnit::m(),
                 "Testing distance units failed");
        delete c;

        CAngle *a = new CAngle(100, CAngleUnit::rad());
        a->switchUnit(CAngleUnit::deg());
        QVERIFY2(a->getUnit() == CAngleUnit::deg() && CAngleUnit::defaultUnit() == CAngleUnit::deg(),
                 "Testing angle units failed");
        delete a;
    }

    void CTestPhysicalQuantities::parserTests()
    {
        QVERIFY2(CLength(33.0, CLengthUnit::ft()) == CLength("33.0 ft"), "Length");
        QVERIFY2(CLength(33.0, CLengthUnit::ft()) != CLength("33.1 ft"), "Length !=");
        QVERIFY2(CLength(-22.8, CLengthUnit::ft()) != CLength("-22.8 cm"), "Length !=");
        QVERIFY2(CSpeed(123.45, CSpeedUnit::km_h()) == CSpeed("123.45km/h"), "Speed");
        QVERIFY2(CMass(33.45, CMassUnit::kg()) == CMass("33.45000 kg"), "CMass");

        // parsing via variant
        CSpeed parsedPq1 = CPqString::parseToVariant("100.123 km/h").value<CSpeed>();
        QVERIFY2(CSpeed(100.123, CSpeedUnit::km_h()) == parsedPq1, "Parsed speed via variant");

        CLength parsedPq2 = CPqString::parseToVariant("-33.123ft").value<CLength>();
        QVERIFY2(CLength(-33.123, CLengthUnit::ft()) == parsedPq2, "Parsed length via variant");

        CFrequency parsedPq3 = CPqString::parse<CFrequency>("122.8MHz");
        QVERIFY2(CFrequency(122.8, CFrequencyUnit::MHz()) == parsedPq3, "Parsed frequency via variant");
    }

    void CTestPhysicalQuantities::basicArithmetic()
    {
        // pressure
        CPressure p1 = CPhysicalQuantitiesConstants::ISASeaLevelPressure();
        CPressure p2(p1);
        p2 *= 2.0;
        CPressure p3 = p1 + p1;
        QVERIFY2(p3 == p2, "Pressure needs to be the same (2times)");
        p3 /= 2.0;
        QVERIFY2(p3 == p1, "Pressure needs to be the same (1time)");
        p3 = p3 - p3;
        QCOMPARE(p3.value() + 1, 1.0); // Value needs to be zero
        p3 = CPressure(1013, CPressureUnit::hPa());
        QVERIFY2(p3 * 1.5 == 1.5 * p3, "Basic commutative test on PQ failed");

        // the time clasas
        CTime time1;
        time1.parseFromString("11:30"); // hhmm

        CTime time2;
        time2.parseFromString("-11:30"); // hhmm

        CTime time3 = time1 + time2;
        QVERIFY2(time3.isZeroEpsilonConsidered(), "Time must be 0");

        // angle
        CAngle a1(180, CAngleUnit::deg());
        CAngle a2(1.5 * CAngle::PI(), CAngleUnit::rad());
        a1 += a2;
        QVERIFY2(a1.valueInteger(CAngleUnit::deg()) == 450, "Expect 450 degrees");
    }

    void CTestPhysicalQuantities::literalsTest()
    {
        using namespace BlackMisc::PhysicalQuantities::Literals;

        // Acceleration
        QVERIFY2(12.5_ft_s2 == CAcceleration(12.5, CAccelerationUnit::ft_s2()), "Acceleration needs to be the same");
        QVERIFY2(5.7_m_s2 == CAcceleration(5.7, CAccelerationUnit::m_s2()), "Acceleration needs to be the same");
        QVERIFY2(48_ft_s2 == CAcceleration(48, CAccelerationUnit::ft_s2()), "Acceleration needs to be the same");
        QVERIFY2(39_m_s2 == CAcceleration(39, CAccelerationUnit::m_s2()), "Acceleration needs to be the same");

        // Angle
        QVERIFY2(179.7_deg == CAngle(179.7, CAngleUnit::deg()), "Angle needs to be the same");
        QVERIFY2(1.1_rad == CAngle(1.1, CAngleUnit::rad()), "Angle needs to be the same");
        QVERIFY2(180_deg == CAngle(180, CAngleUnit::deg()), "Angle needs to be the same");
        QVERIFY2(1_rad == CAngle(1, CAngleUnit::rad()), "Angle needs to be the same");

        // Frequency
        QVERIFY2(500.7_Hz == CFrequency(500.7, CFrequencyUnit::Hz()), "Frequency needs to be the same");
        QVERIFY2(99.7_kHz == CFrequency(99.7, CFrequencyUnit::kHz()), "Frequency needs to be the same");
        QVERIFY2(122.8_MHz == CFrequency(122.8, CFrequencyUnit::MHz()), "Frequency needs to be the same");
        QVERIFY2(4.1_GHz == CFrequency(4.1, CFrequencyUnit::GHz()), "Frequency needs to be the same");
        QVERIFY2(3000_Hz == CFrequency(3000, CFrequencyUnit::Hz()), "Frequency needs to be the same");
        QVERIFY2(122_kHz == CFrequency(122, CFrequencyUnit::kHz()), "Frequency needs to be the same");
        QVERIFY2(1333_MHz == CFrequency(1333, CFrequencyUnit::MHz()), "Frequency needs to be the same");
        QVERIFY2(5_GHz == CFrequency(5, CFrequencyUnit::GHz()), "Frequency needs to be the same");

        // Length
        QVERIFY2(79.24_m == CLength(79.24, CLengthUnit::m()), "Length needs to be the same");
        QVERIFY2(1024.2_NM == CLength(1024.2, CLengthUnit::NM()), "Length needs to be the same");
        QVERIFY2(78.1_SM == CLength(78.1, CLengthUnit::SM()), "Length needs to be the same");
        QVERIFY2(99.9_cm == CLength(99.9, CLengthUnit::cm()), "Length needs to be the same");
        QVERIFY2(147.2_ft == CLength(147.2, CLengthUnit::ft()), "Length needs to be the same");
        QVERIFY2(450.4_km == CLength(450.4, CLengthUnit::km()), "Length needs to be the same");
        QVERIFY2(136.2_mi == CLength(136.2, CLengthUnit::mi()), "Length needs to be the same");
        QVERIFY2(100_m == CLength(100, CLengthUnit::m()), "Length needs to be the same");
        QVERIFY2(320_NM == CLength(320, CLengthUnit::NM()), "Length needs to be the same");
        QVERIFY2(145_SM == CLength(145, CLengthUnit::SM()), "Length needs to be the same");
        QVERIFY2(200_cm == CLength(200, CLengthUnit::cm()), "Length needs to be the same");
        QVERIFY2(38000_ft == CLength(38000, CLengthUnit::ft()), "Length needs to be the same");
        QVERIFY2(100_km == CLength(100, CLengthUnit::km()), "Length needs to be the same");
        QVERIFY2(78_mi == CLength(78, CLengthUnit::mi()), "Length needs to be the same");

        // Mass
        QVERIFY2(142.1_g == CMass(142.1, CMassUnit::g()), "Mass needs to be the same");
        QVERIFY2(2048.123_kg == CMass(2048.123, CMassUnit::kg()), "Mass needs to be the same");
        QVERIFY2(8.2_lb == CMass(8.2, CMassUnit::lb()), "Mass needs to be the same");
        QVERIFY2(60.5_tonne == CMass(60.5, CMassUnit::tonne()), "Mass needs to be the same");
        QVERIFY2(45.3_shortTon == CMass(45.3, CMassUnit::shortTon()), "Mass needs to be the same");
        QVERIFY2(100_g == CMass(100, CMassUnit::g()), "Mass needs to be the same");
        QVERIFY2(2048_kg == CMass(2048, CMassUnit::kg()), "Mass needs to be the same");
        QVERIFY2(100_lb == CMass(100, CMassUnit::lb()), "Mass needs to be the same");
        QVERIFY2(30_tonne == CMass(30, CMassUnit::tonne()), "Mass needs to be the same");
        QVERIFY2(10_shortTon == CMass(10, CMassUnit::shortTon()), "Mass needs to be the same");

        // Pressure
        QVERIFY2(36.111_Pa == CPressure(36.111, CPressureUnit::Pa()), "Pressure needs to be the same");
        QVERIFY2(45.4_bar == CPressure(45.4, CPressureUnit::bar()), "Pressure needs to be the same");
        QVERIFY2(67.8_hPa == CPressure(67.8, CPressureUnit::hPa()), "Pressure needs to be the same");
        QVERIFY2(129.2_psi == CPressure(129.2, CPressureUnit::psi()), "Pressure needs to be the same");
        QVERIFY2(20.2_inHg == CPressure(20.2, CPressureUnit::inHg()), "Pressure needs to be the same");
        QVERIFY2(1013.13_mbar == CPressure(1013.13, CPressureUnit::mbar()), "Pressure needs to be the same");
        QVERIFY2(22.92_mmHg == CPressure(22.92, CPressureUnit::mmHg()), "Pressure needs to be the same");
        QVERIFY2(40_Pa == CPressure(40, CPressureUnit::Pa()), "Pressure needs to be the same");
        QVERIFY2(16_bar == CPressure(16, CPressureUnit::bar()), "Pressure needs to be the same");
        QVERIFY2(67_hPa == CPressure(67, CPressureUnit::hPa()), "Pressure needs to be the same");
        QVERIFY2(129_psi == CPressure(129, CPressureUnit::psi()), "Pressure needs to be the same");
        QVERIFY2(20_inHg == CPressure(20, CPressureUnit::inHg()), "Pressure needs to be the same");
        QVERIFY2(1013_mbar == CPressure(1013, CPressureUnit::mbar()), "Pressure needs to be the same");
        QVERIFY2(22_mmHg == CPressure(22, CPressureUnit::mmHg()), "Pressure needs to be the same");

        // Speed
        QVERIFY2(450.2_kts == CSpeed(450.2, CSpeedUnit::kts()), "Speed needs to be the same");
        QVERIFY2(5.7_m_s == CSpeed(5.7, CSpeedUnit::m_s()), "Speed needs to be the same");
        QVERIFY2(470.1_NM_h == CSpeed(470.1, CSpeedUnit::NM_h()), "Speed needs to be the same");
        QVERIFY2(20.45_ft_s == CSpeed(20.45, CSpeedUnit::ft_s()), "Speed needs to be the same");
        QVERIFY2(115.16_km_h == CSpeed(115.16, CSpeedUnit::km_h()), "Speed needs to be the same");
        QVERIFY2(160.5_ft_min == CSpeed(160.5, CSpeedUnit::ft_min()), "Speed needs to be the same");
        QVERIFY2(1020_kts == CSpeed(1020, CSpeedUnit::kts()), "Speed needs to be the same");
        QVERIFY2(20_m_s == CSpeed(20, CSpeedUnit::m_s()), "Speed needs to be the same");
        QVERIFY2(500_NM_h == CSpeed(500, CSpeedUnit::NM_h()), "Speed needs to be the same");
        QVERIFY2(400_ft_s == CSpeed(400, CSpeedUnit::ft_s()), "Speed needs to be the same");
        QVERIFY2(161_km_h == CSpeed(161, CSpeedUnit::km_h()), "Speed needs to be the same");
        QVERIFY2(4000_ft_min == CSpeed(4000, CSpeedUnit::ft_min()), "Speed needs to be the same");

        // Temperature
        QVERIFY2(-15.2_degC == CTemperature(-15.2, CTemperatureUnit::C()), "Temperature needs to be the same");
        QVERIFY2(68.1_degF == CTemperature(68.1, CTemperatureUnit::F()), "Temperature needs to be the same");
        QVERIFY2(232.2_degK == CTemperature(232.2, CTemperatureUnit::K()), "Temperature needs to be the same");
        QVERIFY2(35_degC == CTemperature(35, CTemperatureUnit::C()), "Temperature needs to be the same");
        QVERIFY2(-20_degF == CTemperature(-20, CTemperatureUnit::F()), "Temperature needs to be the same");
        QVERIFY2(140_degK == CTemperature(140, CTemperatureUnit::K()), "Temperature needs to be the same");

        // Time
        QVERIFY2(24.5_d == CTime(24.5, CTimeUnit::d()), "Time needs to be the same");
        QVERIFY2(23.99_h == CTime(23.99, CTimeUnit::h()), "Time needs to be the same");
        QVERIFY2(30.1_s == CTime(30.1, CTimeUnit::s()), "Time needs to be the same");
        QVERIFY2(20.0_ms == CTime(20.0, CTimeUnit::ms()), "Time needs to be the same");
        QVERIFY2(65.7_min == CTime(65.7, CTimeUnit::min()), "Time needs to be the same");
        QVERIFY2(53215.0_hms == CTime(53215, CTimeUnit::hms()), "Time needs to be the same");
        QVERIFY2(236.0_hrmin == CTime(236, CTimeUnit::hrmin()), "Time needs to be the same");
        QVERIFY2(3030.0_minsec == CTime(3030, CTimeUnit::minsec()), "Time needs to be the same");
        QVERIFY2(31_d == CTime(31, CTimeUnit::d()), "Time needs to be the same");
        QVERIFY2(24_h == CTime(24, CTimeUnit::h()), "Time needs to be the same");
        QVERIFY2(60_s == CTime(60, CTimeUnit::s()), "Time needs to be the same");
        QVERIFY2(40_ms == CTime(40, CTimeUnit::ms()), "Time needs to be the same");
        QVERIFY2(36_min == CTime(36, CTimeUnit::min()), "Time needs to be the same");
        QVERIFY2(102010_hms == CTime(102010, CTimeUnit::hms()), "Time needs to be the same");
        QVERIFY2(510_hrmin == CTime(510, CTimeUnit::hrmin()), "Time needs to be the same");
        QVERIFY2(2637_minsec == CTime(2637, CTimeUnit::minsec()), "Time needs to be the same");
    }
} // namespace

//! main
BLACKTEST_APPLESS_MAIN(BlackMiscTest::CTestPhysicalQuantities);

#include "testphysicalquantities.moc"

//! \endcond
