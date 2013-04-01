#include "samplesaviation.h"

namespace BlackMiscTest {

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

    // bye
    return 0;
}

} // namespace
