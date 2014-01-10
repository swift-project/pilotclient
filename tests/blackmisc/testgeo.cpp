#include "testgeo.h"

using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Math;


namespace BlackMiscTest
{

    /*
     * Geo classes tests
     */
    void CTestGeo::geoBasics()
    {
        CLatitude lati(10, CAngleUnit::deg());
        QVERIFY2(lati * 2 == lati + lati, "Latitude addition should be equal");
        lati += CLatitude(20, CAngleUnit::deg());
        QVERIFY2(lati.valueRounded() == 30.0, "Latitude should be 30 degrees");

        double lat = 27.999999, lon = 86.999999, h = 8820.999999; // Mt Everest
        CCoordinateGeodetic startGeoVec(lat, lon, h);
        CCoordinateEcef mediumEcefVec = CCoordinateTransformation::toEcef(startGeoVec);
        CCoordinateGeodetic endGeoVec = CCoordinateTransformation::toGeodetic(mediumEcefVec);

        // this == contains some implicit rounding, since it is based on PQs
        QVERIFY2(startGeoVec == endGeoVec, "Reconverted geo vector should be equal ");

        CCoordinateNed nedVec = CCoordinateTransformation::toNed(mediumEcefVec, startGeoVec);
        CCoordinateEcef ecefReconvert = CCoordinateTransformation::toEcef(nedVec);

        // check against rounded reconvert
        QVERIFY2(mediumEcefVec.rounded() == ecefReconvert.rounded(), "Reconverted geo vector should be equal");
    }

} // namespace
