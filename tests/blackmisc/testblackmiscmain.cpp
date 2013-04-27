/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testblackmiscmain.h"

namespace BlackMiscTest
{
/*
 * Starting main, equivalent to QTEST_APPLESS_MAIN for multiple test classes.
 */
int CBlackMiscTestMain::unitMain(int argc, char *argv[])
{
    int status = 0;
    {
        CTestPhysicalQuantities pqBaseTests ;
        CTestAviation avBaseTests;
        CTestVectorMatrix vmTests;
        CTestGeo geoTests;
        status |= QTest::qExec(&pqBaseTests, argc, argv);
        status |= QTest::qExec(&avBaseTests, argc, argv);
        status |= QTest::qExec(&vmTests, argc, argv);
        status |= QTest::qExec(&geoTests, argc, argv);
    }
    return status;
}
} // namespace
