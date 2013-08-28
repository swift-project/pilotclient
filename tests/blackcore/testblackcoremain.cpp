/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testblackcoremain.h"
#include "testinterpolator.h"
#include "testnetmediators.h"

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
        CTestNetMediators mediatorTests;
        status |= QTest::qExec(&mediatorTests, argc, argv);
    }
    return status;
}
} // namespace
