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
#include "testinterpolator.h"
#include "testnetwork.h"
#include "testreaders.h"
#include "blackmisc/test.h"

#include <QStringList>
#include <QtTest>

namespace BlackCoreTest
{
    int CBlackCoreTestMain::unitMain(int argc, char *argv[])
    {
        BlackMisc::CTest test(argc, argv);

        int status = 0;
        {
            CTestInterpolator interpolatorTests;
            status |= test.exec(&interpolatorTests, "blackcore_interpolator");
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
