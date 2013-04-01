#ifndef PQPHYSICALQUANTITY_H
#define PQPHYSICALQUANTITY_H

#include <QtGlobal>
#include <QString>
#include <QLocale>
#include "blackmisc/pqbase.h"
#include "blackmisc/pqunits.h"
#include "blackmisc/debug.h"

namespace BlackMisc {

/*!
 * \brief A physical quantity such as "5m", "20s", "1500ft/s"
 * \author KWB
 */
template <class MU, class PQ> class CPhysicalQuantity
{
    /*!
     * Stream operator for debugging
     * \brief operator <<
     * \param debug
     * \param quantity
     * \return
     * \remarks Has to be in the header files to avoid template link errors
     */
    friend QDebug operator<<(QDebug debug, const CPhysicalQuantity &quantity) {
        QString v = quantity.stringForStreamingOperator();
        debug << v;
        return debug;
    }

    /*!
     * Stream operator for log messages
     * \brief operator <<
     * \param log
     * \param quantity
     * \return
     * \remarks Has to be in the header files to avoid template link errors
     */
    friend CLogMessage operator<<(CLogMessage log, const CPhysicalQuantity &quantity) {
        QString v = quantity.stringForStreamingOperator();
        log << v;
        return log;
    }

private:
    qint32 m_unitValueI; //!< value backed by integer, allows sole integer arithmetic
    double m_unitValueD; //!< value backed by double
    double m_convertedSiUnitValueD; //!< SI unit value
    bool m_isIntegerBaseValue; //!< flag integer? / double?

protected:
    MU m_unit; //!< unit
    MU m_conversionSiUnit; //!< corresponding SI base unit
    /*!
     * \brief String for streaming operators
     * \return
     */
    virtual QString stringForStreamingOperator() const { return this->unitValueRoundedWithUnit(-1); }

    /*!
     * \brief Constructor with int
     * \param baseValue
     * \param unit
     * \param siBaseUnit
     * \param unitConverter
     */
    CPhysicalQuantity(qint32 baseValue, const MU &unit, const MU &siConversionUnit);
    /*!
     * \brief Constructor with double
     * \param baseValue
     * \param unit
     * \param siBaseUnit
     */
    CPhysicalQuantity(double baseValue, const MU &unit, const MU &siConversionUnit);
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
     * \brief Unit of the distance
     * \return
     */
    MU getUnit() const { return this->m_unit; }
    /*!
     * \brief Conversion SI unit
     * \return
     */
    MU getConversionSiUnit() const { return this->m_conversionSiUnit; }
    /*!
     * \brief Switch unit, e.g. feet meter
     * \param newUnit
     * \return
     */
    bool switchUnit(const MU &newUnit);
    /*!
     * \brief Value in SI base unit? Meter is an SI base unit, hertz not!
     * \return
     */
    bool isSiBaseUnit() const { return this->m_unit.isSiBaseUnit(); }
    /*!
     * \brief Value in SI unit? Hertz is an derived SI unit, NM not!
     * \return
     */
    bool isSiUnit() const { return this->m_unit.isSiUnit(); }
    /*!
     * \brief Value in unprefixed SI unit? Meter is a unprefixed, kilometer a prefixed SI Unit
     * \return
     */
    bool isUnprefixedSiUnit() const { return this->m_unit.isUnprefixedSiUnit(); }
    /*!
     * \brief Value in given unit
     * \param unit
     * @return
     */
    double value(const MU &unit) const;
    /*!
     * \brief Rounded value in unit
     * \param unit
     * \param digits
     * @return
     */
    double valueRounded(const MU &unit, int digits = -1) const;
    /*!
     * \brief Value to QString with unit, e.g. "5.00m"
     * \param unit
     * \param digits
     * @return
     */
    QString valueRoundedWithUnit(const MU &unit, int digits = -1) const;
    /*!
     * \brief Value a int
     * @return
     */
    qint32 unitValueToInteger() const { return this->m_unitValueI;}
    /*!
     * \brief Value a double
     * @return
     */
    double unitValueToDouble() const { return this->m_unitValueD;}
    /*!
     * \brief Value to QString with unit, e.g. "5.00m"
     * \param digits
     * @return
     */
    QString unitValueRoundedWithUnit(int digits = -1) const;
    /*!
     * \brief SI value to integer
     * @return
     */
    qint32 siBaseUnitValueToInteger() const { return CMeasurementUnit::round(this->m_convertedSiUnitValueD,0);}
    /*!
     * \brief SI value to double
     * @return
     */
    double siBaseUnitValueToDouble() const { return this->m_convertedSiUnitValueD;}
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
    double convertedSiValueToDouble() const { return this->m_convertedSiUnitValueD;}
    /*!
     * \brief SI value as integer
     * \return
     */
    qint32 convertedSiValueToInteger() const { return static_cast<qint32>(CMeasurementUnit::round(this->m_convertedSiUnitValueD,0));}
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
     * \brief Add to the unit value.
     * \remarks Since overloading the + operator with double did lead to unintended conversions, as explicit method
     * \param value
     */
    void addUnitValue(double value);
    /*!
     * \brief Substratc to the unit value.
     * \remarks Since overloading the - operator with double did lead to unintended conversions, as explicit method
     * \param value
     */
    void substractUnitValue(double value);
    /*!
     * \brief Cast as QString
     */
    operator QString() const { return this->unitValueRoundedWithUnit();}

    /*!
     * \brief Multiply operator *=
     * \param multiply
     * \return
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
    PQ operator *(double multiply) const;
    /*!
     * \brief Operator /
     * \param divide
     * @return
     */
    PQ operator /(double divide) const;
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
     * \brief Plus operator +=
     * \param otherQuantity
     * @return
     */
    CPhysicalQuantity &operator +=(const CPhysicalQuantity &otherQuantity);
    /*!
     * \brief Minus operator-=
     * \param otherQuantity
     * @return
     */
    CPhysicalQuantity &operator -=(const CPhysicalQuantity &otherQuantity);
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
    /*!
     * \brief Assignment operator =
     * \param otherQuantity
     * @return
     */
    CPhysicalQuantity &operator =(const CPhysicalQuantity &otherQuantity);
    /*!
     * \brief Plus operator +
     * \param otherQuantity
     * @return
     */
    PQ operator +(const PQ &otherQuantity) const;
    /*!
     * \brief Minus operator -
     * \param otherQuantity
     * @return
     */
    PQ operator -(const PQ &otherQuantity) const;

};

} // namespace

#endif // PQPHYSICALQUANTITY_H
