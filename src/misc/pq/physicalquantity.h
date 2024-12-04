// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PQ_PHYSICALQUANTITY_H
#define SWIFT_MISC_PQ_PHYSICALQUANTITY_H

#include <QDBusArgument>
#include <QJsonObject>
#include <QString>
#include <QtGlobal>

#include "misc/mixin/mixindatastream.h"
#include "misc/mixin/mixindbus.h"
#include "misc/mixin/mixinicon.h"
#include "misc/mixin/mixinindex.h"
#include "misc/mixin/mixinjson.h"
#include "misc/mixin/mixinmetatype.h"
#include "misc/mixin/mixinstring.h"
#include "misc/pq/pqstring.h"
#include "misc/pq/units.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::physical_quantities
{
    template <class MU, class PQ>
    class CPhysicalQuantity;
}

//! \cond
#define SWIFT_TEMPLATE_PQ_MIXINS(MU, PQ, Extern, Export)                                                               \
    namespace swift::misc::physical_quantities                                                                         \
    {                                                                                                                  \
        class PQ;                                                                                                      \
    }                                                                                                                  \
    namespace swift::misc::private_ns                                                                                  \
    {                                                                                                                  \
        Extern template struct Export CValueObjectMetaInfo<physical_quantities::PQ>;                                   \
        Extern template struct Export MetaTypeHelper<physical_quantities::PQ>;                                         \
    }                                                                                                                  \
    namespace swift::misc::mixin                                                                                       \
    {                                                                                                                  \
        Extern template class Export                                                                                   \
            DBusOperators<physical_quantities::CPhysicalQuantity<physical_quantities::MU, physical_quantities::PQ>>;   \
        Extern template class Export DataStreamOperators<                                                              \
            physical_quantities::CPhysicalQuantity<physical_quantities::MU, physical_quantities::PQ>>;                 \
        Extern template class Export                                                                                   \
            JsonOperators<physical_quantities::CPhysicalQuantity<physical_quantities::MU, physical_quantities::PQ>>;   \
        Extern template class Export Index<physical_quantities::PQ>;                                                   \
        Extern template class Export MetaType<physical_quantities::PQ>;                                                \
        Extern template class Export String<physical_quantities::PQ>;                                                  \
        Extern template class Export                                                                                   \
            Icon<physical_quantities::CPhysicalQuantity<physical_quantities::MU, physical_quantities::PQ>>;            \
    }
//! \endcond

/*!
 * \def SWIFT_DECLARE_PQ_MIXINS
 * Explicit template declaration of mixins for a CPhysicalQuantity subclass
 * to be placed near the top of the header that defines the class
 */

/*!
 * \def SWIFT_DEFINE_PQ_MIXINS
 * Explicit template definition of mixins for a CPhysicalQuantity subclass
 */
#if defined(Q_OS_WIN) && defined(Q_CC_GNU)
#    define SWIFT_DECLARE_PQ_MIXINS(MU, PQ)
#    define SWIFT_DEFINE_PQ_MIXINS(MU, PQ)
#elif defined(Q_OS_WIN) && defined(Q_CC_CLANG)
#    define SWIFT_DECLARE_PQ_MIXINS(MU, PQ) SWIFT_TEMPLATE_PQ_MIXINS(MU, PQ, extern, )
#    define SWIFT_DEFINE_PQ_MIXINS(MU, PQ) SWIFT_TEMPLATE_PQ_MIXINS(MU, PQ, , SWIFT_MISC_EXPORT)
#else
#    define SWIFT_DECLARE_PQ_MIXINS(MU, PQ) SWIFT_TEMPLATE_PQ_MIXINS(MU, PQ, extern, )
#    define SWIFT_DEFINE_PQ_MIXINS(MU, PQ) SWIFT_TEMPLATE_PQ_MIXINS(MU, PQ, , )
#endif

SWIFT_DECLARE_PQ_MIXINS(CAngleUnit, CAngle)
SWIFT_DECLARE_PQ_MIXINS(CLengthUnit, CLength)
SWIFT_DECLARE_PQ_MIXINS(CPressureUnit, CPressure)
SWIFT_DECLARE_PQ_MIXINS(CFrequencyUnit, CFrequency)
SWIFT_DECLARE_PQ_MIXINS(CMassUnit, CMass)
SWIFT_DECLARE_PQ_MIXINS(CTemperatureUnit, CTemperature)
SWIFT_DECLARE_PQ_MIXINS(CSpeedUnit, CSpeed)
SWIFT_DECLARE_PQ_MIXINS(CTimeUnit, CTime)
SWIFT_DECLARE_PQ_MIXINS(CAccelerationUnit, CAcceleration)

namespace swift::misc::physical_quantities
{
    class CAngle;
    class CLength;
    class CPressure;
    class CFrequency;
    class CMass;
    class CTemperature;
    class CSpeed;
    class CTime;
    class CAcceleration;

