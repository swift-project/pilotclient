/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testphysicalquantities.h"

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMiscTest
{

/*
 * Constructor
 */
CTestPhysicalQuantities::CTestPhysicalQuantities(QObject *parent) : QObject(parent)
{
    // void
}

/*
 * Basic unit tests for physical units
 */
void CTestPhysicalQuantities::unitsBasics()
{
    QVERIFY2(CMeasurementPrefix::k() > CMeasurementPrefix::h(), "kilo > hecto");

    // some tests on units
    CLengthUnit du1 = CLengthUnit::m(); // Copy
    CLengthUnit du2 = CLengthUnit::m(); // Copy
    QVERIFY2(du1 == du2, "Compare by value 1");
    du1 = CLengthUnit::m(); // Copy
    du2 = CLengthUnit::m(); // Copy
    QVERIFY2(du1 == du2, "Compare by value 2");
    QVERIFY2(CLengthUnit::m() == CLengthUnit::m(), "Compare by value");
    QVERIFY2(CMeasurementPrefix::h() < CMeasurementPrefix::M(), "hecto < mega");

    CFrequencyUnit fu1 = CFrequencyUnit::Hz();
    QVERIFY2(fu1 != du1, "Hz must not be meter");
}

/*
 * Distance tests
 */
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
    d1.addUnitValue(d1.getUnit().getEpsilon()); // this should be still the same
    QVERIFY2(d1 == d2, "Epsilon: 1meter+epsilon shall be 100cm");
    QVERIFY2(!(d1 != d2), "Epsilon: 1meter+epsilon shall be 100cm");

    d1.addUnitValue(d1.getUnit().getEpsilon()); // now over epsilon threshold
    QVERIFY2(d1 != d2, "Epsilon exceeded: 1meter+2epsilon shall not be 100cm");
    QVERIFY2(d1 > d2, "d1 shall be greater");
}

/*
 * Unit tests for speed
 */
void CTestPhysicalQuantities::speedBasics()
{
    CSpeed s1(100, CSpeedUnit::km_h());
    CSpeed s2(1000, CSpeedUnit::ft_min());
    QVERIFY2(s1.valueRounded(CSpeedUnit::NM_h(), 0) == 54, qPrintable(QString("100km/h is not %1 NM/h").arg(s1.valueRounded(CSpeedUnit::NM_h(), 0))));
    QVERIFY2(s2.valueRounded(CSpeedUnit::m_s(), 1) == 5.1, qPrintable(QString("1000ft/min is not %1 m/s").arg(s2.valueRounded(CSpeedUnit::m_s(), 1))));
}

/*
 * Frequency unit tests
 */
void CTestPhysicalQuantities::frequencyTests()
{
    CFrequency f1(1, CFrequencyUnit::MHz());
    QVERIFY2(f1.valueRounded(CFrequencyUnit::kHz(), 2) == 1000, "Mega is 1000kHz");
    QVERIFY2(f1.unitValueToDouble() == 1 , "1MHz");
    QVERIFY2(f1.convertedSiValueToDouble() == 1000000 , "1E6 Hz");
    CFrequency f2(CMeasurementPrefix::M().toDouble(), CFrequencyUnit::Hz()) ; // 1 Megahertz
    QVERIFY2(f1 == f2 , "MHz is 1E6 Hz");
}

/*
 * Angle tests
 */
void CTestPhysicalQuantities::angleTests()
{
    CAngle a1(180, CAngleUnit::deg());
    CAngle a2(1.5 * CAngle::pi(), CAngleUnit::rad());
    CAngle a3(35.4336, CAngleUnit::sexagesimalDeg()); // 35.72666
    a2.switchUnit(CAngleUnit::deg());
    QVERIFY2(a2.unitValueToInteger() == 270, qPrintable(QString("1.5Pi should be 270deg, not %1 deg").arg(a2.unitValueToInteger())));
    QVERIFY2(a1.piFactor() == 1, qPrintable(QString("Pi should be 1PI, not %1").arg(a1.piFactor())));
    QVERIFY2(a3.valueRounded(CAngleUnit::deg()) == 35.73, "Expecting 35.73");
}

/*
 * Weight tests
 */
