/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include "testgeo.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/geo/earthangle.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/pq/units.h"

#include <QTest>

using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Math;

namespace BlackMiscTest
{
    void CTestGeo::geoBasics()
    {
        CLatitude lati(10, CAngleUnit::deg());
        QVERIFY2(lati * 2 == lati + lati, "Latitude addition should be equal");
        lati += CLatitude(20, CAngleUnit::deg());
        QVERIFY2(lati.valueRounded() == 30.0, "Latitude should be 30 degrees");
    }

    void CTestGeo::coordinateGeodetic()
    {
        CCoordinateGeodetic northPole = { 90.0, 0.0 };
        CCoordinateGeodetic southPole = { -90.0, 0.0 };
        QCOMPARE(calculateEuclideanDistance(northPole, southPole), 2.0);
        CCoordinateGeodetic equator = { 0.0, 70.354683 };
        QCOMPARE(calculateEuclideanDistance(northPole, equator), std::sqrt(2.0f));
    }
} // namespace

//! \endcond