    /*!
     * A physical quantity such as "5m", "20s", "1500ft/s"
     */
    template <class MU, class PQ>
    class CPhysicalQuantity :
        public mixin::DBusOperators<CPhysicalQuantity<MU, PQ>>,
        public mixin::DataStreamOperators<CPhysicalQuantity<MU, PQ>>,
        public mixin::JsonOperators<CPhysicalQuantity<MU, PQ>>,
        public mixin::Index<PQ>,
        public mixin::MetaType<PQ>,
        public mixin::String<PQ>,
        public mixin::Icon<CPhysicalQuantity<MU, PQ>>
    {
        //! \copydoc swift::misc::CValueObject::compare
        friend int compare(const PQ &a, const PQ &b) { return compareImpl(a, b); }

    public:
        //! Index
        enum ColumnIndex
        {
            IndexUnit = CPropertyIndexRef::GlobalIndexCPhysicalQuantity,
            IndexValue,
            IndexValueRounded0DigitsWithUnit,
            IndexValueRounded1DigitsWithUnit,
            IndexValueRounded2DigitsWithUnit,
            IndexValueRounded3DigitsWithUnit,
            IndexValueRounded6DigitsWithUnit
        };

        //! Unit
        const MU &getUnit() const;

        //! Simply set unit, do no calclulate conversion
        //! \sa switchUnit
        void setUnit(const MU &unit);

        //! Set unit by string
        void setUnitBySymbol(const QString &unitName);

        //! Unit
        QString getUnitSymbol() const;

        //! Change unit, and convert value to maintain the same quantity
        PQ &switchUnit(const MU &newUnit);

        //! Return copy with switched unit
        PQ switchedUnit(const MU &newUnit) const;

        //! Is quantity null?
        bool isNull() const;

        //! Set null
        void setNull();

        //! Value in given unit
        double value(MU unit) const;

        //! Value in current unit
        double value() const;

        //! Set value in current unit
        void setCurrentUnitValue(double value);

        //! As integer value
        int valueInteger(MU unit) const;

        //! As integer value
        QString valueIntegerAsString(MU unit) const { return QString::number(this->valueInteger(unit)); }

        //! As integer value in current unit
        int valueInteger() const;

        //! As integer value in current unit
        QString valueIntegerAsString() const { return QString::number(this->valueInteger()); }

        //! Is value an integer
        bool isInteger() const;

        //! Rounded value in given unit
        //! \note default digits is CMeasurementUnit::getDisplayDigits
        double valueRounded(MU unit, int digits = -1) const;

        //! Rounded value in current unit
        //! \note default digits is CMeasurementUnit::getDisplayDigits
        double valueRounded(int digits = -1) const;

        //! Rounded value in given unit
        //! \note default digits is CMeasurementUnit::getDisplayDigits
        QString valueRoundedAsString(MU unit, int digits = -1) const;

        //! Value to QString with the given unit, e.g. "5.00m"
        //! \note default digits is CMeasurementUnit::getDisplayDigits
        QString valueRoundedWithUnit(const MU &unit, int digits = -1, bool withGroupSeparator = false,
                                     bool i18n = false) const;

        //! Value to QString with the current unit, e.g. "5.00m"
        //! \note default digits is CMeasurementUnit::getDisplayDigits
        QString valueRoundedWithUnit(int digits = -1, bool withGroupSeparator = false, bool i18n = false) const;

        //! Round current value in current unit to epsilon
        //! \sa CMeasurementUnit::roundToEpsilon
        void roundToEpsilon();

        //! Change value without changing unit
        void setValueSameUnit(double value);

        //! Add to the value in the current unit.
        void addValueSameUnit(double value);

        //! Substract from the value in the current unit.
        void substractValueSameUnit(double value);

        //! Multiply operator *=
        CPhysicalQuantity &operator*=(double multiply);

        //! Divide operator /=
        CPhysicalQuantity &operator/=(double divide);

        //! Operator *
        PQ operator*(double multiply) const;

        //! Operator to support commutative multiplication
        friend PQ operator*(double factor, const PQ &other) { return other * factor; }

        //! Operator /
        PQ operator/(double divide) const;

        //! Unary operator -
        PQ operator-() const;

        //! Equal operator ==
        friend bool operator==(const CPhysicalQuantity &a, const CPhysicalQuantity &b) { return a.equals(b); }

        //! Not equal operator !=
        friend bool operator!=(const CPhysicalQuantity &a, const CPhysicalQuantity &b) { return !a.equals(b); }

        //! Plus operator +=
        CPhysicalQuantity &operator+=(const CPhysicalQuantity &other);

        //! Minus operator-=
        CPhysicalQuantity &operator-=(const CPhysicalQuantity &other);

        //! Greater operator >
        friend bool operator>(const CPhysicalQuantity &a, const CPhysicalQuantity &b) { return b.lessThan(a); }

        //! Less operator <
        friend bool operator<(const CPhysicalQuantity &a, const CPhysicalQuantity &b) { return a.lessThan(b); }

        //! Less equal operator <=
        friend bool operator<=(const CPhysicalQuantity &a, const CPhysicalQuantity &b) { return !b.lessThan(a); }

        //! Greater equal operator >=
        friend bool operator>=(const CPhysicalQuantity &a, const CPhysicalQuantity &b) { return !a.lessThan(b); }

        //! Plus operator +
        friend PQ operator+(const PQ &a, const PQ &b)
        {
            PQ copy(a);
            copy += b;
            return copy;
        }

        //! Minus operator -
        friend PQ operator-(const PQ &a, const PQ &b)
        {
            PQ copy(a);
            copy -= b;
            return copy;
        }

        //! Quantity value <= epsilon
        bool isZeroEpsilonConsidered() const;

        //! Value >= 0 epsilon considered
        bool isPositiveWithEpsilonConsidered() const;

        //! Value <= 0 epsilon considered
        bool isNegativeWithEpsilonConsidered() const;

        //! Make value always positive
        const PQ &makePositive();

        //! Make value always negative
        const PQ &makeNegative();

        //! Absolute value (always >=0)
        PQ abs() const;

        //! \copydoc swift::misc::mixin::DBusByMetaClass::marshallToDbus
        void marshallToDbus(QDBusArgument &argument) const;

        //! \copydoc swift::misc::mixin::DBusByMetaClass::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument);

        //! Marshal to DBus, preserving the measurement unit
        void marshallToDbus(QDBusArgument &argument, LosslessTag) const;

        //! Unmarshal from DBus, with preserved measurement unit
        void unmarshallFromDbus(const QDBusArgument &argument, LosslessTag);

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::marshalToDataStream
        void marshalToDataStream(QDataStream &stream) const;

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream);

