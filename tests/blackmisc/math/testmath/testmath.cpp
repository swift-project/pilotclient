/* Copyright (C) 2016
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
#include "test.h"

#include <QTest>

using namespace BlackMisc::Math;

namespace BlackMiscTest
{
    //! Math unit tests
    class CTestMath : public QObject
    {
        Q_OBJECT

    private slots:
        //! Unit test for round to multiple of
        void testRoundToMultipleOf();
    };

    void CTestMath::testRoundToMultipleOf()
    {
        QVERIFY2(CMathUtils::roundToMultipleOf(0, 3) == 0, "Nearest multiple of 3 from 0 should be 0");
        QVERIFY2(CMathUtils::roundToMultipleOf(1, 3) == 0, "Nearest multiple of 3 from 1 should be 0");
        QVERIFY2(CMathUtils::roundToMultipleOf(2, 3) == 3, "Nearest multiple of 3 from 2 should be 3");
        QVERIFY2(CMathUtils::roundToMultipleOf(3, 3) == 3, "Nearest multiple of 3 from 3 should be 3");

        QVERIFY2(CMathUtils::roundToMultipleOf(0, -3) == 0, "Nearest multiple of -3 from 0 should be 0");
        QVERIFY2(CMathUtils::roundToMultipleOf(1, -3) == 0, "Nearest multiple of -3 from 1 should be 0");
        QVERIFY2(CMathUtils::roundToMultipleOf(2, -3) == 3, "Nearest multiple of -3 from 2 should be 3");
        QVERIFY2(CMathUtils::roundToMultipleOf(3, -3) == 3, "Nearest multiple of -3 from 3 should be 3");

        QVERIFY2(CMathUtils::roundToMultipleOf(-1, 3) == 0, "Nearest multiple of 3 from -1 should be 0");
        QVERIFY2(CMathUtils::roundToMultipleOf(-2, 3) == -3, "Nearest multiple of 3 from -2 should be -3");
        QVERIFY2(CMathUtils::roundToMultipleOf(-3, 3) == -3, "Nearest multiple of 3 from -3 should be -3");

        QVERIFY2(CMathUtils::roundToMultipleOf(-1, -3) == 0, "Nearest multiple of -3 from -1 should be 0");
        QVERIFY2(CMathUtils::roundToMultipleOf(-2, -3) == -3, "Nearest multiple of -3 from -2 should be -3");
        QVERIFY2(CMathUtils::roundToMultipleOf(-3, -3) == -3, "Nearest multiple of -3 from -3 should be -3");
    }

} // namespace

//! main
BLACKTEST_APPLESS_MAIN(BlackMiscTest::CTestMath);

#include "testmath.moc"

//! \endcond
