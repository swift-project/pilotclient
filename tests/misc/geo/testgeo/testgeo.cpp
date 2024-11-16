// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc

#include <QTest>

#include "test.h"

#include "misc/geo/coordinategeodetic.h"
#include "misc/geo/earthangle.h"
#include "misc/geo/latitude.h"
#include "misc/pq/physicalquantity.h"
#include "misc/pq/units.h"

using namespace swift::misc::geo;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::math;

namespace MiscTest
{
    //! Geo classes tests
    class CTestGeo : public QObject
    {
        Q_OBJECT

    private slots:
        //! Basic unit tests for geo classes
        void geoBasics();

        //! CCoordinateGeodetic unit tests
        void coordinateGeodetic();
    };

    void CTestGeo::geoBasics()
    {
        CLatitude lat(10, CAngleUnit::deg());
        QVERIFY2(lat * 2 == lat + lat, "Latitude addition should be equal");
        lat += CLatitude(20, CAngleUnit::deg());
        QVERIFY2(lat.valueRounded() == 30.0, "Latitude should be 30 degrees");

        CAngle a(20, 0);
        lat = CLatitude(a);
        double v = lat.valueRounded(CAngleUnit::deg());
        QVERIFY2(v == 20.0, "Values shall be the same");

        a = CAngle(28, 0);
        lat = CLatitude(a);
        v = lat.valueRounded(CAngleUnit::deg());
        QVERIFY2(v == 28.0, "Values shall be the same");

        a = CAngle(30, 0, 0);
        lat = CLatitude(a);
        v = lat.valueRounded(CAngleUnit::deg());
        QVERIFY2(v == 30.0, "Values shall be the same");
    }

    void CTestGeo::coordinateGeodetic()
    {
        CCoordinateGeodetic northPole = { 90.0, 0.0 };
        CCoordinateGeodetic southPole = { -90.0, 0.0 };
        QCOMPARE(calculateEuclideanDistance(northPole, southPole), 2.0);
        CCoordinateGeodetic equator = { 0.0, 70.354683 };
        QCOMPARE(calculateEuclideanDistance(northPole, equator), std::sqrt(2.0f));

        CCoordinateGeodetic testCoordinate = northPole;
        double latValue = testCoordinate.latitude().value(CAngleUnit::deg());
        double lngValue = testCoordinate.longitude().value(CAngleUnit::deg());
        QVERIFY2(latValue == 90.0, "Latitude value supposed to be 90");
        QVERIFY2(lngValue == 0.0, "Longitude value supposed to be 0");
        CLatitude newLat(90.0, CAngleUnit::deg());
        testCoordinate.setLatitude(newLat);
        latValue = testCoordinate.latitude().value(CAngleUnit::deg());
        QCOMPARE(latValue, newLat.value(CAngleUnit::deg()));
    }
} // namespace MiscTest

//! main
SWIFTTEST_APPLESS_MAIN(MiscTest::CTestGeo);

#include "testgeo.moc"

//! \endcond