void CTestPhysicalQuantities::massTests()
{
    CMass w1(1000, CMassUnit::kg());
    CMass w2(w1.unitValueToInteger(), CMassUnit::kg());
    w2.switchUnit(CMassUnit::t());
    QVERIFY2(w2.unitValueToInteger() == 1, "1tonne shall be 1000kg");
    w2.switchUnit(CMassUnit::lb());
    QVERIFY2(w2.unitValueToDoubleRounded(2) == 2204.62, "1tonne shall be 2204pounds");
    QVERIFY2(w1 == w2, "Masses shall be equal");
}

/*
 * Pressure tests
 */
void CTestPhysicalQuantities::pressureTests()
{
    CPressure p1(1013.25, CPressureUnit::hPa());
    CPressure p2(29.92, CPressureUnit::inHg());
    CPressure p3(29.92, CPressureUnit::inHgFL());
    CPressure p4(p1);
    p4.switchUnit(CPressureUnit::mbar());

    // does not match exactly
    QVERIFY2(p1 != p2, "Standard pressure test little difference");
    QVERIFY2(p1 == p3, "Standard pressure test matching");
    QVERIFY2(p1.unitValueToDouble() == p4.unitValueToDouble(), "mbar/hPa test");
}

/*
 * Temperature tests
 */
void CTestPhysicalQuantities::temperatureTests()
{
    CTemperature t1(0, CTemperatureUnit::C()); // 0C
    CTemperature t2(1, CTemperatureUnit::F()); // 1F
    CTemperature t3(220.15, CTemperatureUnit::F());
    CTemperature t4(10, CTemperatureUnit::F());
    QVERIFY2(t1.convertedSiValueToDoubleRounded() == 273.15, qPrintable(QString("0C shall be 273.15K, not %1 K").arg(t1.convertedSiValueToDoubleRounded())));
    QVERIFY2(t2.valueRounded(CTemperatureUnit::C()) == -17.22, qPrintable(QString("1F shall be -17.22C, not %1 C").arg(t2.valueRounded(CTemperatureUnit::C()))));
    QVERIFY2(t3.valueRounded(CTemperatureUnit::C()) == 104.53, qPrintable(QString("220.15F shall be 104.53C, not %1 C").arg(t3.valueRounded(CTemperatureUnit::C()))));
    QVERIFY2(t4.valueRounded(CTemperatureUnit::K()) == 260.93, qPrintable(QString("10F shall be 260.93K, not %1 K").arg(t4.valueRounded(CTemperatureUnit::K()))));
}

/*
 * Temperature tests
 */
void CTestPhysicalQuantities::timeTests()
{
    CTime t1(1, CTimeUnit::h());
    QVERIFY2(t1.convertedSiValueToInteger() == 3600, "1hour shall be 3600s");
}

/*
 * Just testing obvious memory create / destruct flaws
 */
void CTestPhysicalQuantities::memoryTests()
{
    CLength *c = new CLength(100, CLengthUnit::m());
    c->switchUnit(CLengthUnit::NM());
    QVERIFY2(c->getUnit() == CLengthUnit::NM() && c->getConversionSiUnit() == CLengthUnit::m(),
             "Testing distance units failed");
    delete c;

    CAngle *a = new CAngle(100, CAngleUnit::rad());
    a->switchUnit(CAngleUnit::deg());
    QVERIFY2(a->getUnit() == CAngleUnit::deg() && c->getConversionSiUnit() == CAngleUnit::rad(),
             "Testing angle units failed");
    delete a;
}

/*
 * @brief Just testing obvious memory create / destruct flaws
 */
void CTestPhysicalQuantities::basicArithmetic()
{
    CPressure p1 = CPhysicalQuantitiesConstants::InternationalStandardSeaLevelPressure();
    CPressure p2(p1);
    p2 *= 2.0;
    CPressure p3 = p1 + p1;
    QVERIFY2(p3 == p2, "Pressure needs to be the same (2times)");
    p3 /= 2.0;
    QVERIFY2(p3 == p1, "Pressure needs to be the same (1time)");
    p3 = p3 - p3;
    QVERIFY2(p3.unitValueToDouble() == 0, "Value needs to be zero");
}
} // namespace
