/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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
