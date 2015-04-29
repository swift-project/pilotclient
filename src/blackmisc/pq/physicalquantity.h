/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_PHYSICALQUANTITY_H
#define BLACKMISC_PQ_PHYSICALQUANTITY_H

#include "blackmisc/pq/measurementunit.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/pq/pqstring.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QtDBus/QDBusMetaType>
#include <QtGlobal>
#include <QString>
#include <QLocale>
#include <typeinfo>

namespace BlackMisc
{
    namespace PhysicalQuantities
    {

        class CLength;
        class CPressure;
        class CFrequency;
        class CMass;
        class CTemperature;
        class CSpeed;
        class CTime;
        class CPressure;
        class CAcceleration;

        /*!
         * A physical quantity such as "5m", "20s", "1500ft/s"
         */
        template <class MU, class PQ> class CPhysicalQuantity :
            public Mixin::DBusOperators<CPhysicalQuantity<MU, PQ>>,
            public Mixin::JsonOperators<CPhysicalQuantity<MU, PQ>>,
            public Mixin::Index<PQ>,
            public Mixin::MetaTypeAndQList<PQ>,
            public Mixin::String<PQ>,
            public Mixin::Icon<CPhysicalQuantity<MU, PQ>>
        {
            //! \copydoc CValueObject::compare
            friend int compare(const PQ &a, const PQ &b) { return compareImpl(a, b); }

        public:
            //! Index
            enum ColumnIndex
            {
                IndexUnit = BlackMisc::CPropertyIndex::GlobalIndexCPhysicalQuantity,
                IndexValue,
                IndexValueRounded0DigitsWithUnit,
                IndexValueRounded1DigitsWithUnit,
                IndexValueRounded2DigitsWithUnit,
                IndexValueRounded3DigitsWithUnit,
                IndexValueRounded6DigitsWithUnit
            };

            //! Unit
            MU getUnit() const;

            //! Simply set unit, do no calclulate conversion
            //! \sa switchUnit
            void setUnit(const MU &unit) { this->m_unit = unit; }

            //! Set unit by string
            void setUnitBySymbol(const QString &unitName);

            //! Unit
            QString getUnitSymbol() const;

            //! Change unit, and convert value to maintain the same quantity
            PQ &switchUnit(const MU &newUnit);

            //! Is quantity null?
            bool isNull() const;

            //! Set null
            void setNull();

            //! Value in given unit
            double value(const MU &unit) const;

            //! Value in current unit
            double value() const;

            //! Set value in current unit
            void setCurrentUnitValue(double value);

            //! Rounded value in given unit
            double valueRounded(const MU &unit, int digits = -1) const;

            //! As integer value
            int valueInteger(const MU &unit) const;

            //! Rounded value in current unit
            double valueRounded(int digits = -1) const;

            //! Value to QString with the given unit, e.g. "5.00m"
            QString valueRoundedWithUnit(const MU &unit, int digits = -1, bool i18n = false) const;

            //! Value to QString with the current unit, e.g. "5.00m"
            QString valueRoundedWithUnit(int digits = -1, bool i18n = false) const;

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
            friend PQ operator *(double factor, const PQ &other) { return other * factor; }

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
            bool isZeroEpsilonConsidered() const;

            //! Value >= 0 epsilon considered
            bool isPositiveWithEpsilonConsidered() const;

            //! Value <= 0 epsilon considered
            bool isNegativeWithEpsilonConsidered() const;

            //! Make value always positive
            void makePositive();

            //! Make value always negative
            void makeNegative();

            //! \copydoc CValueObject::marshallToDbus
            void marshallToDbus(QDBusArgument &argument) const;

            //! \copydoc CValueObject::unmarshallFromDbus
            void unmarshallFromDbus(const QDBusArgument &argument);

            //! \copydoc CValueObject::qHash
            uint getValueHash() const;

            //! \copydoc CValueObject::qHash
            friend uint qHash(const PQ &pq) { return pq.getValueHash(); };

            //! \copydoc CValueObject::toJson
            QJsonObject toJson() const;

            //! \copydoc CValueObject::convertFromJson
            void convertFromJson(const QJsonObject &json);

            //! Parse to string, with specified separator
            void parseFromString(const QString &value, CPqString::SeparatorMode mode);

            //! \copydoc CValueObject::parseFromString
            void parseFromString(const QString &value);

            //! \copydoc CValueObject::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

        protected:
            //! Constructor with double
            CPhysicalQuantity(double value, const MU &unit);

            //! Constructor by parsed string, e.g. 10m
            CPhysicalQuantity(const QString &unitString);

        private:
            double m_value; //!< numeric part
            MU m_unit;      //!< unit part

            //! Which subclass of CMeasurementUnit is used?
            typedef MU UnitClass;

            //! Implementation of compare
            static int compareImpl(const PQ &, const PQ &);

            //! Easy access to derived class (CRTP template parameter)
            PQ const *derived() const;

            //! Easy access to derived class (CRTP template parameter)
            PQ *derived();
        };

        //! \cond PRIVATE
        extern template class BLACKMISC_EXPORT_TEMPLATE CPhysicalQuantity<CLengthUnit, CLength>;
        extern template class BLACKMISC_EXPORT_TEMPLATE CPhysicalQuantity<CPressureUnit, CPressure>;
        extern template class BLACKMISC_EXPORT_TEMPLATE CPhysicalQuantity<CFrequencyUnit, CFrequency>;
        extern template class BLACKMISC_EXPORT_TEMPLATE CPhysicalQuantity<CMassUnit, CMass>;
        extern template class BLACKMISC_EXPORT_TEMPLATE CPhysicalQuantity<CTemperatureUnit, CTemperature>;
        extern template class BLACKMISC_EXPORT_TEMPLATE CPhysicalQuantity<CSpeedUnit, CSpeed>;
        extern template class BLACKMISC_EXPORT_TEMPLATE CPhysicalQuantity<CAngleUnit, CAngle>;
        extern template class BLACKMISC_EXPORT_TEMPLATE CPhysicalQuantity<CTimeUnit, CTime>;
        extern template class BLACKMISC_EXPORT_TEMPLATE CPhysicalQuantity<CAccelerationUnit, CAcceleration>;
        //! \endcond

    }
}

#endif // guard
