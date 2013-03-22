#include "blackmisc/pqmass.h"

namespace BlackMisc {

/**
 * Default Constructor
 */
CMass::CMass(): CPhysicalQuantity(0, CMassUnit::kg(),CMassUnit::kg())
{
    //void
}

/**
 * Constructor
 */
CMass::CMass(const CPhysicalQuantity &weight): CPhysicalQuantity(weight)
{
    // void
}

/**
 * Constructor
 */
CMass::CMass(qint32 value, const CMassUnit &unit) : CPhysicalQuantity(value, unit, CMassUnit::kg())
{
    // void
}

/**
 * Constructor
 */
CMass::CMass(double value, const CMassUnit &unit) : CPhysicalQuantity(value, unit, CMassUnit::kg())
{
    // void
}

} // namespace BlackCore
