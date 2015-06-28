/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testphysicalquantities.h"
#include "testaviation.h"
#include "testgeo.h"
#include "testidentifier.h"
#include "testcontainers.h"
#include "testvariantandmap.h"
#include "testhardware.h"
#include "testvaluecache.h"
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
            CTestGeo geoTests;
            CTestContainers containerTests;
            CTestVariantAndMap variantAndMap;
            CTestHardware hardwareTests;
            CTestIdentifier identifierTests;
            CTestValueCache valueCacheTests;
            status |= QTest::qExec(&pqBaseTests, argc, argv);
            status |= QTest::qExec(&avBaseTests, argc, argv);
            status |= QTest::qExec(&geoTests, argc, argv);
            status |= QTest::qExec(&identifierTests, argc, argv);
            status |= QTest::qExec(&containerTests, argc, argv);
            status |= QTest::qExec(&variantAndMap, argc, argv);
            status |= QTest::qExec(&hardwareTests, argc, argv);
            status |= QTest::qExec(&valueCacheTests, argc, argv);
        }
        return status;
    }
} // namespace
