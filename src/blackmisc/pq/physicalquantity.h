/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_PHYSICALQUANTITY_H
#define BLACKMISC_PQ_PHYSICALQUANTITY_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/mixin/mixindbus.h"
#include "blackmisc/mixin/mixindatastream.h"
#include "blackmisc/mixin/mixinicon.h"
#include "blackmisc/mixin/mixinjson.h"
#include "blackmisc/pq/pqstring.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/mixin/mixinindex.h"
#include "blackmisc/mixin/mixinstring.h"
#include "blackmisc/mixin/mixinmetatype.h"

#include <QDBusArgument>
#include <QJsonObject>
#include <QString>
#include <QtGlobal>

namespace BlackMisc::PhysicalQuantities
{
    template <class MU, class PQ> class CPhysicalQuantity;
}

//! \cond
#define BLACK_TEMPLATE_PQ_MIXINS(MU, PQ, Extern, Export)                                                                                        \
    namespace BlackMisc::PhysicalQuantities { class PQ; }                                                                                       \
    namespace BlackMisc::Private                                                                                                                \
    {                                                                                                                                           \
        Extern template struct Export CValueObjectMetaInfo<PhysicalQuantities::PQ>;                                                             \
        Extern template struct Export MetaTypeHelper<PhysicalQuantities::PQ>;                                                                   \
    }                                                                                                                                           \
    namespace BlackMisc::Mixin                                                                                                                  \
    {                                                                                                                                           \
        Extern template class Export DBusOperators<PhysicalQuantities::CPhysicalQuantity<PhysicalQuantities::MU, PhysicalQuantities::PQ>>;      \
        Extern template class Export DataStreamOperators<PhysicalQuantities::CPhysicalQuantity<PhysicalQuantities::MU, PhysicalQuantities::PQ>>;\
        Extern template class Export JsonOperators<PhysicalQuantities::CPhysicalQuantity<PhysicalQuantities::MU, PhysicalQuantities::PQ>>;      \
        Extern template class Export Index<PhysicalQuantities::PQ>;                                                                             \
        Extern template class Export MetaType<PhysicalQuantities::PQ>;                                                                          \
        Extern template class Export String<PhysicalQuantities::PQ>;                                                                            \
        Extern template class Export Icon<PhysicalQuantities::CPhysicalQuantity<PhysicalQuantities::MU, PhysicalQuantities::PQ>>;               \
    }
//! \endcond

/*!
 * \def BLACK_DECLARE_PQ_MIXINS
 * Explicit template declaration of mixins for a CPhysicalQuantity subclass
 * to be placed near the top of the header that defines the class
 */

/*!
 * \def BLACK_DEFINE_PQ_MIXINS
 * Explicit template definition of mixins for a CPhysicalQuantity subclass
 */
#if defined(Q_OS_WIN) && defined(Q_CC_GNU)
#  define BLACK_DECLARE_PQ_MIXINS(MU, PQ)
#  define BLACK_DEFINE_PQ_MIXINS(MU, PQ)
#elif defined(Q_OS_WIN) && defined(Q_CC_CLANG)
#  define BLACK_DECLARE_PQ_MIXINS(MU, PQ) BLACK_TEMPLATE_PQ_MIXINS(MU, PQ, extern, )
#  define BLACK_DEFINE_PQ_MIXINS(MU, PQ)  BLACK_TEMPLATE_PQ_MIXINS(MU, PQ,       , BLACKMISC_EXPORT)
#else
#  define BLACK_DECLARE_PQ_MIXINS(MU, PQ) BLACK_TEMPLATE_PQ_MIXINS(MU, PQ, extern, )
#  define BLACK_DEFINE_PQ_MIXINS(MU, PQ)  BLACK_TEMPLATE_PQ_MIXINS(MU, PQ,       , )
#endif

BLACK_DECLARE_PQ_MIXINS(CAngleUnit, CAngle)
BLACK_DECLARE_PQ_MIXINS(CLengthUnit, CLength)
BLACK_DECLARE_PQ_MIXINS(CPressureUnit, CPressure)
BLACK_DECLARE_PQ_MIXINS(CFrequencyUnit, CFrequency)
BLACK_DECLARE_PQ_MIXINS(CMassUnit, CMass)
BLACK_DECLARE_PQ_MIXINS(CTemperatureUnit, CTemperature)
BLACK_DECLARE_PQ_MIXINS(CSpeedUnit, CSpeed)
BLACK_DECLARE_PQ_MIXINS(CTimeUnit, CTime)
BLACK_DECLARE_PQ_MIXINS(CAccelerationUnit, CAcceleration)

