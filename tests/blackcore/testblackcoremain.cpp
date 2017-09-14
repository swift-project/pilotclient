/* Copyright (C) 2014
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
 * \ingroup testblackcore
 */

#include "testblackcoremain.h"
#include "testnetwork.h"
#include "testreaders.h"
#include "testcontext.h"
#include "testconnectivity.h"
#include "blackmisc/test/test.h"
#include <QStringList>
#include <QtTest>

namespace BlackCoreTest
{
    int CBlackCoreTestMain::unitMain(int argc, char *argv[])
    {
        BlackMisc::Test::CTest test(argc, argv);
        int status = 0;
        {
            CTestConnectivity connectivityTest;
            status |= test.exec(&connectivityTest, "blackcore_network");
        }
        {
            CTestContext contextTests;
            status |= test.exec(&contextTests, "blackcore_context");
        }
        {
            CTestReaders readersTests;
            status |= test.exec(&readersTests, "blackcore_readers");
        }
        {
            CTestNetwork networkTests;
            status |= test.exec(&networkTests, "blackcore_network");
        }
        return status;
    }
} // namespace

//! \endcond
