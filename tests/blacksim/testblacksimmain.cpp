/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testblacksimmain.h"
#include "testsimcommon.h"

namespace BlackSimTest
{
    /*
     * Starting main, equivalent to QTEST_APPLESS_MAIN for multiple test classes.
     */
    int CBlackSimTestMain::unitMain(int argc, char *argv[])
    {
        int status = 0;
        {
            CTestFsCommon fscommon;
            status |= QTest::qExec(&fscommon, argc, argv);
        }
        return status;
    }
} // namespace
