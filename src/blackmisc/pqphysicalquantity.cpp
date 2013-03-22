#include "blackmisc/pqphysicalquantity.h"

namespace BlackMisc {

/**
 * Constructor by integer
 */
CPhysicalQuantity::CPhysicalQuantity(qint32 baseValue, const CMeasurementUnit &unit, const CMeasurementUnit &siConversionUnit) :
    _unit(unit), _conversionSiUnit(siConversionUnit)
{
    this->setUnitValue(baseValue);
}

/**
 * Constructor by double
 */
CPhysicalQuantity::CPhysicalQuantity(double baseValue, const CMeasurementUnit &unit, const CMeasurementUnit &siConversionUnit) :
    _unit(unit), _conversionSiUnit(siConversionUnit)
{
    this->setUnitValue(baseValue);
}

/**
 * Copy constructor
 */
CPhysicalQuantity::CPhysicalQuantity(const CPhysicalQuantity &otherQuantity) :
    _unitValueD(otherQuantity._unitValueD), _unitValueI(otherQuantity._unitValueI), _convertedSiUnitValueD(otherQuantity._convertedSiUnitValueD),
    _isIntegerBaseValue(otherQuantity._isIntegerBaseValue), _unit(otherQuantity._unit), _conversionSiUnit(otherQuantity._conversionSiUnit)
{
    // void
}

/**
 * Stream operator
 */
QDebug operator <<(QDebug d, const CPhysicalQuantity &quantity)
{
    QString v = quantity.unitValueRoundedWithUnit(-1);
    d << v;
    return d;
}

/**
 * Stream operator
 */
CLogMessage operator <<(CLogMessage d, const CPhysicalQuantity &quantity)
{
    QString v = quantity.unitValueRoundedWithUnit(-1);
    d << v;
    return d;
}

/**
 * Equal operator ==
 */
bool CPhysicalQuantity::operator ==(const CPhysicalQuantity &otherQuantity) const
{
    if(this == &otherQuantity) return true;
    if(this->_unit.getType()!= otherQuantity._unit.getType()) return false;

    // some special case for best quality
    double diff;
    const double lenient = 1.001; // even diff alread has a round issue
    if (this->_unit == otherQuantity._unit) {
        // same unit
        if (this->_isIntegerBaseValue && otherQuantity._isIntegerBaseValue) {
            // pure integer comparison, no rounding issues
            return this->_unitValueI == otherQuantity._unitValueI;
        } else {
            diff = abs(this->_unitValueD - otherQuantity._unitValueD);
            return diff <= (lenient * this->_unit.getEpsilon());
        }
    } else {
        // based on SI value
        diff = abs(this->_convertedSiUnitValueD - otherQuantity._convertedSiUnitValueD);
        return diff <= (lenient * this->_unit.getEpsilon());
    }
}

/**
 * Not equal
 */
bool CPhysicalQuantity::operator !=(const CPhysicalQuantity &otherQuantity) const {
    if(this == &otherQuantity) return false;
    return !(*this == otherQuantity);
}

/**
 * Assigment operator =
 */
CPhysicalQuantity& CPhysicalQuantity::operator=(const CPhysicalQuantity &otherQuantity) {

    // Check for self-assignment!
    if (this == &otherQuantity)  return *this; // Same object? Yes, so skip assignment, and just return *this

    CPhysicalQuantity::_unitValueI = otherQuantity._unitValueI;
    CPhysicalQuantity::_unitValueD = otherQuantity._unitValueD;
    CPhysicalQuantity::_convertedSiUnitValueD = otherQuantity._convertedSiUnitValueD;
    CPhysicalQuantity::_isIntegerBaseValue = otherQuantity._isIntegerBaseValue;
    CPhysicalQuantity::_unit = otherQuantity._unit;
    CPhysicalQuantity::_conversionSiUnit = otherQuantity._conversionSiUnit;
    return *this;
}

/**
 * Plus operator
 */
CPhysicalQuantity &CPhysicalQuantity::operator +=(const CPhysicalQuantity &otherQuantity)
{
    if (this->_unit == otherQuantity._unit) {
        // same unit
        if (this->_isIntegerBaseValue && otherQuantity._isIntegerBaseValue) {
            // pure integer, no rounding issues
            this->setUnitValue(otherQuantity._unitValueI + this->_unitValueI);
        } else {
            this->setUnitValue(otherQuantity._unitValueI + this->_unitValueI);
        }
    } else {
        double v = otherQuantity.value(this->_unit);
        this->setUnitValue(v + this->_unitValueD);
    }
    return *this;
}

/**
 * Plus operator
 */
CPhysicalQuantity &CPhysicalQuantity::operator +=(double unprefixedSiUnitValue)
{
    if (!this->isUnprefixedSiUnit()) {
        this->switchUnit(this->_conversionSiUnit);
    }
    this->setUnitValue(this->_unitValueD + unprefixedSiUnitValue);
    return *this;
}

/**
 * Plus operator
 */
const CPhysicalQuantity CPhysicalQuantity::operator +(const CPhysicalQuantity &otherQuantity) const
{
    CPhysicalQuantity pq = (*this);
    return pq+= otherQuantity;
}

/**
 * Minus operator
 */
CPhysicalQuantity &CPhysicalQuantity::operator -=(const CPhysicalQuantity &otherQuantity)
{
    if (this->_unit == otherQuantity._unit) {
        // same unit
        if (this->_isIntegerBaseValue && otherQuantity._isIntegerBaseValue) {
            // pure integer, no rounding issues
            this->setUnitValue(otherQuantity._unitValueI - this->_unitValueI);
        } else {
            this->setUnitValue(otherQuantity._unitValueI - this->_unitValueI);
        }
    } else {
        double v = otherQuantity.value(this->_unit);
        this->setUnitValue(v - this->_unitValueD);
    }
    return *this;
}

/**
 * Minus operator
 */
CPhysicalQuantity &CPhysicalQuantity::operator -=(double unprefixedSiUnitValue)
{
    *this += (-unprefixedSiUnitValue);
    return *this;
}

/**
 * Minus operator
 */
const CPhysicalQuantity CPhysicalQuantity::operator -(const CPhysicalQuantity &otherQuantity) const
{
    CPhysicalQuantity pq = (*this);
    return pq-= otherQuantity;
}

/**
 * Multiply operator
 */
CPhysicalQuantity &CPhysicalQuantity::operator *=(double multiply)
{
    this->setUnitValue(this->_unitValueD *multiply);
    return *this;
}

/**
 * Multiply operator
 */
const CPhysicalQuantity CPhysicalQuantity::operator *(double multiply) const
{
    CPhysicalQuantity pq= (*this);
    return pq *= multiply;
}

/**
 * Divide operator /=
 */
CPhysicalQuantity &CPhysicalQuantity::operator /=(double divide)
{
    this->setUnitValue(this->_unitValueD / divide);
    return *this;
}

/**
 * Divide operator /=
 */
const CPhysicalQuantity CPhysicalQuantity::operator /(double divide) const
{
    CPhysicalQuantity pq= (*this);
    return pq /= divide;
}

/**
 * Less operator <
 */
bool CPhysicalQuantity::operator <(const CPhysicalQuantity &otherQuantity) const {
    if(this == &otherQuantity) return false;
    double diff = this->_convertedSiUnitValueD - otherQuantity._convertedSiUnitValueD;
    return (diff < 0 && abs(diff) >= this->_unit.getEpsilon());
}

bool CPhysicalQuantity::operator >(const CPhysicalQuantity &otherQuantity) const {
    if(this == &otherQuantity) return false;
    return otherQuantity < *this;
}

bool CPhysicalQuantity::operator >=(const CPhysicalQuantity &otherQuantity) const {
    if(this == &otherQuantity) return true;
    return !(*this < otherQuantity);
}

bool CPhysicalQuantity::operator <=(const CPhysicalQuantity &otherQuantity) const {
    if(this == &otherQuantity) return true;
    return !(*this > otherQuantity);
}

/**
 * Switch to another unit
 */
bool CPhysicalQuantity::switchUnit(const CMeasurementUnit &unit)
{
    if (this->_unit == unit) return true;
    if (this->_unit.getType() != unit.getType()) return false; // not possible

    double cf = this->_unit.conversionFactor(unit);
    this->_unit =unit;
    this->setUnitValue(cf * this->_unitValueD);
    return true;
}

/**
 * Init by integer
 */
void CPhysicalQuantity::setUnitValue(qint32 baseValue)
{
    this->_unitValueI= baseValue;
    this->_unitValueD= double(baseValue);
    this->_isIntegerBaseValue = true;
    this->setConversionSiUnitValue();
}

/**
 * Init by double
 */
void CPhysicalQuantity::setUnitValue(double baseValue)
{
    this->_unitValueD = baseValue;
    this->_unitValueI = qRound(baseValue);
    this->_isIntegerBaseValue = false;
    this->setConversionSiUnitValue();

}

/**
 * @brief set SI value
 */
void CPhysicalQuantity::setConversionSiUnitValue() {
    if (this->_unit == CMeasurementUnit::None()) {
        this->_convertedSiUnitValueD=0.0;
    } else {
        double f = this->_unit.conversionFactor(this->_conversionSiUnit);
        this->_convertedSiUnitValueD =f * this->_unitValueD;
    }
}

/**
 * Round
 */
double CPhysicalQuantity::unitValueToDoubleRounded(int digits) const
{
    if (digits < 1) digits = this->_unit.getDisplayDigits();
    return CPhysicalQuantity::round(this->_unitValueD, digits);
}

/**
 * Rounded to QString
 */
QString CPhysicalQuantity::toQStringRounded(double value, int digits)
{
    double v =CPhysicalQuantity::round(value, digits);
    QString s = QLocale::system().toString(v, 'f', digits);
    return s;
}

/**
 * Rounded value to QString
 */
QString CPhysicalQuantity::unitValueToQStringRounded(int digits) const
{
    if (digits < 1) digits = this->_unit.getDisplayDigits();
    return CPhysicalQuantity::toQStringRounded(this->_unitValueD, digits);
}

/**
 * Rounded SI value to QString
 */
QString CPhysicalQuantity::convertedSiValueToQStringRounded(int digits) const
{
    if (digits < 1) digits = this->_conversionSiUnit.getDisplayDigits();
    return CPhysicalQuantity::toQStringRounded(this->_convertedSiUnitValueD, digits);
}

/**
 * Value rounded in original unit
 */
QString CPhysicalQuantity::unitValueRoundedWithUnit(int digits) const {
    if (digits < 1) digits = this->_unit.getDisplayDigits();
    return this->unitValueToQStringRounded(digits).append(this->_unit.getUnitName());
}

/**
 * SI base unit value with unit
 */
QString CPhysicalQuantity::convertedSiValueRoundedWithUnit(int digits) const {
    if (digits < 1) digits = this->_conversionSiUnit.getDisplayDigits();
    return this->convertedSiValueToQStringRounded(digits).append(this->_conversionSiUnit.getUnitName());
}

/**
 * Value rounded in unit
 */
QString CPhysicalQuantity::valueRoundedWithUnit(const CMeasurementUnit &unit, int digits) const
{
    if (unit == this->_unit) return this->unitValueRoundedWithUnit(digits);
    if (unit == this->_conversionSiUnit) return this->convertedSiValueRoundedWithUnit(digits);
    if (digits < 0) digits = unit.getDisplayDigits();
    return CPhysicalQuantity::toQStringRounded(this->value(unit), digits).append(unit.getUnitName());
}

/**
 * Value rounded in unit
 */
double CPhysicalQuantity::valueRounded(const CMeasurementUnit &unit, int digits) const
{
    if (digits < 1) digits = unit.getDisplayDigits();
    return CPhysicalQuantity::round(this->value(unit),digits);
}

/**
 * Value in unit
 */
double CPhysicalQuantity::value(const CMeasurementUnit &unit) const
{
    double v;
    if (unit == this->_unit)
        v = this->_unitValueD;
    else if (unit == this->_conversionSiUnit)
        v = this->_convertedSiUnitValueD;
    else
        v = (this->_unit.conversionFactor(unit)) * (this->_unitValueD);
    return v;
}

/**
 * Round utility method
 */
double CPhysicalQuantity::convertedSiValueToDoubleRounded(int digits) const
{
    if (digits < 1) digits = this->_conversionSiUnit.getDisplayDigits();
    return CPhysicalQuantity::round(this->_convertedSiUnitValueD, digits);
}

/**
 * Round utility method
 */
double CPhysicalQuantity::round(double value, int digits) {
    // gosh, is there no Qt method for this???
    // It's year 2013
    double m = pow(10.0,digits);
    double rv = double(qRound(value * m) / m);
    return rv;
}

} // namespace BlackCore
