#include "testgeo.h"
#include "blackmisc/geolatitude.h"
#include "blackmisc/geolongitude.h"

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
    }

} // namespace
