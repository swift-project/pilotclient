#include "blackmisc/pqtemperature.h"

namespace BlackMisc {

/**
 * Default Constructor
 */
CTemperature::CTemperature(): CPhysicalQuantity(0, CTemperatureUnit::K(),CTemperatureUnit::K())
{
    //void
}

/**
 * Constructor
 */
CTemperature::CTemperature(const CPhysicalQuantity &temperature): CPhysicalQuantity(temperature)
{
    //void
}

/**
 * Constructor
 */
CTemperature::CTemperature(qint32 value, const CTemperatureUnit &unit) : CPhysicalQuantity(value, unit, CTemperatureUnit::K())
{
    // void
}

/**
 * Constructor
 */
CTemperature::CTemperature(double value, const CTemperatureUnit &unit) : CPhysicalQuantity(value, unit, CTemperatureUnit::K())
{
    // void
}

} // namespace BlackCore
