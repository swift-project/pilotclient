/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testblackcoremain.h"
#include "testinterpolator.h"
#include "testreaders.h"
#include "testnetwork.h"

namespace BlackCoreTest
{

    /*
     * Starting main, equivalent to QTEST_APPLESS_MAIN for multiple test classes.
     */
    int CBlackCoreTestMain::unitMain(int argc, char *argv[])
    {
        int status = 0;
        {
            CTestInterpolator interpolatorTests;
            status |= QTest::qExec(&interpolatorTests, argc, argv);
        }
        {
            CTestReaders readersTests;
            status |= QTest::qExec(&readersTests, argc, argv);
        }
        {
            CTestNetwork networkTests;
            status |= QTest::qExec(&networkTests, argc, argv);
        }
        return status;
    }
} // namespace
