/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testmain.h"

namespace BlackMiscTest
{
/*
 * Starting main, equivalent to QTEST_APPLESS_MAIN for multiple test classes.
 */
int CTestMain::unitMain(int argc, char *argv[])
{
    int status = 0;
    {
        CTestPhysicalQuantitiesBase pqBaseTests ;
        CTestAviationBase avBaseTests;
        status |= QTest::qExec(&pqBaseTests, argc, argv);
        status |= QTest::qExec(&avBaseTests, argc, argv);
    }
    return status;
}
}
