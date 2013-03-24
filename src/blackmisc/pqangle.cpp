#include "blackmisc/pqangle.h"

namespace BlackMisc {

/**
 * Default constructor
 */
CAngle::CAngle() : CPhysicalQuantity(0, CAngleUnit::rad(), CAngleUnit::rad())
{
    // void
}

/**
 * Constructor
 */
CAngle::CAngle(const CPhysicalQuantity &angle): CPhysicalQuantity(angle)
{
    //void
}

/**
 * Constructor
 */
CAngle::CAngle(qint32 value, const CAngleUnit &unit) : CPhysicalQuantity(value, unit, CAngleUnit::rad())
{
    // void
}

/**
 * Constructor
 */
CAngle::CAngle(double value, const CAngleUnit &unit) : CPhysicalQuantity(value, unit, CAngleUnit::rad())
{
    // void
}

/**
 * Destructor
 */
CAngle::~CAngle()
{
    // void
}

} // namespace