namespace BlackMisc::PhysicalQuantities
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
    template <class MU, class PQ> class CPhysicalQuantity :
        public Mixin::DBusOperators<CPhysicalQuantity<MU, PQ>>,
                public Mixin::DataStreamOperators<CPhysicalQuantity<MU, PQ>>,
                public Mixin::JsonOperators<CPhysicalQuantity<MU, PQ>>,
                public Mixin::Index<PQ>,
                public Mixin::MetaType<PQ>,
                public Mixin::String<PQ>,
                public Mixin::Icon<CPhysicalQuantity<MU, PQ>>
    {
        //! \copydoc CValueObject::compare
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

        //! @{
        //! As integer value
        int valueInteger(MU unit) const;
        QString valueIntegerAsString(MU unit) const { return QString::number(this->valueInteger(unit)); }
        //! @}

        //! @{
        //! As integer value in current unit
        int valueInteger() const;
        QString valueIntegerAsString() const { return QString::number(this->valueInteger()); }
        //! @}

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
        QString valueRoundedWithUnit(const MU &unit, int digits = -1, bool withGroupSeparator = false, bool i18n = false) const;

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
        CPhysicalQuantity &operator *=(double multiply);

        //! Divide operator /=
        CPhysicalQuantity &operator /=(double divide);

        //! Operator *
        PQ operator *(double multiply) const;

        //! Operator to support commutative multiplication
        friend PQ operator *(double factor, const PQ &other) { return other * factor; }

        //! Operator /
        PQ operator /(double divide) const;

        //! Unary operator -
        PQ operator -() const;

        //! Equal operator ==
        friend bool operator==(const CPhysicalQuantity &a, const CPhysicalQuantity &b) { return a.equals(b); }

        //! Not equal operator !=
        friend bool operator!=(const CPhysicalQuantity &a, const CPhysicalQuantity &b) { return !a.equals(b); }

        //! Plus operator +=
        CPhysicalQuantity &operator +=(const CPhysicalQuantity &other);

        //! Minus operator-=
        CPhysicalQuantity &operator -=(const CPhysicalQuantity &other);

        //! Greater operator >
        friend bool operator>(const CPhysicalQuantity &a, const CPhysicalQuantity &b) { return b.lessThan(a); }

        //! Less operator <
        friend bool operator<(const CPhysicalQuantity &a, const CPhysicalQuantity &b) { return a.lessThan(b); }

        //! Less equal operator <=
        friend bool operator<=(const CPhysicalQuantity &a, const CPhysicalQuantity &b) { return !b.lessThan(a); }

        //! Greater equal operator >=
        friend bool operator>=(const CPhysicalQuantity &a, const CPhysicalQuantity &b) { return !a.lessThan(b); }

        //! Plus operator +
        friend PQ operator +(const PQ &a, const PQ &b) { PQ copy(a); copy += b; return copy; }

        //! Minus operator -
        friend PQ operator -(const PQ &a, const PQ &b) { PQ copy(a); copy -= b; return copy; }

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

        //! \copydoc BlackMisc::Mixin::DBusByMetaClass::marshallToDbus
        void marshallToDbus(QDBusArgument &argument) const;

        //! \copydoc BlackMisc::Mixin::DBusByMetaClass::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument);

        //! Marshal to DBus, preserving the measurement unit
        void marshallToDbus(QDBusArgument &argument, LosslessTag) const;

        //! Unmarshal from DBus, with preserved measurement unit
        void unmarshallFromDbus(const QDBusArgument &argument, LosslessTag);

        //! \copydoc BlackMisc::Mixin::DataStreamByMetaClass::marshalToDataStream
        void marshalToDataStream(QDataStream &stream) const;

        //! \copydoc BlackMisc::Mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream);

        //! \copydoc BlackMisc::Mixin::HashByMetaClass::qHash
        uint getValueHash() const;

        //! \copydoc CValueObject::qHash
        friend uint qHash(const PQ &pq) { return pq.getValueHash(); } // clazy:exclude=qhash-namespace

        //! \copydoc BlackMisc::Mixin::JsonByMetaClass::toJson
        QJsonObject toJson() const;

        //! \copydoc BlackMisc::Mixin::JsonByMetaClass::convertFromJson
        void convertFromJson(const QJsonObject &json);

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const PQ &pq) const;

        //! \copydoc BlackMisc::Mixin::String::toQString
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
        MU m_unit;      //!< unit part

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
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CLengthUnit, CLength>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CPressureUnit, CPressure>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CFrequencyUnit, CFrequency>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CMassUnit, CMass>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CTemperatureUnit, CTemperature>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CSpeedUnit, CSpeed>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CAngleUnit, CAngle>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CTimeUnit, CTime>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE CPhysicalQuantity<CAccelerationUnit, CAcceleration>;
    //! \endcond
} // ns

#endif // guard
