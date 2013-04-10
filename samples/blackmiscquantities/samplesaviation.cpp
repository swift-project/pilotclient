#include "samplesaviation.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMiscTest
{

/**
 * Samples
 */
int CSamplesAviation::samples()
{
    // CSpeed s1(10, CSpeedUnit::kts());
    CHeading h1(180, true, CAngleUnit::deg());
    CHeading h2(180, false, CAngleUnit::deg());
    qDebug() << h1 << h2 << (h1 == h2) << (h1 != h2) << (h1 == h1);

    CAviationVerticalPositions vp1 = CAviationVerticalPositions::fromAltitudeAndElevationInFt(10000.0, 1111.0);
    CAviationVerticalPositions vp2 = vp1;
    qDebug() << vp1 << (vp1 == vp2) << (vp1 != vp2);

    CComSystem c1 = CComSystem::getCom1System(125.3);
    qDebug() << c1;
    c1.setActiveUnicom();
    qDebug() << c1;

    // CComSystem *c1p = new CComSystem("Test", CFrequency(125.3,CFrequencyUnit::MHz()));
    if (!CComSystem::tryGetComSystem(c1, "Test", -1.0))
        qDebug() << c1 << "is reset to default as expected";
    else
        qDebug() << "Something is wrong here";

    try {
        // uncomment to test assert
        // CFrequency f1(-1.0, CFrequencyUnit::MHz());
        // c1 = CComSystem("ups", f1, f1);
        // qDebug() << "Why do I get here??";
    } catch (std::range_error &ex) {
        qDebug() << "As expected" << ex.what();
    }

    CNavSystem nav1;
    CNavSystem::tryGetNav1System(nav1, 110.0);
    qDebug() << nav1;

    CTransponder tr1("T1", 7000, CTransponder::StateStandby);
    CTransponder tr2("T2", "4532", CTransponder::ModeMil3);
    qDebug() << tr1 << tr2;

    // bye
    qDebug() << "-----------------------------------------------";
    return 0;
}

} // namespace
