#include "blackmisc/pqtemperature.h"

namespace BlackMisc {

/**
 * Default Constructor
 */
CTemperature::CTemperature(): CPhysicalQuantity(0, CTemperatureUnit::K(),CTemperatureUnit::K())
{
    this->setUnitValue(this->unitValueToDouble()); // I have to recall, since virtual method overriding does not work in constructor
}

/**
 * Constructor
 */
CTemperature::CTemperature(const CPhysicalQuantity &temperature): CPhysicalQuantity(temperature)
{
    this->setUnitValue(this->unitValueToDouble()); // I have to recall, since virtual method overriding does not work in constructor
}

/**
 * Constructor
 */
CTemperature::CTemperature(qint32 value, const CTemperatureUnit &unit) : CPhysicalQuantity(value, unit, CTemperatureUnit::K())
{
    this->setUnitValue(value); // I have to recall, since virtual method overriding does not work in constructor
}

/**
 * Constructor
 */
CTemperature::CTemperature(double value, const CTemperatureUnit &unit) : CPhysicalQuantity(value, unit, CTemperatureUnit::K())
{
    this->setUnitValue(value); // I have to recall, since virtual method overriding does not work in constructor
}

/**
 * Overriden specializedmethodfortemperture
 */
double CTemperature::calculateValueInOtherUnit(const CMeasurementUnit &otherUnit) const
{
    if (this->getUnit()==otherUnit) return this->siBaseUnitValueToDouble();

    double siValue;
    // I always convert via SI Unit, other I would need too many conversions
    if(otherUnit == this->_conversionSiUnit) {
        // here I expect a conversion to SI is required and not done yet
        if(this->_unit == CTemperatureUnit::C()) {
            siValue = this->unitValueToDouble() + 273.15;
        } else if(this->_unit == CTemperatureUnit::F()) {
            siValue = (this->unitValueToDouble() + 459.67) *5.0 / 9.0;
        } else{
            // TODO: EXCEPTION
        }
    } else {
        // here I expect the SI value is already set
        siValue = this->siBaseUnitValueToDouble();
    }

    // from SI
    if (otherUnit == this->_conversionSiUnit) return siValue;
    if(otherUnit == CTemperatureUnit::C()) {
        return siValue - 273.15;
    } else if(this->_unit == CTemperatureUnit::F()) {
        return (siValue * 9.0 / 5.0) - 459.67;
    }
    // TODO: Exception
    return 0;
}

} // namespace BlackCore
