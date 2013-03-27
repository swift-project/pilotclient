#include "blackmisc/pqallquantities.h"

namespace BlackMisc {

/**
 * Constructor by integer
 */
template <class MU, class PQ> CPhysicalQuantity<MU,PQ>::CPhysicalQuantity(qint32 baseValue, const MU &unit, const MU &siConversionUnit, const CPhysicalQuantityUnitConverter unitConverter) :
    m_unit(unit), m_conversionSiUnit(siConversionUnit), m_unitConverter(unitConverter)
{
    this->setUnitValue(baseValue);
}

/**
 * Constructor by double
 */
template <class MU, class PQ> CPhysicalQuantity<MU,PQ>::CPhysicalQuantity(double baseValue, const MU &unit, const MU &siConversionUnit, const CPhysicalQuantityUnitConverter unitConverter) :
    m_unit(unit), m_conversionSiUnit(siConversionUnit),m_unitConverter(unitConverter)
{
    this->setUnitValue(baseValue);
}

/**
 * Copy constructor
 */
template <class MU, class PQ> CPhysicalQuantity<MU,PQ>::CPhysicalQuantity(const CPhysicalQuantity &otherQuantity) :
    m_unitValueD(otherQuantity.m_unitValueD), m_unitValueI(otherQuantity.m_unitValueI), m_convertedSiUnitValueD(otherQuantity.m_convertedSiUnitValueD),
    m_isIntegerBaseValue(otherQuantity.m_isIntegerBaseValue), m_unit(otherQuantity.m_unit), m_conversionSiUnit(otherQuantity.m_conversionSiUnit), m_unitConverter(otherQuantity.m_unitConverter)
{
    // void
}

/*!
 * Destructor
 */
template <class MU, class PQ> CPhysicalQuantity<MU,PQ>::~CPhysicalQuantity()
{
    // void
}

/**
 * Equal operator ==
 */
template <class MU, class PQ> bool CPhysicalQuantity<MU,PQ>::operator ==(const CPhysicalQuantity<MU,PQ> &otherQuantity) const
{
    if(this == &otherQuantity) return true;
    if(this->m_unit.getType()!= otherQuantity.m_unit.getType()) return false;

    // some special case for best quality
    double diff;
    const double lenient = 1.001; // even diff already has a rounding issue to be avoided
    if (this->m_unit == otherQuantity.m_unit) {
        // same unit
        if (this->m_isIntegerBaseValue && otherQuantity.m_isIntegerBaseValue) {
            // pure integer comparison, no rounding issues
            return this->m_unitValueI == otherQuantity.m_unitValueI;
        } else {
            diff = abs(this->m_unitValueD - otherQuantity.m_unitValueD);
            return diff <= (lenient * this->m_unit.getEpsilon());
        }
    } else {
        // based on SI value
        diff = abs(this->m_convertedSiUnitValueD - otherQuantity.m_convertedSiUnitValueD);
        return diff <= (lenient * this->m_unit.getEpsilon());
    }
}

/**
 * Not equal
 */
template <class MU, class PQ> bool CPhysicalQuantity<MU,PQ>::operator !=(const CPhysicalQuantity<MU,PQ> &otherQuantity) const {
    if(this == &otherQuantity) return false;
    return !(*this == otherQuantity);
}

/**
 * Assigment operator =
 */
template <class MU, class PQ> CPhysicalQuantity<MU,PQ>& CPhysicalQuantity<MU,PQ>::operator=(const CPhysicalQuantity<MU,PQ> &otherQuantity) {

    // Check for self-assignment!
    if (this == &otherQuantity)  return *this; // Same object? Yes, so skip assignment, and just return *this

    this->m_unitValueI = otherQuantity.m_unitValueI;
    this->m_unitValueD = otherQuantity.m_unitValueD;
    this->m_convertedSiUnitValueD = otherQuantity.m_convertedSiUnitValueD;
    this->m_isIntegerBaseValue = otherQuantity.m_isIntegerBaseValue;
    this->m_unit = otherQuantity.m_unit;
    this->m_conversionSiUnit = otherQuantity.m_conversionSiUnit;
    this->m_unitConverter = otherQuantity.m_unitConverter;
    return *this;
}

/**
 * Plus operator
 */
template <class MU, class PQ> CPhysicalQuantity<MU,PQ> &CPhysicalQuantity<MU,PQ>::operator +=(const CPhysicalQuantity<MU,PQ> &otherQuantity)
{
    if (this->m_unit == otherQuantity.m_unit) {
        // same unit
        if (this->m_isIntegerBaseValue && otherQuantity.m_isIntegerBaseValue) {
            // pure integer, no rounding issues
            this->setUnitValue(otherQuantity.m_unitValueI + this->m_unitValueI);
        } else {
            this->setUnitValue(otherQuantity.m_unitValueD + this->m_unitValueD);
        }
    } else {
        double v = otherQuantity.value(this->m_unit);
        this->setUnitValue(v + this->m_unitValueD);
    }
    return *this;
}

/**
 * Plus operator
 */
template <class MU, class PQ> PQ CPhysicalQuantity<MU, PQ>::operator +(const PQ &otherQuantity) const
{
    PQ plus(otherQuantity);
    plus += (*this);
    return plus;
}


/**
 * Explicit plus
 */
template <class MU, class PQ> void CPhysicalQuantity<MU,PQ>::addUnitValue(double value)
{
    this->setUnitValue(this->m_unitValueD + value);
}

/**
 * Explicit minus
 */
template <class MU, class PQ> void CPhysicalQuantity<MU,PQ>::substractUnitValue(double value)
{
    this->setUnitValue(this->m_unitValueD - value);
}

/**
 * Minus operator
 */
template <class MU, class PQ> CPhysicalQuantity<MU,PQ> &CPhysicalQuantity<MU,PQ>::operator -=(const CPhysicalQuantity<MU,PQ> &otherQuantity)
{
    if (this->m_unit == otherQuantity.m_unit) {
        // same unit
        if (this->m_isIntegerBaseValue && otherQuantity.m_isIntegerBaseValue) {
            // pure integer, no rounding issues
            this->setUnitValue(otherQuantity.m_unitValueI - this->m_unitValueI);
        } else {
            this->setUnitValue(otherQuantity.m_unitValueD - this->m_unitValueD);
        }
    } else {
        double v = otherQuantity.value(this->m_unit);
        this->setUnitValue(v - this->m_unitValueD);
    }
    return *this;
}

/**
 * Minus operator
 */
template <class MU, class PQ> PQ CPhysicalQuantity<MU, PQ>::operator -(const PQ &otherQuantity) const
{
    PQ minus;
    minus += (*this);
    minus -= otherQuantity;
    return minus;
}

/**
 * Multiply operator
 */
template <class MU, class PQ> CPhysicalQuantity<MU,PQ> &CPhysicalQuantity<MU,PQ>::operator *=(double multiply)
{
    this->setUnitValue(this->m_unitValueD *multiply);
    return *this;
}

/**
 * Multiply operator
 */
template <class MU, class PQ> PQ CPhysicalQuantity<MU, PQ>::operator *(double multiply) const
{
    PQ times;
    times += (*this);
    times *= multiply;
    return times;
}

/**
 * Divide operator /=
 */
template <class MU, class PQ> CPhysicalQuantity<MU,PQ> &CPhysicalQuantity<MU,PQ>::operator /=(double divide)
{
    this->setUnitValue(this->m_unitValueD / divide);
    return *this;
}

/**
 * Divide operator /
 */
template <class MU, class PQ> PQ CPhysicalQuantity<MU,PQ>::operator /(double divide) const
{
    PQ div;
    div += (*this);
    div /= divide;
    return div;
}

/**
 * Less operator <
 */
template <class MU, class PQ> bool CPhysicalQuantity<MU,PQ>::operator <(const CPhysicalQuantity<MU,PQ> &otherQuantity) const {
    if(this == &otherQuantity) return false;
    double diff = this->m_convertedSiUnitValueD - otherQuantity.m_convertedSiUnitValueD;
    return (diff < 0 && abs(diff) >= this->m_unit.getEpsilon());
}

/**
 * Greater than
 */
template <class MU, class PQ> bool CPhysicalQuantity<MU,PQ>::operator >(const CPhysicalQuantity<MU,PQ> &otherQuantity) const {
    if(this == &otherQuantity) return false;
    return otherQuantity < *this;
}

/**
 * Greater / Equal
 */
template <class MU, class PQ> bool CPhysicalQuantity<MU,PQ>::operator >=(const CPhysicalQuantity<MU,PQ> &otherQuantity) const {
    if(this == &otherQuantity) return true;
    return !(*this < otherQuantity);
}

/**
 * Less equal
 */
template <class MU, class PQ> bool CPhysicalQuantity<MU,PQ>::operator <=(const CPhysicalQuantity<MU,PQ> &otherQuantity) const {
    if(this == &otherQuantity) return true;
    return !(*this > otherQuantity);
}

/**
 * Switch to another unit
 */
template <class MU, class PQ> bool CPhysicalQuantity<MU,PQ>::switchUnit(const MU &newUnit)
{
    if (this->m_unit == newUnit) return true;
    if (this->m_unit.getType() != newUnit.getType()) return false; // not possible
    double cf = this->m_unitConverter(this, newUnit);
    this->m_unit = newUnit;
    this->setUnitValue(cf);
    return true;
}

/**
 * Init by integer
 */
template <class MU, class PQ> void CPhysicalQuantity<MU,PQ>::setUnitValue(qint32 baseValue)
{
    this->m_unitValueI= baseValue;
    this->m_unitValueD= double(baseValue);
    this->m_isIntegerBaseValue = true;
    this->setConversionSiUnitValue();
}

/**
 * Init by double
 */
template <class MU, class PQ> void CPhysicalQuantity<MU,PQ>::setUnitValue(double baseValue)
{
    this->m_unitValueD = baseValue;
    this->m_unitValueI = qRound(baseValue);
    this->m_isIntegerBaseValue = false;
    this->setConversionSiUnitValue();
}

/**
 * Set SI value
 */
template <class MU, class PQ> void CPhysicalQuantity<MU,PQ>::setConversionSiUnitValue() {
    this->m_convertedSiUnitValueD = this->m_unitConverter(this, this->m_conversionSiUnit);
}

/**
 * Standard conversion by factor, used in most cases, in some cases (e.g. CTemperature) arbitrary converter
 */
template <class MU, class PQ> double CPhysicalQuantity<MU,PQ>::standardUnitFactorValueConverter(const CPhysicalQuantity<MU,PQ> *quantity, const MU &otherUnit) {
    if (quantity->m_unit == MU::None() || quantity->m_unitValueD == 0.0) return 0.0;
    if (quantity->m_unit == otherUnit) return quantity->m_unitValueD;
    double f = quantity->m_unit.conversionFactor(otherUnit);
    return f * quantity->m_unitValueD;
}

/**
 * Round
 */
template <class MU, class PQ> double CPhysicalQuantity<MU,PQ>::unitValueToDoubleRounded(int digits) const
{
    if (digits < 1) digits = this->m_unit.getDisplayDigits();
    return CPhysicalQuantity<MU,PQ>::round(this->m_unitValueD, digits);
}

/**
 * Rounded to QString
 */
template <class MU, class PQ> QString CPhysicalQuantity<MU,PQ>::toQStringRounded(double value, int digits)
{
    double v =CPhysicalQuantity<MU,PQ>::round(value, digits);
    QString s = QLocale::system().toString(v, 'f', digits);
    return s;
}

/**
 * Rounded value to QString
 */
template <class MU, class PQ> QString CPhysicalQuantity<MU,PQ>::unitValueToQStringRounded(int digits) const
{
    if (digits < 1) digits = this->m_unit.getDisplayDigits();
    return CPhysicalQuantity<MU,PQ>::toQStringRounded(this->m_unitValueD, digits);
}

/**
 * Rounded SI value to QString
 */
template <class MU, class PQ> QString CPhysicalQuantity<MU,PQ>::convertedSiValueToQStringRounded(int digits) const
{
    if (digits < 1) digits = this->m_conversionSiUnit.getDisplayDigits();
    return CPhysicalQuantity<MU,PQ>::toQStringRounded(this->m_convertedSiUnitValueD, digits);
}

/**
 * Value rounded in original unit
 */
template <class MU, class PQ> QString CPhysicalQuantity<MU,PQ>::unitValueRoundedWithUnit(int digits) const {
    if (digits < 1) digits = this->m_unit.getDisplayDigits();
    return this->unitValueToQStringRounded(digits).append(this->m_unit.getUnitName());
}

/**
 * SI base unit value with unit
 */
template <class MU, class PQ> QString CPhysicalQuantity<MU,PQ>::convertedSiValueRoundedWithUnit(int digits) const {
    if (digits < 1) digits = this->m_conversionSiUnit.getDisplayDigits();
    return this->convertedSiValueToQStringRounded(digits).append(this->m_conversionSiUnit.getUnitName());
}

/**
 * Value rounded in unit
 */
template <class MU, class PQ> QString CPhysicalQuantity<MU,PQ>::valueRoundedWithUnit(const MU &unit, int digits) const
{
    if (unit == this->m_unit) return this->unitValueRoundedWithUnit(digits);
    if (unit == this->m_conversionSiUnit) return this->convertedSiValueRoundedWithUnit(digits);
    if (digits < 0) digits = unit.getDisplayDigits();
    return CPhysicalQuantity<MU,PQ>::toQStringRounded(this->value(unit), digits).append(unit.getUnitName());
}

/**
 * Value rounded in unit
 */
template <class MU, class PQ> double CPhysicalQuantity<MU,PQ>::valueRounded(const MU &unit, int digits) const
{
    if (digits < 1) digits = unit.getDisplayDigits();
    return CPhysicalQuantity<MU,PQ>::round(this->value(unit),digits);
}

/**
 * Value in unit
 */
template <class MU, class PQ> double CPhysicalQuantity<MU,PQ>::value(const MU &unit) const
{
    if (unit == this->m_conversionSiUnit) return this->m_convertedSiUnitValueD;
    double v =  this->m_unitConverter(this, unit);
    return v;
}

/**
 * Round utility method
 */
template <class MU, class PQ> double CPhysicalQuantity<MU,PQ>::convertedSiValueToDoubleRounded(int digits) const
{
    if (digits < 1) digits = this->m_conversionSiUnit.getDisplayDigits();
    return CPhysicalQuantity<MU,PQ>::round(this->m_convertedSiUnitValueD, digits);
}

/**
 * Round utility method
 */
template <class MU, class PQ> double CPhysicalQuantity<MU,PQ>::round(double value, int digits) {
    // gosh, is there no Qt method for this???
    // It's year 2013
    double m = pow(10.0,digits);
    double rv = double(qRound(value * m) / m);
    return rv;
}

// see here for the reason of this forward initialization
// http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
template class CPhysicalQuantity<CDistanceUnit, CDistance>;
template class CPhysicalQuantity<CPressureUnit, CPressure>;
template class CPhysicalQuantity<CFrequencyUnit, CFrequency>;
template class CPhysicalQuantity<CMassUnit, CMass>;
template class CPhysicalQuantity<CTemperatureUnit, CTemperature>;
template class CPhysicalQuantity<CSpeedUnit, CSpeed>;
template class CPhysicalQuantity<CAngleUnit, CAngle>;

} // namespace
