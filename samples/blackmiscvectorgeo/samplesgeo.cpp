/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "samplesgeo.h"

using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMiscTest
{

    /*
     * Samples
     */
    int CSamplesGeo::samples()
    {
        CLatitude lat1(20.0, CAngleUnit::deg());
        CLatitude lat2 = lat1;
        CLatitude lat3 = lat1 - lat2;

        qDebug() << lat1 << lat2 << lat3;
        qDebug() << (lat1 + lat2) << (lat1 - lat2);

        lat3 += lat1;
        CLongitude lon1(33.0, CAngleUnit::deg());
        qDebug() << lon1 << lat3;

//    lat3 += lon1; // must not work
//    lat3 = lon1; //must not work
//    CGeoLongitude lonx(lat2); // must notwork

        CCoordinateGeodetic cg(10.0, 20.0, 1000);
        CCoordinateEcef ce = CCoordinateTransformation::toEcef(cg);
        CCoordinateGeodetic cg2 = CCoordinateTransformation::toGeodetic(ce);
        cg2.switchUnit(CAngleUnit::deg());
        qDebug() << cg << ce << cg2;

        CCoordinateNed cned = CCoordinateTransformation::toNed(ce, cg);
        CCoordinateEcef ce2 = CCoordinateTransformation::toEcef(cned);
        qDebug() << ce << cned << ce2;
        qDebug() << (cned + cned) << (ce + ce);

//    cned += ce2; // must not work

        // bye
        qDebug() << "-----------------------------------------------";
        return 0;

    }

} // namespace
