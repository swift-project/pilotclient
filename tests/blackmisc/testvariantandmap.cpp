/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/avatcstation.h"
#include "testvariantandmap.h"
#include <QList>
#include <QString>
#include <vector>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;

namespace BlackMiscTest
{
    /*
     * Variant tests
     */
    void CTestVariantAndValueMap::variant()
    {
        // ATC station
        QDateTime dtFrom = QDateTime::currentDateTimeUtc();
        QDateTime dtUntil = dtFrom.addSecs(60 * 60.0); // 1 hour
        QDateTime dtFrom2 = dtUntil;
        QDateTime dtUntil2 = dtUntil.addSecs(60 * 60.0);
        CCoordinateGeodetic geoPos =
            CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", CLength(1487, CLengthUnit::ft()));
        CAtcStation station1(CCallsign("eddm_twr"), CUser("123456", "Joe Doe"),
                             CFrequency(118.7, CFrequencyUnit::MHz()),
                             geoPos, CLength(50, CLengthUnit::km()), false, dtFrom, dtUntil);
        CAtcStation station2(station1);
        CAtcStation station3(CCallsign("eddm_app"), CUser("654321", "Jen Doe"),
                             CFrequency(120.7, CFrequencyUnit::MHz()),
                             geoPos, CLength(100, CLengthUnit::km()), false, dtFrom2, dtUntil2);

        // compare
        QVariant station1qv = QVariant::fromValue(station1);
        QVERIFY2(station1 == station1, "Station should be equal");
        QVERIFY2(station1 == station2, "Station should be equal");
        QVERIFY2(station1 != station3, "Station should not be equal");
        QVERIFY2(station1qv == station1, "Station should be equal (QVariant)");
        QVERIFY2(station1 == station1qv, "Station should be equal (QVariant)");
        QVERIFY2(station2 == station1qv, "Station should be equal (QVariant)");
        QVERIFY2(station3 != station1qv, "Station should be equal (QVariant)");
    }

    /*
     * Value map tests
     */
    void CTestVariantAndValueMap::valueMap()
    {
        // ATC station
        QDateTime dtFrom = QDateTime::currentDateTimeUtc();
        QDateTime dtUntil = dtFrom.addSecs(60 * 60.0); // 1 hour
        CCoordinateGeodetic geoPos =
            CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", CLength(1487, CLengthUnit::ft()));
        CAtcStation station1(CCallsign("eddm_twr"), CUser("123456", "Joe Doe"),
                             CFrequency(118.7, CFrequencyUnit::MHz()),
                             geoPos, CLength(50, CLengthUnit::km()), false, dtFrom, dtUntil);

        // value maps
        CValueMap vmWildcard(true);
        CValueMap vmNoWildcard(false);
        CValueMap vm;
        vm.addValue(CAtcStation::IndexController, CUser("123456", "Joe Doe"));

        // compare
        QVERIFY2(vmWildcard == station1, "Station should be equal to wildcard");
        QVERIFY2(station1 != vmNoWildcard, "Station should not be equal to empty list");
        QVERIFY2(station1 == vm, "Controller should match");

    }

} //namespace BlackMiscTest