        //! \copydoc swift::misc::mixin::HashByMetaClass::qHash
        size_t getValueHash() const;

        //! \copydoc CValueObject::qHash
        friend size_t qHash(const PQ &pq) { return pq.getValueHash(); } // clazy:exclude=qhash-namespace

        //! \copydoc swift::misc::mixin::JsonByMetaClass::toJson
        QJsonObject toJson() const;

        //! \copydoc swift::misc::mixin::JsonByMetaClass::convertFromJson
        void convertFromJson(const QJsonObject &json);

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const PQ &pq) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Parse value from string
        void parseFromString(const QString &value);

        //! Parse to string, with specified separator
        void parseFromString(const QString &value, CPqString::SeparatorMode mode);

        //! Parse to string, with specified separator
        void parseFromString(const QString &value, CPqString::SeparatorMode mode, const MU &defaultUnitIfMissing);

        //! Compare with other PQ
        int compare(const PQ &other) const { return compareImpl(*this->derived(), other); }

        //! Maximum of 2 quantities
        static const PQ &maxValue(const PQ &pq1, const PQ &pq2);

        //! Minimum of 2 quantities
        static const PQ &minValue(const PQ &pq1, const PQ &pq2);

        //! Implementation of compare
        static int compare(const PQ &a, const PQ &b) { return compareImpl(a, b); }

        //! NULL PQ
        static const PQ &null();

        //! Parsed from given string
        static PQ parsedFromString(const QString &value, CPqString::SeparatorMode mode = CPqString::SeparatorBestGuess);

        //! Parsed from given string
        static PQ parsedFromString(const QString &value, CPqString::SeparatorMode mode, const MU &defaultUnitIfMissing);

    protected:
        //! Constructor with double
        CPhysicalQuantity(double value, MU unit);

        //! Constructor by parsed string, e.g. 10m
        CPhysicalQuantity(const QString &unitString);

    private:
        double m_value; //!< numeric part
        MU m_unit; //!< unit part

        //! Which subclass of CMeasurementUnit is used?
        using UnitClass = MU;

        //! Implementation of compare
        static int compareImpl(const PQ &, const PQ &);

        //! Private implementation of equality operators
        bool equals(const CPhysicalQuantity &other) const;

        //! Private implementation of comparison operators
        bool lessThan(const CPhysicalQuantity &other) const;

        //! Easy access to derived class (CRTP template parameter)
        PQ const *derived() const;

        //! Easy access to derived class (CRTP template parameter)
        PQ *derived();
    };

    //! \cond PRIVATE
    extern template class SWIFT_MISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CLengthUnit, CLength>;
    extern template class SWIFT_MISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CPressureUnit, CPressure>;
    extern template class SWIFT_MISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CFrequencyUnit, CFrequency>;
    extern template class SWIFT_MISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CMassUnit, CMass>;
    extern template class SWIFT_MISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CTemperatureUnit, CTemperature>;
    extern template class SWIFT_MISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CSpeedUnit, CSpeed>;
    extern template class SWIFT_MISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CAngleUnit, CAngle>;
    extern template class SWIFT_MISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CTimeUnit, CTime>;
    extern template class SWIFT_MISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CAccelerationUnit, CAcceleration>;
    //! \endcond
} // namespace swift::misc::physical_quantities

#endif // SWIFT_MISC_PQ_PHYSICALQUANTITY_H
