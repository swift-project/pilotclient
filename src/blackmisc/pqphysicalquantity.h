/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQPHYSICALQUANTITY_H
#define BLACKMISC_PQPHYSICALQUANTITY_H

#include "blackmisc/pqbase.h"
#include "blackmisc/pqunits.h"
#include "blackmisc/mathematics.h"
#include "blackmisc/pqstring.h"
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
        template <class MU, class PQ> class CPhysicalQuantity : public BlackMisc::CValueObject
        {

        private:
            double m_value; //!< numeric part
            MU m_unit; //!< unit part

            //! Which subclass of CMeasurementUnit is used?
            typedef MU UnitClass;

            //! Easy access to derived class (CRTP template parameter)
            PQ const *derived() const
            {
                return static_cast<PQ const *>(this);
            }

            //! Easy access to derived class (CRTP template parameter)
            PQ *derived()
            {
                return static_cast<PQ *>(this);
            }

        protected:
            //! Constructor with double
            CPhysicalQuantity(double value, const MU &unit);

            //! Copy constructor
            CPhysicalQuantity(const CPhysicalQuantity &other);

            //! Constructor by parsed string, e.g. 10m
            CPhysicalQuantity(const QString &unitString) : m_value(0.0), m_unit(MU::nullUnit())
            {
                this->parseFromString(unitString);
            }

            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

        public:
            //! Virtual destructor
            virtual ~CPhysicalQuantity() {}

            //! Unit
            MU getUnit() const
            {
                return this->m_unit;
            }

            /*!
             * \brief Simply set unit, do no calclulate conversion
             * \sa switchUnit
             */
            void setUnit(const MU &unit) { this->m_unit = unit; }

            //! Set unit by string
            void setUnitBySymbol(const QString &unitName)
            {
                this->m_unit = CMeasurementUnit::unitFromSymbol<MU>(unitName);
            }

            //! Unit
            QString getUnitSymbol() const { return this->m_unit.getSymbol(true); }

            //! Change unit, and convert value to maintain the same quantity
            PQ &switchUnit(const MU &newUnit);

            //! Is quantity null?
            bool isNull() const { return this->m_unit.isNull(); }

            //! Set null
            void setNull() { this->m_unit = MU::nullUnit(); }

            //! Value in given unit
            double value(const MU &unit) const;

            //! Value in current unit
            double value() const
            {
                if (this->isNull())
                {
                    return 0.0;
                }
                return this->m_value;
            }

            //! Set value in current unit
            void setCurrentUnitValue(double value)
            {
                if (!this->isNull())
                {
                    this->m_value = value;
                }
            }

            //! Rounded value in given unit
            double valueRounded(const MU &unit, int digits = -1) const;

            //! Rounded value in current unit
            double valueRounded(int digits = -1) const
            {
                return this->valueRounded(this->m_unit, digits);
            }

            //! Value to QString with the given unit, e.g. "5.00m"
            QString valueRoundedWithUnit(const MU &unit, int digits = -1, bool i18n = false) const;

            //! Value to QString with the current unit, e.g. "5.00m"
            QString valueRoundedWithUnit(int digits = -1, bool i18n = false) const
            {
                return this->valueRoundedWithUnit(this->m_unit, digits, i18n);
            }

            //! Change value without changing unit
            void setValueSameUnit(double value);

            //! Add to the value in the current unit.
            void addValueSameUnit(double value);

            //! Substract from the value in the current unit.
            void substractValueSameUnit(double value);

            //! Multiply operator *=
            CPhysicalQuantity &operator *=(double multiply);

            //! Divide operator /=
            CPhysicalQuantity &operator /=(double divide);

            //! Operator *
            PQ operator *(double multiply) const;

            //! Operator to support commutative multiplication
            friend PQ operator *(double factor, const PQ &other)
            {
                return other * factor;
            }

            //! Operator /
            PQ operator /(double divide) const;

            //! Equal operator ==
            bool operator==(const CPhysicalQuantity &other) const;

            //! Not equal operator !=
            bool operator!=(const CPhysicalQuantity &other) const;

            //! Plus operator +=
            CPhysicalQuantity &operator +=(const CPhysicalQuantity &other);

            //! Minus operator-=
            CPhysicalQuantity &operator -=(const CPhysicalQuantity &other);

            //! Greater operator >
            bool operator >(const CPhysicalQuantity &other) const;

            //! Less operator <
            bool operator <(const CPhysicalQuantity &other) const;

            //! Less equal operator <=
            bool operator <=(const CPhysicalQuantity &other) const;

            //! Greater equal operator >=
            bool operator >=(const CPhysicalQuantity &other) const;

            //! Plus operator +
            PQ operator +(const PQ &other) const;

            //! Minus operator -
            PQ operator -(const PQ &other) const;

            //! Quantity value <= epsilon
            bool isZeroEpsilonConsidered() const
            {
                return this->m_unit.isEpsilon(this->m_value);
            }

            //! Value >= 0 epsilon considered
            bool isPositiveWithEpsilonConsidered() const
            {
                return !this->isZeroEpsilonConsidered() && this->m_value > 0;
            }

            //! Value <= 0 epsilon considered
            bool isNegativeWithEpsilonConsidered() const
            {
                return !this->isZeroEpsilonConsidered() && this->m_value < 0;
            }

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            virtual void fromJson(const QJsonObject &json) override;

            //! Parse to string, with specified separator
            virtual void parseFromString(const QString &value, CPqString::SeparatorMode mode)
            {
                *this = CPqString::parse<PQ>(value, mode);
            }

            //! \copydoc CValueObject::parseFromString
            virtual void parseFromString(const QString &value) override
            {
                *this = CPqString::parse<PQ>(value, CPqString::SeparatorsCLocale);
            }

            //! Register metadata of unit and quantity
            static void registerMetadata();
        };

    } // namespace
} // namespace

#endif // guard
