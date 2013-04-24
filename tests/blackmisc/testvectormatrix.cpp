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
    CVector3D v1(1);
    v1 *= 2.0;
    CVector3D v2(2);
    QVERIFY2(v1 == v2, "Vectors should be equal");
    QVERIFY2(v1 * 2 == 2 * v2, "Commutative vector multiplication failed");
    CVector3D v3(1, 1, 1);
    CVector3D v4(2, 2, 2);
    CVector3D v5 = v3.crossProduct(v4);
    QVERIFY2(v5.isZeroEpsilon(), "Cross product shall be {0, 0, 0}");
    CVector3D v6(1, 2, 3);
    CVector3D v7(3, 4, 5);
    QVERIFY2(v6.crossProduct(v7) == CVector3D(-2, 4, -2), "Cross product is wrong");
    QVERIFY2(v6.dotProduct(v7) == 26, "Dot product is wrong, 26 expected");
}

/*
 * Matrix tests
 * http://www.bluebit.gr/matrix-calculator/
 */
void CTestVectorMatrix::matrixBasics()
{
    CMatrix3x3 m1;
    CMatrix3x3 m2 = m1 - m1;
    QVERIFY2(m1.isIdentity(), "Default matrix should be identity");
    QVERIFY2(m2.isZero(), "Matrix should be zero");

    bool invertible;
    CMatrix3x3 mr;
    CMatrix3x3 msame(3.0);
    mr = msame.inverse(invertible);
    QVERIFY2(!invertible, "Matrix with sigular values cannot be inverted");

    mr.setToIdentity();
    QVERIFY2(mr.determinant() == 1, "Identity matrix should have determinant 1");
    mr = mr.inverse(invertible);
    QVERIFY2(invertible && mr.isIdentity() == 1, "Identity matrix should have inverse identity");

    m1.setToIdentity();
    m2 = m1 + m1;
    m1 = m1 * 2.0;
    QVERIFY2(m1 == m2, "2* Identity should be Identity + Identity");
    QVERIFY2(m1 * 2 == 2 * m1, "Commutative matrix multiplication failed");

    m1 /= 2.0;
    m2 -= m1;
    QVERIFY2(m1 == m2, "Identity should be Identity");

    m1 = CMatrix3x3(1, 5, 3, 2, 4, 7, 4, 6, 2);
    double det = m1.determinant();
    QVERIFY2(det == 74, qPrintable(QString("Determinant should be 74, but is: %1").arg(det)));
    QVERIFY2(m1(0, 0) == 1 && m1(1, 0) == 2 && m1(0, 2) == 3, "Index error");
    CMatrix3x3 mi = m1.inverse(invertible);
    CMatrix3x3 mid = m1 * mi;
    mid.round();
    QVERIFY2(mid.isIdentity(), qPrintable(QString("Multiply with inverse should be identity: %1").arg(mid.toQString())));
}

} // namespace
