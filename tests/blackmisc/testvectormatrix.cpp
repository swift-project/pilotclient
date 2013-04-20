/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testvectormatrix.h"

using namespace BlackMisc::Math;

namespace BlackMiscTest
{

/*
 * Basic tests vector
 */
void CTestVectorMatrix::vectorBasics()
{

}

/*
 * Matrix tests
 */
void CTestVectorMatrix::matrixBasics()
{
    CMatrix3x3 m1;
    CMatrix3x3 m2 = m1 - m1;
    QVERIFY2(m1.isIdentity(), "Default matrix should be identity");
    QVERIFY2(m2.isZero(), "Matrix should be zero");
}

} // namespace

#include "testvectormatrix.h"

