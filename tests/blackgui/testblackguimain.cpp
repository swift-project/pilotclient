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
 * \ingroup testblackgui
 */

#include "testblackguimain.h"
#include "testutils.h"
#include "blackmisc/test.h"

#include <QStringList>
#include <QtTest>

namespace BlackGuiTest
{
    /*
     * Starting main, equivalent to QTEST_APPLESS_MAIN for multiple test classes.
     */
    int CBlackGuiTestMain::unitMain(int argc, char *argv[])
    {
        BlackMisc::CTest test(argc, argv);

        int status = 0;
        {
            CTestGuiUtilities utilityTests;
            status |= test.exec(&utilityTests, "blackgui_guiutilities");
        }
        return status;
    }
} // namespace

//! \endcond
