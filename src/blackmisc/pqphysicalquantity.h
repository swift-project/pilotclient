#ifndef PQPHYSICALQUANTITY_H
#define PQPHYSICALQUANTITY_H

#include <QtGlobal>
#include <QString>
#include <QLocale>
#include "blackmisc/pqbase.h"
#include "blackmisc/debug.h"

namespace BlackMisc {

class CPhysicalQuantity;

/*!
 * Our converter function, should be implemented as static method of the quantity
 * classes for clarity
 */
typedef double (*CPhysicalQuantityUnitConverter)(const CPhysicalQuantity *quantity, const CMeasurementUnit &unit);

/*!
 * \brief A physical quantity such as "5m", "20s", "1500ft/s"
 * \author KWB
 */
class CPhysicalQuantity
{
    /*!
     * Stream operator for debugging
     * \brief operator <<
     * \param debug
     * \param quantity
     * \return
     */
    friend QDebug operator<<(QDebug debug, const CPhysicalQuantity &quantity);
    /*!
     * Stream operator for log messages
     * \brief operator <<
     * \param log
     * \param quantity
     * \return
     */
    friend CLogMessage operator<<(CLogMessage log, const CPhysicalQuantity &quantity);

private:
    qint32 _unitValueI; //!< value backed by integer, allows sole integer arithmetic
    double _unitValueD; //!< value backed by double
    double _convertedSiUnitValueD; //!< SI unit value
    bool _isIntegerBaseValue; //!< flag integer? / double?
    CPhysicalQuantityUnitConverter _unitConverter; //! <! converts values between units

    /*!
     * Convert value in another unit, normally just by a factor, but in some cases
     * (e.g. CTemperature)overridden because arbitrary conversion is required
     * \sa CMeasurementUnit::conversionFactor(CMeasurementUnit)
     * \param quantity quanity
     * \param otherUnit
     * \return
     */
    static double standardUnitFactorValueConverter(const CPhysicalQuantity *quantity, const CMeasurementUnit &otherUnit);

protected:
    CMeasurementUnit _pUnit; //!< unit
    CMeasurementUnit _pConversionSiUnit; //!< corresponding SI base unit

    /*!
     * \brief Constructor with int
     * \param baseValue
     * \param unit
     * \param siBaseUnit
     * \param unitConverter
     */
    CPhysicalQuantity(qint32 baseValue, const CMeasurementUnit &unit, const CMeasurementUnit &siConversionUnit,
                      const CPhysicalQuantityUnitConverter unitConverter = CPhysicalQuantity::standardUnitFactorValueConverter);
    /*!
     * \brief Constructor with double
     * \param baseValue
     * \param unit
     * \param siBaseUnit
     */
    CPhysicalQuantity(double baseValue, const CMeasurementUnit &unit, const CMeasurementUnit &siConversionUnit,
                      const CPhysicalQuantityUnitConverter unitConverter = CPhysicalQuantity::standardUnitFactorValueConverter);
    /*!
     * \brief Init by integer
     * \param baseValue
     * \param unitConverter
     */
    void setUnitValue(qint32 baseValue);
    /*!
     * \brief Init by double
     * \param baseValue
     */
    void setUnitValue (double baseValue);
    /*!
     * \brief Set the SI value
     */
    void setConversionSiUnitValue();

public:
    /*!
     * \brief Copy constructor
     * \param otherQuantity
     */
    CPhysicalQuantity(const CPhysicalQuantity &otherQuantity);
    /*!
     * \brief Virtual destructor
     */
    virtual ~CPhysicalQuantity();

