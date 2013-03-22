#include "blackmisc/pqbase.h"

namespace BlackMisc {

// -----------------------------------------------------------------------
// --- Mulitplier --------------------------------------------------------
// -----------------------------------------------------------------------

/**
 * Constructor
 */
CMeasurementPrefix::CMeasurementPrefix(const QString &name, const QString &unitName, double factor):
    _name(name),_prefix(unitName),_factor(factor)
{
    // void
}

/**
 * Constructor
 */
CMeasurementPrefix::CMeasurementPrefix(const CMeasurementPrefix &otherMultiplier) :
    _name(otherMultiplier._name), _prefix(otherMultiplier._prefix), _factor(otherMultiplier._factor)
{
    // void
}

/**
 * Assignment operator
 */
CMeasurementPrefix& CMeasurementPrefix::operator=(const CMeasurementPrefix &otherMultiplier) {

    if (this == &otherMultiplier) return *this; // Same object? Yes, so skip assignment, and just return *this
    this->_name = otherMultiplier._name;
    this->_prefix=otherMultiplier._prefix;
    this->_factor=otherMultiplier._factor;
    return *this;
}

/**
 * Equal?
 */
bool CMeasurementPrefix::operator ==(const CMeasurementPrefix &otherMultiplier) const
{
    if ( this == &otherMultiplier ) return true;
    return this->_factor == otherMultiplier._factor && this->_name == otherMultiplier._name;
}

/**
 * Not equal
 */
bool CMeasurementPrefix::operator !=(const CMeasurementPrefix &otherMultiplier) const
{
    return !(*this == otherMultiplier);
}

/**
 * Greater?
 */
bool CMeasurementPrefix::operator >(const CMeasurementPrefix &otherMultiplier) const
{
    return this->_factor > otherMultiplier._factor;
}

/**
 * Less?
 */
bool CMeasurementPrefix::operator <(const CMeasurementPrefix &otherMultiplier) const
{
    return this->_factor < otherMultiplier._factor;
}

/**
 * Stream to debug
 */
QDebug operator<<(QDebug d, const CMeasurementPrefix &multiplier)
{
    d << multiplier._name;
    return d;
}

/**
 * Log to debug
 */
CLogMessage operator<<(CLogMessage log, const CMeasurementPrefix &multiplier)
{
    log << multiplier._name;
    return log;
}

// -----------------------------------------------------------------------
// --- Measurement unit --------------------------------------------------
// -----------------------------------------------------------------------

/**
 * Constructor
 */
CMeasurementUnit::CMeasurementUnit(const QString &name, const QString &unitName, const QString &type, bool isSIUnit, bool isSIBaseUnit, double conversionFactorToSI, const CMeasurementPrefix &multiplier, qint32 displayDigits, double epsilon):
    _name(name), _unitName(unitName), _type(type), _isSIUnit(isSIUnit), _isSIBaseUnit(isSIBaseUnit),_displayDigits(displayDigits),_conversionFactorToSIConversionUnit(conversionFactorToSI),
    _epsilon(epsilon), _multiplier(multiplier)
{
    // void
}

/*
 * Copy constructor
 */
CMeasurementUnit::CMeasurementUnit(const CMeasurementUnit &otherUnit):
    _name(otherUnit._name), _unitName(otherUnit._unitName), _type(otherUnit._type), _isSIUnit(otherUnit._isSIUnit),
    _isSIBaseUnit(otherUnit._isSIBaseUnit), _displayDigits(otherUnit._displayDigits),_conversionFactorToSIConversionUnit(otherUnit._conversionFactorToSIConversionUnit),
    _epsilon(otherUnit._epsilon), _multiplier(otherUnit._multiplier)
{
    // void
}

/**
 * Assigment operator
 */
CMeasurementUnit &CMeasurementUnit::operator =(const CMeasurementUnit &otherUnit)
{
    if (this == &otherUnit) return *this; // Same object? Yes, so skip assignment, and just return *this
    this->_name = otherUnit._name;
    this->_unitName =otherUnit._unitName;
    this->_type=otherUnit._type;
    this->_isSIUnit =otherUnit._isSIUnit;
    this->_isSIBaseUnit =otherUnit._isSIBaseUnit;
    this->_conversionFactorToSIConversionUnit=otherUnit._conversionFactorToSIConversionUnit;
    this->_multiplier = otherUnit._multiplier;
    this->_displayDigits=otherUnit._displayDigits;
    this->_epsilon= otherUnit._epsilon;
    return *this;
}

/**
 * Equal operator
 */
bool CMeasurementUnit::operator ==(const CMeasurementUnit &otherUnit) const
{
    if ( this == &otherUnit ) return true;
    if ( this->_type != otherUnit._type) return false;
    return this->_multiplier == otherUnit._multiplier && this->_name == otherUnit._name
            && this->_isSIUnit==otherUnit._isSIUnit;
}

/**
 * Stream to debug
 */
QDebug operator <<(QDebug d, const CMeasurementUnit &unit)
{
    d << unit._name;
    return d;
}

/**
 * Stream to log
 */
CLogMessage operator<<(CLogMessage log, const CMeasurementUnit &unit)
{
    log << unit._name;
    return log;
}

/**
 * Unequal operator
 */
bool CMeasurementUnit::operator !=(const CMeasurementUnit &otherUnit) const
{
    return !(otherUnit == *this);
}

/**
 * Conversion factor from unit x to y
 */
double CMeasurementUnit::conversionFactor(const CMeasurementUnit &to) const
{
    return CMeasurementUnit::conversionFactor(*this, to);
}

/**
 * Conversion factor from unit x to y
 */
double CMeasurementUnit::conversionFactor(const CMeasurementUnit &from, const CMeasurementUnit &to)
{
    if (from == to) return 1.0;
    double cf = from._conversionFactorToSIConversionUnit / to._conversionFactorToSIConversionUnit;
    return cf;
}

} // namespace BlackCore
