#include "blackmisc/pqmass.h"

namespace BlackMisc {

/**
 * Default Constructor
 */
CMass::CMass(): CPhysicalQuantity(0, CMassUnit::kg(),CMassUnit::kg())
{
    // void
}

/**
 * Constructor
 */
CMass::CMass(const CPhysicalQuantity &mass): CPhysicalQuantity(mass)
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

/**
 * Destructor
 */
CMass::~CMass()
{
    // void
}

} // namespace BlackMisc
