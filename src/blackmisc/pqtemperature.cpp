#include "blackmisc/pqtemperature.h"

namespace BlackMisc {

/**
 * Default Constructor
 */
CTemperature::CTemperature(): CPhysicalQuantity(0, CTemperatureUnit::K(),CTemperatureUnit::K(), CTemperature::temperaturUnitConverter)
{
    // void
}

/**
 * Constructor
 */
CTemperature::CTemperature(const CPhysicalQuantity &temperature): CPhysicalQuantity(temperature)
{
    // void
}

/**
 * Constructor
 */
CTemperature::CTemperature(qint32 value, const CTemperatureUnit &unit) : CPhysicalQuantity(value, unit, CTemperatureUnit::K(), CTemperature::temperaturUnitConverter)
{
    // void
}

/**
 * Constructor
 */
CTemperature::CTemperature(double value, const CTemperatureUnit &unit) : CPhysicalQuantity(value, unit, CTemperatureUnit::K(), CTemperature::temperaturUnitConverter)
{
    // void
}

/**
 * Destructor
 */
CTemperature::~CTemperature()
{
    // void
}

/**
 * Specialized method for temperture
 */
double CTemperature::temperaturUnitConverter(const CPhysicalQuantity *quantity, const CMeasurementUnit &otherUnit)
{
    CTemperature *me = (CTemperature*) quantity; // allow me access to protected
    if (me->_pUnit  == otherUnit) return me->siBaseUnitValueToDouble();

    double siValue;
    // I always convert via SI Unit, other I would need too many conversions
    if(otherUnit == me->_pConversionSiUnit) {
        // here I expect a conversion to SI is required and not done yet
        if(me->_pUnit == CTemperatureUnit::C()) {
            siValue = quantity->unitValueToDouble() + 273.15;
        } else if(me->_pUnit == CTemperatureUnit::F()) {
            siValue = (me->unitValueToDouble() + 459.67) *5.0 / 9.0;
        } else{
            // TODO: EXCEPTION
        }
    } else {
        // here I expect the SI value is already set
        siValue = quantity->siBaseUnitValueToDouble();
    }

    // from SI
    if (otherUnit == me->_pConversionSiUnit) return siValue;
    if(otherUnit == CTemperatureUnit::C()) {
        return siValue - 273.15;
    } else if(me->_pUnit == CTemperatureUnit::F()) {
        return (siValue * 9.0 / 5.0) - 459.67;
    }
    // TODO: Exception
    return 0;
}

} // namespace BlackCore
