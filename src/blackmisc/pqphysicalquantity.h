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
#include <QtDBus/QDBusMetaType>
#include <QtGlobal>
#include <QString>
#include <QLocale>
#include <typeinfo>

namespace BlackMisc
{
namespace PhysicalQuantities
{

/*!
 * \brief A physical quantity such as "5m", "20s", "1500ft/s"
 */
template <class MU, class PQ> class CPhysicalQuantity : public BlackMisc::CStreamable
{
private:
    double m_value; //!< numeric part
    MU m_unit; //!< unit part

    /*!
     * Which subclass of CMeasurementUnit does this quantity use?
     */
    typedef MU UnitClass;

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
    /*!
     * \brief Constructor with double
     * \param value
     * \param unit
     */
    CPhysicalQuantity(double value, const MU &unit);

    /*!
     * \brief Name as string
     * \param i18n
     * \return
     */
    virtual QString convertToQString(bool i18n = false) const
    {
        return this->valueRoundedWithUnit(this->getUnit(), -1, i18n);
    }

public:
    /*!
     * \brief Virtual destructor
     */
    virtual ~CPhysicalQuantity() {}

    /*!
     * \brief Unit of the distance
     * \return
     */
    MU getUnit() const
    {
        return this->m_unit;
    }

    /*!
     * \brief Change unit, and convert value to maintain the same quantity
     * \param newUnit
     * \return
     */
    PQ &switchUnit(const MU &newUnit);

    /*!
     * \brief Value in given unit
     * \param unit
     * \return
     */
    double value(const MU &unit) const;

    /*!
     * \brief Value in current unit
     * \return
     */
    double value() const
    {
        return this->m_value;
    }

    /*!
     * \brief Rounded value in given unit
     * \param unit
     * \param digits
     * \return
     */
    double valueRounded(const MU &unit, int digits = -1) const;

    /*!
     * \brief Rounded value in current unit
     * \param digits
     * \return
     */
    double valueRounded(int digits = -1) const
    {
        return this->valueRounded(this->m_unit, digits);
    }

    /*!
     * \brief Value to QString with the given unit, e.g. "5.00m"
     * \param unit
     * \param digits
     * \param i18n
     * \return
     */
    QString valueRoundedWithUnit(const MU &unit, int digits = -1, bool i18n = false) const;

    /*!
     * \brief Value to QString with the current unit, e.g. "5.00m"
     * \param digits
     * \param i18n
     * \return
     */
    QString valueRoundedWithUnit(int digits = -1, bool i18n = false) const
    {
        return this->valueRoundedWithUnit(this->m_unit, digits, i18n);
    }

    /*!
     * \brief Change value without changing unit
     * \param value
     */
    void setValueSameUnit(double value);

    /*!
     * \brief Add to the value in the current unit.
     * \param value
     */
    void addValueSameUnit(double value);

    /*!
     * \brief Substract from the value in the current unit.
     * \param value
     */
    void substractValueSameUnit(double value);

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
        return this->m_unit.isEpsilon(this->m_value);
    }

    /*!
     * \brief Value >= 0 epsilon considered
     * \return
     */
    bool isNonNegativeEpsilon() const
    {
        return this->isZeroEpsilon() || this->m_value > 0;
    }

    /*!
     * \brief Value <= 0 epsilon considered
     * \return
     */
    bool isNonPositiveEpsilon() const
    {
        return this->isZeroEpsilon() || this->m_value < 0;
    }

    /*!
     * \brief Stream to DBus <<
     * \param argument
     */
    virtual void marshallToDbus(QDBusArgument &argument) const
    {
        argument << this->value(UnitClass::defaultUnit());
        argument << this->m_value;
        argument << this->m_unit;
    }

    /*!
     * \brief Stream from DBus >>
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument)
    {
        double ignore;
        argument >> ignore;
        argument >> this->m_value;
        argument >> this->m_unit;
    }

    /*!
     * \brief Register metadata of unit and quantity
     */
    static void registerMetadata()
    {
        qRegisterMetaType<MU>(typeid(MU).name());
        qDBusRegisterMetaType<MU>();
        qDBusRegisterMetaType<QList<MU> >();
        qRegisterMetaType<PQ>(typeid(PQ).name());
        qDBusRegisterMetaType<PQ>();
        qDBusRegisterMetaType<QList<PQ> >();
    }
};

} // namespace
} // namespace

#endif // guard
