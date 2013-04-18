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

    CCoordinateGeodetic gc(10.0, 20.0, 1000);
    qDebug() << gc;

    // bye
    qDebug() << "-----------------------------------------------";
    return 0;

}

} // namespace
