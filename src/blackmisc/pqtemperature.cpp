#include "blackmisc/pqtemperature.h"

namespace BlackMisc {

/**
 * Specialized method for temperture
 */
double CTemperature::temperaturUnitConverter(const CPhysicalQuantity<CTemperatureUnit, CTemperature> *quantity, const CTemperatureUnit &otherUnit)
{
    CTemperature *me = (CTemperature*) quantity; // allow me access to protected
    if (me->m_unit  == otherUnit) return me->siBaseUnitValueToDouble();

    double siValue;
    // I always convert via SI Unit, other I would need too many conversions
    if(otherUnit == me->m_conversionSiUnit) {
        // here I expect a conversion to SI is required and not done yet
        if(me->m_unit == CTemperatureUnit::C()) {
            siValue = quantity->unitValueToDouble() + 273.15;
        } else if(me->m_unit == CTemperatureUnit::F()) {
            siValue = (me->unitValueToDouble() + 459.67) *5.0 / 9.0;
        } else{
            // TODO: EXCEPTION
        }
    } else {
        // here I expect the SI value is already set
        siValue = quantity->siBaseUnitValueToDouble();
    }

    // from SI
    if (otherUnit == me->m_conversionSiUnit) return siValue;
    if(otherUnit == CTemperatureUnit::C()) {
        return siValue - 273.15;
    } else if(me->m_unit == CTemperatureUnit::F()) {
        return (siValue * 9.0 / 5.0) - 459.67;
    }
    // TODO: Exception
    return 0;
}

} // namespace
