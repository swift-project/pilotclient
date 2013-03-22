#include "blackmisc/pqspeed.h"

namespace BlackMisc {

/**
 * Default constructor
 */
CSpeed::CSpeed() : CPhysicalQuantity(0, CSpeedUnit::m_s(), CSpeedUnit::m_s())
{
    // void
}

/**
 * Constructor
 */
CSpeed::CSpeed(const CPhysicalQuantity &speed): CPhysicalQuantity(speed)
{
    //void
}

/**
 * Constructor
 */
CSpeed::CSpeed(qint32 value, const CSpeedUnit &unit) : CPhysicalQuantity(value, unit, CSpeedUnit::m_s())
{
    // void
}

/**
 * Constructor
 */
CSpeed::CSpeed(double value, const CSpeedUnit &unit) : CPhysicalQuantity(value, unit, CSpeedUnit::m_s())
{
    // void
}


} // namespace
