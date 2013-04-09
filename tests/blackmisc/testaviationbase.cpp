/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testaviationbase.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMiscTest {

/*
 * Constructor
 */
CTestAviationBase::CTestAviationBase(QObject *parent): QObject(parent)
{
    // void
}

/*
 * Basic tests
 */
void CTestAviationBase::headingBasics()
{
    CHeading h1(180, true, CAngleUnit::deg());
    CHeading h2(180, false, CAngleUnit::deg());
    CHeading h3(181, true, CAngleUnit::deg());
    CAngle a1(200,CAngleUnit::deg());
    CHeading h4;
    h4 = h1;
    QVERIFY2(h1 != h2, "Magnetic and true heading are not the same");
    QVERIFY2(h1 < h3, "180deg are less than 181deg");
    QVERIFY2(a1 > h3, "200deg are more than 200deg");
    QVERIFY2(h4 == h1, "Values shall be equal");

    h1 -= h1;
    QVERIFY2(h1.unitValueToDouble() == 0, "Value shall be 0");

    // h4 = h1 + h2; does not work, because misleading
    h2 += h2; // add just angle
    QVERIFY2(h2.unitValueToDouble() == 360, "Value shall be 360");
}

/*
 * Vertical positions
 */
void CTestAviationBase::verticalPosition()
{
    CAviationVerticalPositions vp1 = CAviationVerticalPositions::fromAltitudeAndElevationInFt(10000.0, 3000.0);
    CAviationVerticalPositions vp2 = vp1;
    QVERIFY2(vp1== vp2, "Values shall be equal");
}

/*
 * COM and NAV units
 */
void CTestAviationBase::comAndNav()
{
    CComSystem c1 = CComSystem::getCom1System(122.8);
    CComSystem c2 = CComSystem::getCom2System(122.8);
    QVERIFY2(c1 != c2, "COM system shall not be equal");
    c1 = c2;
    QVERIFY2(c1 == c2, "COM system shall be equal");
    CFrequency f(100.0, CFrequencyUnit::MHz());
    QVERIFY2(!CComSystem::tryGetComSystem(c1, "no valid unit", f), "Expect not to get COM system");
    CNavSystem nav1;
    QVERIFY2(CNavSystem::tryGetNav1System(nav1, 110.0), "Expect NAV system");
    QVERIFY2(!CNavSystem::tryGetNav1System(nav1, 200.0), "Expect no NAV system");
}

/*
 * COM and NAV units
 */
void CTestAviationBase::transponder()
{
    CTransponder t1 = CTransponder::getStandardTransponder(7000, CTransponder::StateStandby);
    CTransponder t2 = t1;
    QVERIFY2(t1 == t2, "Transponders shall be equal");
    t2.setTransponderMode(CTransponder::ModeC);
    QVERIFY2(t1 != t2, "Transponders shall not be equal");
    QVERIFY2(!CTransponder::tryGetStandardTransponder(t1, 8888, CTransponder::StateStandby), "No valid transponder");
    QVERIFY2(!CTransponder::tryGetStandardTransponder(t1, 781, CTransponder::StateStandby), "No valid transponder");
    QVERIFY2(!CTransponder::tryGetStandardTransponder(t1, "0781", CTransponder::StateStandby), "No valid transponder");
    QVERIFY2(CTransponder::tryGetStandardTransponder(t1, "7211", CTransponder::StateStandby), "No valid transponder");
    QVERIFY2(!CTransponder::tryGetStandardTransponder(t1, "schnitzel", CTransponder::StateStandby), "No valid transponder");

}

} // namespace
