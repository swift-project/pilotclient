/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQPHYSICALQUANTITY_H
#define BLACKMISC_PQPHYSICALQUANTITY_H

#include "blackmisc/mathematics.h"
#include "blackmisc/pqbase.h"
#include "blackmisc/pqunits.h"
#include "blackmisc/debug.h"
#include <QDBusMetaType>
#include <QtGlobal>
#include <QString>
#include <QLocale>

namespace BlackMisc
{
namespace PhysicalQuantities
{

/*!
 * \brief A physical quantity such as "5m", "20s", "1500ft/s"
 */
template <class MU, class PQ> class CPhysicalQuantity : public BlackMisc::CBaseStreamStringifier
{
private:
    double m_unitValueD; //!< value backed by double
    qint32 m_unitValueI; //!< value backed by integer, allows sole integer arithmetic
    double m_convertedSiUnitValueD; //!< SI unit value
    bool m_isIntegerBaseValue; //!< flag integer? / double?

    /*!
     * \brief Easy access to derived class (CRTP template parameter)
     * \return
     */
    PQ const* derived() const
    {
        return static_cast<PQ const *>(this);
    }

    /*!
     * \brief Easy access to derived class (CRTP template parameter)
     * \return
     */
    PQ* derived()
    {
        return static_cast<PQ *>(this);
    }

protected:
    MU m_unit; //!< unit
    MU m_conversionSiUnit; //!< corresponding SI base unit

    /*!
     * \brief Constructor by integer
     * \param baseValue
     * \param unit
     * \param siConversionUnit
     */
    CPhysicalQuantity(qint32 baseValue, const MU &unit, const MU &siConversionUnit);

    /*!
     * \brief Constructor with double
     * \param baseValue
     * \param unit
     * \param siConversionUnit
     */
    CPhysicalQuantity(double baseValue, const MU &unit, const MU &siConversionUnit);

    /*!
     * \brief Name as string
     * \param i18n
     * \return
     */
    virtual QString convertToQString(bool i18n = false) const
    {
        return this->unitValueRoundedWithUnit(-1, i18n);
    }

    /*!
     * \brief Init by integer
     * \param baseValue
     */
    void setUnitValue(qint32 baseValue);

    /*!
     * \brief Init by double
     * \param baseValue
     */

    void setUnitValue(double baseValue);

    /*!
     * \brief Set the SI value
     */
    void setConversionSiUnitValue();

public:
    /*!
     * \brief Copy constructor
     * \param other
     */
    CPhysicalQuantity(const CPhysicalQuantity &other);

    /*!
     * \brief Virtual destructor
     */
    virtual ~CPhysicalQuantity();

    /*!
     * \brief Unit of the distance
     * \return
     */
    MU getUnit() const
    {
        return this->m_unit;
    }

    /*!
     * \brief Conversion SI unit
     * \return
     */
    MU getConversionSiUnit() const
    {
        return this->m_conversionSiUnit;
    }

    /*!
     * \brief Switch unit, e.g. feet to meter
     * \param newUnit
     * \return
     */
    PQ &switchUnit(const MU &newUnit);

    /*!
     * \brief Value in SI base unit? Meter is an SI base unit, hertz not!
     * \return
     */
    bool isSiBaseUnit() const
    {
        return this->m_unit.isSiBaseUnit();
    }

    /*!
     * \brief Value in SI unit? Hertz is an derived SI unit, NM not!
     * \return
     */
    bool isSiUnit() const
    {
        return this->m_unit.isSiUnit();
    }

    /*!
     * \brief Value in unprefixed SI unit? Meter is a unprefixed, kilometer a prefixed SI Unit
     * \return
     */
    bool isUnprefixedSiUnit() const
    {
        return this->m_unit.isUnprefixedSiUnit();
    }

    /*!
     * \brief Value in given unit
     * \param unit
     * \return
     */
    double value(const MU &unit) const;

    /*!
     * \brief Rounded value in unit
     * \param unit
     * \param digits
     * \return
     */
    double valueRounded(const MU &unit, int digits = -1) const;

    /*!
     * \brief Value to QString with unit, e.g. "5.00m"
     * \param unit
     * \param digits
     * \param i18n
     * \return
     */
    QString valueRoundedWithUnit(const MU &unit, int digits = -1, bool i18n = false) const;

    /*!
     * \brief Value as int
     * \return
     */
    qint32 unitValueToInteger() const
    {
        return this->m_unitValueI;
    }

    /*!
     * \brief Value as double
     * \return
     */
    double unitValueToDouble() const
    {
        return this->m_unitValueD;
    }

    /*!
     * \brief Value to QString with unit, e.g. "5.00m"
     * \param digits
     * \param i18n
     * \return
     */
    QString unitValueRoundedWithUnit(int digits = -1, bool i18n = false) const;

    /*!
     * \brief Rounded value by n digits
     * \param digits
     * \return
     */
    double unitValueToDoubleRounded(int digits = -1) const;

    /*!
     * \brief Rounded value by n digits
     * \param digits if no value is provided, unit rounding is taken
     * \return
     */
    QString unitValueToQStringRounded(int digits = -1) const;

    /*!
     * \brief Conversion SI value as double
     * \return
     */
    double convertedSiValueToDouble() const
    {
        return this->m_convertedSiUnitValueD;
    }

    /*!
     * \brief SI value as integer
     * \return
     */
    qint32 convertedSiValueToInteger() const
    {
        return static_cast<qint32>(
                    BlackMisc::Math::CMath::round(this->m_convertedSiUnitValueD, 0));
    }

    /*!
     * \brief Rounded SI value by n digits
     * \param digits
     * \return
     */
    double convertedSiValueToDoubleRounded(int digits = -1) const;

    /*!
     * \brief Rounded value by n digits
     * \param digits if no value is provided, unit rounding is taken
     * \return
     */
    QString convertedSiValueToQStringRounded(int digits = -1) const;

    /*!
     * \brief SI Base unit value rounded
     * \param digits
     * \param i18n
     * \return
     */
    QString convertedSiValueRoundedWithUnit(int digits = -1, bool i18n = false) const;

    /*!
     * \brief Add to the unit value.
     * \remarks Since overloading the + operator with double did lead to unintended conversions, as explicit method
     * \param value
     */
    void addUnitValue(double value);

    /*!
     * \brief Substract to the unit value.
     * \remarks Since overloading the - operator with double did lead to unintended conversions, as explicit method
     * \param value
     */
    void substractUnitValue(double value);

    /*!
    * \brief Multiply operator *=
    * \param multiply
    * \return
    */
    CPhysicalQuantity &operator *=(double multiply);

    /*!
     * \brief Divide operator /=
     * \param divide
     * \return
     */
    CPhysicalQuantity &operator /=(double divide);

    /*!
     * \brief Operator *
     * \param multiply
     * \return
     */
    PQ operator *(double multiply) const;

    /*!
     * \brief Operator to support commutative multiplication
     * \param factor
     * \param other
     * \return
     */
    friend PQ operator *(double factor, const PQ &other)
    {
        return other * factor;
    }

    /*!
     * \brief Operator /
     * \param divide
     * \return
     */
    PQ operator /(double divide) const;

    /*!
     * \brief Equal operator ==
     * \param other
     * \return
     */
    bool operator==(const CPhysicalQuantity &other) const;

    /*!
     * \brief Not equal operator !=
     * \param other
     * \return
     */
    bool operator!=(const CPhysicalQuantity &other) const;

    /*!
     * \brief Plus operator +=
     * \param other
     * \return
     */
    CPhysicalQuantity &operator +=(const CPhysicalQuantity &other);

    /*!
     * \brief Minus operator-=
     * \param other
     * \return
     */
    CPhysicalQuantity &operator -=(const CPhysicalQuantity &other);

    /*!
     * \brief Greater operator >
     * \param other
     * \return
     */
    bool operator >(const CPhysicalQuantity &other) const;

    /*!
     * \brief Less operator <
     * \param other
     * \return
     */
    bool operator <(const CPhysicalQuantity &other) const;

    /*!
     * \brief Less equal operator <=
     * \param other
     * \return
     */
    bool operator <=(const CPhysicalQuantity &other) const;

    /*!
     * \brief Greater equal operator >=
     * \param other
     * \return
     */
    bool operator >=(const CPhysicalQuantity &other) const;

    /*!
     * \brief Assignment operator =
     * \param other
     * \return
     */
    CPhysicalQuantity &operator =(const CPhysicalQuantity &other);

    /*!
     * \brief Plus operator +
     * \param other
     * \return
     */
    PQ operator +(const PQ &other) const;

    /*!
     * \brief Minus operator -
     * \param other
     * \return
     */
    PQ operator -(const PQ &other) const;

    /*!
     * \brief Quantity value <= epsilon
     * \return
     */
    bool isZeroEpsilon() const
    {
        if (this->m_isIntegerBaseValue) return this->m_unitValueI == 0;
        return this->m_unit.isEpsilon(this->m_unitValueD);
    }

    /*!
     * \brief Value >= 0 epsilon considered
     * \return
     */
    bool isGreaterOrEqualZeroEpsilon() const
    {
        return this->isZeroEpsilon() || this->m_unitValueD > 0;
    }

    /*!
     * \brief Value <= 0 epsilon considered
     * \return
     */
    bool isLessOrEqualZeroEpsilon() const
    {
        return this->isZeroEpsilon() || this->m_unitValueD < 0;
    }

    /*!
     * \brief Stream to DBus <<
     * \param argument
     */
    virtual void marshallToDbus(QDBusArgument &argument) const
    {
        argument << this->m_unitValueD;
        argument << this->m_unitValueI;
        argument << this->m_convertedSiUnitValueD;
        argument << this->m_isIntegerBaseValue;
        argument << this->m_unit;
        argument << this->m_conversionSiUnit;
    }

    /*!
     * \brief Stream from DBus >>
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument)
    {
        argument >> this->m_unitValueD;
        argument >> this->m_unitValueI;
        argument >> this->m_convertedSiUnitValueD;
        argument >> this->m_isIntegerBaseValue;
        argument >> this->m_unit;
        argument >> this->m_conversionSiUnit;
    }

    /*!
     * \brief Register metadata of unit and quantity
     */
    static void registerMetadata()
    {
        qRegisterMetaType<MU>(typeid(MU).name());
        qDBusRegisterMetaType<MU>();
        qDBusRegisterMetaType<QList<MU>>();
        qRegisterMetaType<PQ>(typeid(PQ).name());
        qDBusRegisterMetaType<PQ>();
        qDBusRegisterMetaType<QList<PQ>>();
    }
};

} // namespace
} // namespace

#endif // guard
