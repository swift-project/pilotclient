/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 */

#include "testblackguimain.h"
#include "testutils.h"

namespace BlackCoreTest
{
    /*
     * Starting main, equivalent to QTEST_APPLESS_MAIN for multiple test classes.
     */
    int CBlackGuiTestMain::unitMain(int argc, char *argv[])
    {
        int status = 0;
        {
            CTestGuiUtilities utilityTests;
            status |= QTest::qExec(&utilityTests, argc, argv);
        }
        return status;
    }
} // namespace

//! \endcond