    /*!
     * \brief Switch unit, e.g. feet meter
     * \param newUnit
     * \return
     */
    bool switchUnit(const CMeasurementUnit &newUnit);
    /*!
     * \brief Value in SI base unit? Meter is an SI base unit, hertz not!
     * \return
     */
    bool isSiBaseUnit() const { return this->_pUnit.isSiBaseUnit(); }
    /*!
     * \brief Value in SI unit? Hertz is an derived SI unit, NM not!
     * \return
     */
    bool isSiUnit() const { return this->_pUnit.isSiUnit(); }
    /*!
     * \brief Value in unprefixed SI unit? Meter is a unprefixed, kilometer a prefixed SI Unit
     * \return
     */
    bool isUnprefixedSiUnit() const { return this->_pUnit.isUnprefixedSiUnit(); }
    /*!
     * \brief Value to QString with unit, e.g. "5.00m"
     * \param digits
     * @return
     */
    QString unitValueRoundedWithUnit(int digits = -1) const;
    /*!
     * \brief Value in given unit
     * \param unit
     * @return
     */
    double value(const CMeasurementUnit &unit) const;
    /*!
     * \brief Rounded value in unit
     * \param unit
     * \param digits
     * @return
     */
    double valueRounded(const CMeasurementUnit &unit, int digits = -1) const;
    /*!
     * \brief Value to QString with unit, e.g. "5.00m"
     * \param unit
     * \param digits
     * @return
     */
    QString valueRoundedWithUnit(const CMeasurementUnit &unit, int digits = -1) const;
    /*!
     * \brief Value a int
     * @return
     */
    qint32 unitValueToInteger() const { return this->_unitValueI;}
    /*!
     * \brief Value a double
     * @return
     */
    double unitValueToDouble() const { return this->_unitValueD;}
    /*!
     * \brief SI value to integer
     * @return
     */
    qint32 siBaseUnitValueToInteger() const { return CPhysicalQuantity::round(this->_convertedSiUnitValueD,0);}
    /*!
     * \brief SI value to double
     * @return
     */
    double siBaseUnitValueToDouble() const { return this->_convertedSiUnitValueD;}
    /*!
     * \brief Rounded value by n digits
     * \param digits
     * @return
     */
    double unitValueToDoubleRounded(int digits = -1) const;
    /*!
     * \brief Rounded value by n digits
     * \param digits if no value is provided, unit rounding is taken
     * @return
     */
    QString unitValueToQStringRounded(int digits = -1) const;
    /*!
     * \brief SI value as double
     * \return
     */
    double convertedSiValueToDouble() const { return this->_convertedSiUnitValueD;}
    /*!
     * \brief SI value as integer
     * \return
     */
    qint32 convertedSiValueToInteger() const { return (qint32)CPhysicalQuantity::round(this->_convertedSiUnitValueD,0);}
    /*!
     * \brief Rounded SI value by n digits
     * \param digits
     * @return
     */
    double convertedSiValueToDoubleRounded(int digits = -1) const;
    /*!
     * \brief Rounded value by n digits
     * \param digits if no value is provided, unit rounding is taken
     * @return
     */
    QString convertedSiValueToQStringRounded(int digits = -1) const;
    /*!
     * \brief SI Base unit value rounded
     * \param digits
     * @return
     */
    QString convertedSiValueRoundedWithUnit(int digits = -1) const;
    /*!
     * \brief Cast as double
     */
    operator double() const { return this->_convertedSiUnitValueD; }
    /*!
     * \brief Cast as QString
     */
    operator QString() const { return this->unitValueRoundedWithUnit();}
    /*!
     * \brief Assignment operator =
     * \param otherQuantity
     * @return
     */
    CPhysicalQuantity &operator =(const CPhysicalQuantity &otherQuantity);
    /*!
     * \brief Plus operator +=
     * \param otherQuantity
     * @return
     */
    CPhysicalQuantity &operator +=(const CPhysicalQuantity &otherQuantity);
    /*!
     * \brief Plus operator +=
     * \param unprefixedSiUnitValue
     * @return
     */
    CPhysicalQuantity &operator +=(double unprefixedSiUnitValue);
    /*!
     * \brief Minus operator-=
     * \param otherQuantity
     * @return
     */
    CPhysicalQuantity &operator -=(const CPhysicalQuantity &otherQuantity);
    /*!
     * \brief Plus operator +=
     * \param unprefixedSiUnitValue
     * @return
     */
    CPhysicalQuantity &operator -=(double unprefixedSiUnitValue);
    /*!
     * \brief Plus operator +
     * \param otherQuantity
     * @return
     */
    CPhysicalQuantity operator +(const CPhysicalQuantity &otherQuantity) const;
    /*!
     * \brief Minus operator -
     * \param otherQuantity
     * @return
     */
    CPhysicalQuantity operator -(const CPhysicalQuantity &otherQuantity) const;
    /*!
     * \brief Multiply operator *=
     * \param multiply
     * @return
     */
    CPhysicalQuantity &operator *=(double multiply);
    /*!
     * \brief Divide operator /=
     * \param divide
     * @return
     */
    CPhysicalQuantity &operator /=(double divide);
    /*!
     * \brief Operator *
     * \param multiply
     * @return
     */
    CPhysicalQuantity operator *(double multiply) const;
    /*!
     * \brief Operator /
     * \param divide
     * @return
     */
    CPhysicalQuantity operator /(double divide) const;
    /*!
     * \brief Equal operator ==
     * \param otherQuantity
     * @return
     */
    bool operator==(const CPhysicalQuantity &otherQuantity) const;
    /*!
     * \brief Not equal operator !=
     * \param otherQuantity
     * @return
     */
    bool operator!=(const CPhysicalQuantity &otherQuantity) const;
    /*!
     * \brief Greater operator >
     * \param otherQuantity
     * @return
     */
    bool operator >(const CPhysicalQuantity &otherQuantity) const;
    /*!
     * \brief Less operator <
     * \param otherQuantity
     * @return
     */
    bool operator <(const CPhysicalQuantity &otherQuantity) const;
    /*!
     * \brief Less equal operator <=
     * \param otherQuantity
     * @return
     */
    bool operator <=(const CPhysicalQuantity &otherQuantity) const;
    /*!
     * \brief Greater equal operator >=
     * \param otherQuantity
     * @return
     */
    bool operator >=(const CPhysicalQuantity &otherQuantity) const;

    // --------------------------------------------------------------------
    // -- static
    // --------------------------------------------------------------------

    /*!
     * \brief Utility round method
     * \param value
     * \param digits
     * \return
     */
    static double round(double value, int digits);

    /*!
     * \brief Rounded string utility method
     * \param value
     * \param digits
     * \return
     */
    static QString toQStringRounded(double value, int digits);
};
} // namespace BlackCore
#endif // PQPHYSICALQUANTITY_H
