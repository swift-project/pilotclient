// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemiscquantities

#include "samplesgeo.h"
#include "misc/aviation/altitude.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/geo/earthangle.h"
#include "misc/geo/latitude.h"
#include "misc/geo/longitude.h"
#include "misc/pq/physicalquantity.h"
#include "misc/pq/units.h"
#include "misc/stringutils.h"

#include <QString>
#include <QTextStream>
#include <QVector3D>

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::geo;

namespace swift::sample
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

        out << latStr << " " << lngStr << Qt::endl;
        out << lat.value(CAngleUnit::deg()) << " " << lat.value(CAngleUnit::sexagesimalDeg()) << " " << lng.value(CAngleUnit::deg()) << " " << lng.value(CAngleUnit::sexagesimalDeg()) << Qt::endl;
        out << geo.latitude().value(CAngleUnit::deg()) << " " << geo.latitude().value(CAngleUnit::sexagesimalDeg()) << " " << geo.longitude().value(CAngleUnit::deg()) << " " << geo.longitude().value(CAngleUnit::sexagesimalDeg()) << Qt::endl;

        out << deltaLat.valueRoundedWithUnit(digits) << " " << deltaLng.valueRoundedWithUnit(digits) << Qt::endl;

        // equal test
        out << "Equal? " << swift::misc::boolToYesNo(lat == geo.latitude()) << " " << swift::misc::boolToYesNo(lng == geo.longitude()) << Qt::endl;

        // check if conversions to xyz have messed something up
        QVector3D geoVector = geo.normalVector();
        CCoordinateGeodetic geo2(geoVector);
        deltaLat = geo2.latitude() - lat;
        deltaLng = geo2.longitude() - lng;
        out << deltaLat.valueRoundedWithUnit(digits) << " " << deltaLng.valueRoundedWithUnit(digits) << Qt::endl;

        // Heading/bearing of same values
        CAngle bearing = geo.calculateBearing(geo);
        CLength distance = geo.calculateGreatCircleDistance(geo);
        out << bearing.valueRoundedWithUnit(CAngleUnit::deg(), 2) << " " << distance.valueRoundedWithUnit(CLengthUnit::m(), 2) << Qt::endl;

        const CCoordinateGeodetic nullCoordinate;
        bearing = geo.calculateBearing(nullCoordinate);
        distance = geo.calculateGreatCircleDistance(nullCoordinate);
        out << bearing.valueRoundedWithUnit(CAngleUnit::deg(), 2) << " " << distance.valueRoundedWithUnit(CLengthUnit::m(), 2) << Qt::endl;

        // bye
        out << "-----------------------------------------------" << Qt::endl;
        return 0;
    }
} // namespace
