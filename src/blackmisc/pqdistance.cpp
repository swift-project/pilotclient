#include "blackmisc/pqdistance.h"

namespace BlackMisc {

/**
 * Default Constructor
 */
CDistance::CDistance(): CPhysicalQuantity(0, CDistanceUnit::m(),CDistanceUnit::m())
{
    //void
}

/**
 * Constructor
 */
CDistance::CDistance(const CPhysicalQuantity &distance): CPhysicalQuantity(distance)
{
    //void
}

/**
 * Constructor
 */
CDistance::CDistance(qint32 value, const CDistanceUnit &unit) : CPhysicalQuantity(value, unit, CDistanceUnit::m())
{
    // void
}

/**
 * Constructor
 */
CDistance::CDistance(double value, const CDistanceUnit &unit) : CPhysicalQuantity(value, unit, CDistanceUnit::m())
{
    // void
}

} // namespace BlackCore
