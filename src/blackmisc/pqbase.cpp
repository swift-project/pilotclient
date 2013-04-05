#include "blackmisc/pqbase.h"

namespace BlackMisc {

// -----------------------------------------------------------------------
// --- Mulitplier --------------------------------------------------------
// -----------------------------------------------------------------------

/**
 * Constructor
 */
CMeasurementPrefix::CMeasurementPrefix(const QString &name, const QString &unitName, double factor):
    m_name(name),m_prefix(unitName),m_factor(factor)
{
    // void
}

/**
 * Constructor
 */
CMeasurementPrefix::CMeasurementPrefix(const CMeasurementPrefix &otherMultiplier) :
    m_name(otherMultiplier.m_name), m_prefix(otherMultiplier.m_prefix), m_factor(otherMultiplier.m_factor)
{
    // void
}

/**
 * Assignment operator
 */
CMeasurementPrefix& CMeasurementPrefix::operator=(const CMeasurementPrefix &otherMultiplier) {

    if (this == &otherMultiplier) return *this; // Same object? Yes, so skip assignment, and just return *this
    this->m_name = otherMultiplier.m_name;
    this->m_prefix=otherMultiplier.m_prefix;
    this->m_factor=otherMultiplier.m_factor;
    return *this;
}

/**
 * Equal?
 */
bool CMeasurementPrefix::operator ==(const CMeasurementPrefix &otherMultiplier) const
{
    if ( this == &otherMultiplier ) return true;
    return this->m_factor == otherMultiplier.m_factor && this->m_name == otherMultiplier.m_name;
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
    return this->m_factor > otherMultiplier.m_factor;
}

/**
 * Less?
 */
bool CMeasurementPrefix::operator <(const CMeasurementPrefix &otherMultiplier) const
{
    return this->m_factor < otherMultiplier.m_factor;
}

/**
 * Stream to debug
 */
QDebug operator<<(QDebug d, const CMeasurementPrefix &multiplier)
{
    d << multiplier.m_name;
    return d;
}

/**
 * Log to debug
 */
CLogMessage operator<<(CLogMessage log, const CMeasurementPrefix &multiplier)
{
    log << multiplier.m_name;
    return log;
}

// -----------------------------------------------------------------------
// --- Measurement unit --------------------------------------------------
// -----------------------------------------------------------------------

/**
 * Constructor
 */
CMeasurementUnit::CMeasurementUnit(const QString &name, const QString &unitName, const QString &type, bool isSIUnit, bool isSIBaseUnit,
                                   double conversionFactorToSI, const CMeasurementPrefix &multiplier, qint32 displayDigits, double epsilon,
                                   UnitConverter toSiConverter, UnitConverter fromSiConverter):
    m_name(name), m_unitName(unitName), m_type(type), m_isSiUnit(isSIUnit), m_isSiBaseUnit(isSIBaseUnit), m_displayDigits(displayDigits),
    m_conversionFactorToSIConversionUnit(conversionFactorToSI),
    m_epsilon(epsilon), m_multiplier(multiplier), m_fromSiConverter(fromSiConverter), m_toSiConverter(toSiConverter)
{
    // void
}

/*
 * Copy constructor
 */
CMeasurementUnit::CMeasurementUnit(const CMeasurementUnit &otherUnit):
    m_name(otherUnit.m_name), m_unitName(otherUnit.m_unitName), m_type(otherUnit.m_type), m_isSiUnit(otherUnit.m_isSiUnit),
    m_isSiBaseUnit(otherUnit.m_isSiBaseUnit), m_displayDigits(otherUnit.m_displayDigits),m_conversionFactorToSIConversionUnit(otherUnit.m_conversionFactorToSIConversionUnit),
    m_epsilon(otherUnit.m_epsilon), m_multiplier(otherUnit.m_multiplier), m_fromSiConverter(otherUnit.m_fromSiConverter), m_toSiConverter(otherUnit.m_toSiConverter)
{
    // void
}

/**
 * Assigment operator
 */
CMeasurementUnit &CMeasurementUnit::operator =(const CMeasurementUnit &otherUnit)
{
    if (this == &otherUnit) return *this; // Same object? Yes, so skip assignment, and just return *this
    this->m_name = otherUnit.m_name;
    this->m_unitName =otherUnit.m_unitName;
    this->m_type=otherUnit.m_type;
    this->m_isSiUnit =otherUnit.m_isSiUnit;
    this->m_isSiBaseUnit =otherUnit.m_isSiBaseUnit;
    this->m_conversionFactorToSIConversionUnit=otherUnit.m_conversionFactorToSIConversionUnit;
    this->m_multiplier = otherUnit.m_multiplier;
    this->m_displayDigits=otherUnit.m_displayDigits;
    this->m_epsilon= otherUnit.m_epsilon;
    this->m_fromSiConverter = otherUnit.m_fromSiConverter;
    this->m_toSiConverter = otherUnit.m_toSiConverter;
    return *this;
}

/**
 * Equal operator
 */
bool CMeasurementUnit::operator ==(const CMeasurementUnit &otherUnit) const
{
    if ( this == &otherUnit ) return true;
    if ( this->m_type != otherUnit.m_type) return false;
    return this->m_multiplier == otherUnit.m_multiplier && this->m_name == otherUnit.m_name
            && this->m_isSiUnit==otherUnit.m_isSiUnit;
}

/**
 * Stream to debug
 */
QDebug operator <<(QDebug d, const CMeasurementUnit &unit)
{
    d << unit.m_name;
    return d;
}

/**
 * Stream to log
 */
CLogMessage operator<<(CLogMessage log, const CMeasurementUnit &unit)
{
    log << unit.m_name;
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
double CMeasurementUnit::conversionToUnit(double value, const CMeasurementUnit &to) const
{
    if (to == (*this)) return value;
    double siValue = this->convertToSiConversionUnit(value);
    return to.convertFromSiConversionUnit(siValue);
}

/*!
 * Value to QString with unit, e.g. "5.00m"
 * @return
 */
QString CMeasurementUnit::valueRoundedWithUnit(double value, int digits) const {
    return this->toQStringRounded(value, digits).append(this->getUnitName());
}

/*!
 * Value rounded
 */
double CMeasurementUnit::valueRounded(double value, int digits) const {
    if (digits < 0) digits = this->m_displayDigits;
    return CMeasurementUnit::round(value, digits);
}

/**
 * Rounded to QString
 */
QString CMeasurementUnit::toQStringRounded(double value, int digits) const
{
    if (digits < 0) digits = this->m_displayDigits;
    double v = CMeasurementUnit::round(value, digits);
    QString s = QLocale::system().toString(v, 'f', digits);
    return s;
}

/**
 * Round utility method
 */
double CMeasurementUnit::round(double value, int digits) {
    // gosh, is there no Qt method for this???
    // It's year 2013
    double m = pow(10.0,digits);
    double rv = double(qRound(value * m) / m);
    return rv;
}

/*!
 * Epsilon rounding
 */
double CMeasurementUnit::epsilonRounding(double value) const
{
    // does notwork reliable with qRound for some reason
    double v = floor((value + this->m_epsilon) / this->m_epsilon);
    v *=this->m_epsilon;
    return v;
}

} // namespace
