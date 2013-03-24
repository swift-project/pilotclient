#include "blackmisc/pqpressure.h"

namespace BlackMisc {

/**
 * Default constructor
 */
CPressure::CPressure() : CPhysicalQuantity(0, CPressureUnit::Pa(), CPressureUnit::Pa())
{
    // void
}

/**
 * Constructor
 */
CPressure::CPressure(const CPhysicalQuantity &pressure): CPhysicalQuantity(pressure)
{
    //void
}

/**
 * Constructor
 */
CPressure::CPressure(qint32 value, const CPressureUnit &unit) : CPhysicalQuantity(value, unit, CPressureUnit::Pa())
{
    // void
}

/**
 * Constructor
 */
CPressure::CPressure(double value, const CPressureUnit &unit) : CPhysicalQuantity(value, unit, CPressureUnit::Pa())
{
    // void
}

/**
 * Destructor
 */
CPressure::~CPressure()
{
    // void
}


} // namespace
