/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "samplesgeo.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/stringutils.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;

namespace BlackSample
{
    int CSamplesGeo::samples(QTextStream &out)
    {
        // comparions https://www.fcc.gov/general/degrees-minutes-seconds-tofrom-decimal-degrees
        const int digits = 12;
        const QString latStr("N 48° 7′ 6.3588"); // 48.118433
        const QString lngStr("E 16° 33′ 39.924");
        out.setRealNumberPrecision(digits);
        CCoordinateGeodetic geo = CCoordinateGeodetic::fromWgs84(latStr, lngStr, CAltitude(100, CAltitude::MeanSeaLevel, CLengthUnit::m()));
        CLatitude lat = CLatitude::fromWgs84(latStr);
        CLongitude lng = CLongitude::fromWgs84(lngStr);

        // check if conversions to xyz have messed something up
        CLatitude deltaLat = geo.latitude() - lat;
        CLongitude deltaLng = geo.longitude() - lng;

        out << latStr << " " << lngStr << endl;
        out <<
            lat.value(CAngleUnit::deg()) << " " << lat.value(CAngleUnit::sexagesimalDeg())  << " " <<
            lng.value(CAngleUnit::deg()) << " " << lng.value(CAngleUnit::sexagesimalDeg()) << endl;
        out <<
            geo.latitude().value(CAngleUnit::deg()) << " " << geo.latitude().value(CAngleUnit::sexagesimalDeg()) << " " <<
            geo.longitude().value(CAngleUnit::deg()) << " " << geo.longitude().value(CAngleUnit::sexagesimalDeg()) << endl;

        out << deltaLat.valueRoundedWithUnit(digits) << " " << deltaLng.valueRoundedWithUnit(digits) << endl;

        // equal test
        out << "Equal? " <<
            BlackMisc::boolToYesNo(lat == geo.latitude()) << " "  <<
            BlackMisc::boolToYesNo(lng == geo.longitude()) << endl;

        // check if conversions to xyz have messed something up
        QVector3D geoVector = geo.normalVector();
        CCoordinateGeodetic geo2(geoVector);
        deltaLat = geo2.latitude() - lat;
        deltaLng = geo2.longitude() - lng;

        out << deltaLat.valueRoundedWithUnit(digits) << " " << deltaLng.valueRoundedWithUnit(digits) << endl;

        // bye
        out << "-----------------------------------------------" << endl;
        return 0;
    }
} // namespace
