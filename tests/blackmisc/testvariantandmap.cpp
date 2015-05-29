/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testvariantandmap.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/propertyindexallclasses.h"
#include "blackmisc/tuple.h"
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
    void CTestVariantAndMap::variant()
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
        CLength l1(0, CLengthUnit::nullUnit());
        CLength l2(0, CLengthUnit::nullUnit());
        QVERIFY2(l1 == l2, "Null lengths should be equal");

        CLength l3(0, CLengthUnit::m());
        CLength l4(-1, CLengthUnit::m());
        QVERIFY2(l1 != l3, "Null length and non-null length should not be equal");
        QVERIFY2(l1 != l4, "Null length and non-null length should not be equal");
        QVERIFY2(!(l1 < l4), "Null length and non-null length should not be comparable");
        QVERIFY2(!(l1 > l4), "Null length and non-null length should not be comparable");
        QVERIFY2(compare(l1, l4) < 0, "Null length and non-null length should be sortable");

        CVariant station1qv = CVariant::fromValue(station1);
        QVERIFY2(station1 == station1, "Station should be equal");

        QVERIFY(station1.getController() == station2.getController());
        QVERIFY(station1.getDistanceToOwnAircraft() == station2.getDistanceToOwnAircraft());

        QVERIFY2(station1 == station2, "Station should be equal");
        QVERIFY2(station1 != station3, "Station should not be equal");
        QVERIFY2(station1qv == station1, "Station should be equal (CVariant)");
        QVERIFY2(station1 == station1qv, "Station should be equal (CVariant)");
        QVERIFY2(station2 == station1qv, "Station should be equal (CVariant)");
        QVERIFY2(station3 != station1qv, "Station should be equal (CVariant)");

        QVERIFY2(compare(station1, station1) == 0, "Station should be equal");
        QVERIFY2(compare(station1, station2) == 0, "Station should be equal");
        QVERIFY2(compare(station1, station3) != 0, "Station should not be equal");
    }

    /*
     * Variant map tests
     */
    void CTestVariantAndMap::valueMap()
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
        CPropertyIndexVariantMap vmWildcard(true);
        CPropertyIndexVariantMap vmNoWildcard(false);
        CPropertyIndexVariantMap vm;
        CPropertyIndexVariantMap vmCopy(vmWildcard);

        // Remark: Shortcoming here, as the callsign will be automatically set for user in station
        // I have to set this as well, otherwise, no match.
        vm.addValue(CAtcStation::IndexController, CUser("123456", "Joe Doe", CCallsign("EDDM_TWR")));

        // compare

        QVERIFY2(vmWildcard == station1, "Station should be equal to wildcard");
        QVERIFY2(station1 != vmNoWildcard, "Station should not be equal to empty list");
        QVERIFY2(station1 == vm, "Controller should match");
        QVERIFY2(vmWildcard == vmCopy, "Maps should be equal");
        QVERIFY2(qHash(vmWildcard) == qHash(vmCopy), "Hashs should be equal (simple)");

        vm.addValue(CAtcStation::IndexFrequency, CFrequency(118.7, CFrequencyUnit::MHz()));
        vm.addValue(CAtcStation::IndexPosition, geoPos);
        vmCopy = vm;
        QVERIFY2(qHash(vm) == qHash(vmCopy), "Hashs should be equal (detailed)");
        vmCopy.setWildcard(!vm.isWildcard());
        QVERIFY2(qHash(vm) != qHash(vmCopy), "Hashs should not be equal (detailed)");
    }

} // namespace
