/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

#include "testaviation.h"
#include "testcompress.h"
#include "testblackmiscmain.h"
#include "testcontainers.h"
#include "testdbus.h"
#include "testflightplan.h"
#include "testgeo.h"
#include "testaircraftparts.h"
#include "testicon.h"
#include "testidentifier.h"
#include "testinput.h"
#include "testinterpolatorlinear.h"
#include "testinterpolatorparts.h"
#include "testlibrarypath.h"
#include "testmath.h"
#include "testphysicalquantities.h"
#include "testprocess.h"
#include "testpropertyindex.h"
#include "testslot.h"
#include "teststringutils.h"
#include "testvaluecache.h"
#include "testvariantandmap.h"
#include "testweather.h"
#include "blackmisc/test/test.h"

#include <QStringList>
#include <QTest>

namespace BlackMiscTest
{
    /*
     * Starting main, equivalent to QTEST_APPLESS_MAIN for multiple test classes.
     */
    int CBlackMiscTestMain::unitMain(int argc, char *argv[])
    {
        BlackMisc::Test::CTest test(argc, argv);

        int status = 0;
        {
            CTestCompress compressTest;
            status |= test.exec(&compressTest, "blackmisc_compress");
        }
        {
            CTestPhysicalQuantities pqBaseTests;
            status |= test.exec(&pqBaseTests, "blackmisc_physicalQuantities");
        }
        {
            CTestAviation avBaseTests;
            status |= test.exec(&avBaseTests, "blackmisc_aviation");
        }
        {
            CTestPropertyIndex piTests;
            status |= test.exec(&piTests, "blackmisc_propertyindex");
        }
        {
            CTestAircraftParts apTests;
            status |= test.exec(&apTests, "blackmisc_aircraftParts");
        }
        {
            CTestGeo geoTests;
            status |= test.exec(&geoTests, "blackmisc_geo");
        }
        {
            CTestFlightPlan fpTests;
            status |= test.exec(&fpTests, "blackmisc_flightplan");
        }
        {
            CTestContainers containerTests;
            status |= test.exec(&containerTests, "blackmisc_containers");
        }
        {
            CTestVariantAndMap variantAndMap;
            status |= test.exec(&variantAndMap, "blackmisc_variantandmap");
        }
        {
            CTestInput inputTests;
            status |= test.exec(&inputTests, "blackmisc_input");
        }
        {
            CTestIdentifier identifierTests;
            status |= test.exec(&identifierTests, "blackmisc_identifier");
        }
        {
            CTestSlot slotTests;
            status |= test.exec(&slotTests, "blackmisc_slot");
        }
        {
            CTestStringUtils stringUtilTests;
            status |= test.exec(&stringUtilTests, "blackmisc_stringutils");
        }
        {
            CTestValueCache valueCacheTests;
            status |= test.exec(&valueCacheTests, "blackmisc_valuecache");
        }
        {
            CTestWeather weatherTests;
            status |= test.exec(&weatherTests, "blackmisc_weather");
        }
        {
            CTestMath mathTests;
            status |= test.exec(&mathTests, "blackmisc_math");
        }
        {
            CTestInterpolatorLinear interpolatorTests;
            status |= test.exec(&interpolatorTests, "blackmisc_interpolatorlinear");
        }
        {
            CTestInterpolatorParts interpolatorParts;
            status |= test.exec(&interpolatorParts, "blackmisc_interpolatorparts");
        }
        {
            CTestLibraryPath libraryPathTests;
            status |= test.exec(&libraryPathTests, "blackmisc_librarypath");
        }
        {
            CTestProcess processTests;
            status |= test.exec(&processTests, "blackmisc_process");
        }
        {
            CTestDBus testDBus;
            status |= test.exec(&testDBus, "blackmisc_dbus");
        }
        {
            CTestIcon testIcon;
            status |= test.exec(&testIcon, "blackmisc_icon");
        }
        return status;
    }
} // namespace

//! \endcond
