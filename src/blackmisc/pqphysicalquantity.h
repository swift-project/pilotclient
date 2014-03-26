/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQPHYSICALQUANTITY_H
#define BLACKMISC_PQPHYSICALQUANTITY_H

#include "blackmisc/pqbase.h"
#include "blackmisc/pqunits.h"
#include "blackmisc/mathematics.h"
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

            //! \brief Which subclass of CMeasurementUnit is used?
            typedef MU UnitClass;

            //! \brief Easy access to derived class (CRTP template parameter)
            PQ const *derived() const
            {
                return static_cast<PQ const *>(this);
            }

            //! \brief Easy access to derived class (CRTP template parameter)
            PQ *derived()
            {
                return static_cast<PQ *>(this);
            }

        protected:
            //! \brief Constructor with double
            CPhysicalQuantity(double value, const MU &unit);

            //! \brief Copy constructor
            CPhysicalQuantity(const CPhysicalQuantity &other);

            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

        public:
            //! \brief Virtual destructor
            virtual ~CPhysicalQuantity() {}

            //! \brief Unit
            MU getUnit() const
            {
                return this->m_unit;
            }

            /*!
             * \brief Simply set unit, do no calclulate conversion
             * \sa switchUnit
             */
            void setUnit(const MU &unit)
            {
                this->m_unit = unit;
            }

            //! \brief Set unit by string
            void setUnitBySymbol(const QString &unitName)
            {
                this->m_unit = CMeasurementUnit::unitFromSymbol<MU>(unitName);
            }

            //! \brief Unit
            QString getUnitSymbol() const
            {
                return this->m_unit.getSymbol(true);
            }

            //! \brief Change unit, and convert value to maintain the same quantity
            PQ &switchUnit(const MU &newUnit);

            //! Is quantity null?
            bool isNull() const
            {
                return this->m_unit.isNull();
            }

            //! \brief Value in given unit
            double value(const MU &unit) const;

            //! \brief Value in current unit
            double value() const
            {
                return this->m_value;
            }

            //! \brief Set value in current unit
            void setCurrentUnitValue(double value)
            {
                this->m_value = value;
            }

            //! \brief Rounded value in given unit
            double valueRounded(const MU &unit, int digits = -1) const;

            //! \brief Rounded value in current unit
            double valueRounded(int digits = -1) const
            {
                return this->valueRounded(this->m_unit, digits);
            }

            //! \brief Value to QString with the given unit, e.g. "5.00m"
            QString valueRoundedWithUnit(const MU &unit, int digits = -1, bool i18n = false) const;

            //! \brief Value to QString with the current unit, e.g. "5.00m"
            QString valueRoundedWithUnit(int digits = -1, bool i18n = false) const
            {
                return this->valueRoundedWithUnit(this->m_unit, digits, i18n);
            }

            //! \brief Change value without changing unit
            void setValueSameUnit(double value);

            //! \brief Add to the value in the current unit.
            void addValueSameUnit(double value);

            //! \brief Substract from the value in the current unit.
            void substractValueSameUnit(double value);

            //! \brief Multiply operator *=
            CPhysicalQuantity &operator *=(double multiply);

            //! \brief Divide operator /=
            CPhysicalQuantity &operator /=(double divide);

            //! \brief Operator *
            PQ operator *(double multiply) const;

            //! \brief Operator to support commutative multiplication
            friend PQ operator *(double factor, const PQ &other)
            {
                return other * factor;
            }

            //! \brief Operator /
            PQ operator /(double divide) const;

            //! \brief Equal operator ==
            bool operator==(const CPhysicalQuantity &other) const;

            //! \brief Not equal operator !=
            bool operator!=(const CPhysicalQuantity &other) const;

            //! \brief Plus operator +=
            CPhysicalQuantity &operator +=(const CPhysicalQuantity &other);

            //! \brief Minus operator-=
            CPhysicalQuantity &operator -=(const CPhysicalQuantity &other);

            //! \brief Greater operator >
            bool operator >(const CPhysicalQuantity &other) const;

            //! \brief Less operator <
            bool operator <(const CPhysicalQuantity &other) const;

            //! \brief Less equal operator <=
            bool operator <=(const CPhysicalQuantity &other) const;

            //! \brief Greater equal operator >=
            bool operator >=(const CPhysicalQuantity &other) const;

            //! \brief Plus operator +
            PQ operator +(const PQ &other) const;

            //! \brief Minus operator -
            PQ operator -(const PQ &other) const;

            //! \brief Quantity value <= epsilon
            bool isZeroEpsilonConsidered() const
            {
                return this->m_unit.isEpsilon(this->m_value);
            }

            //! \brief Value >= 0 epsilon considered
            bool isPositiveWithEpsilonConsidered() const
            {
                return !this->isZeroEpsilonConsidered() && this->m_value > 0;
            }

            //! \brief Value <= 0 epsilon considered
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

            //! \brief Register metadata of unit and quantity
            static void registerMetadata();
        };

    } // namespace
} // namespace

#endif // guard
