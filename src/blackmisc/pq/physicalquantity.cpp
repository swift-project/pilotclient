/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/pq/measurementunit.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/pq/pqstring.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/pressure.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/mass.h"
#include "blackmisc/pq/temperature.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/acceleration.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/variant.h"
#include "blackmisc/verify.h"

#include <QCoreApplication>
#include <QDBusArgument>
#include <QHash>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QString>
#include <QtGlobal>
#include <limits>
#include <cmath>

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        template <class MU, class PQ>
        const MU &CPhysicalQuantity<MU, PQ>::getUnit() const
        {
            return m_unit;
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::setUnit(const MU &unit)
        {
            m_unit = unit;
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::setUnitBySymbol(const QString &unitName)
        {
            m_unit = CMeasurementUnit::unitFromSymbol<MU>(unitName);
        }

        template <class MU, class PQ>
        QString CPhysicalQuantity<MU, PQ>::getUnitSymbol() const { return m_unit.getSymbol(true); }

        template <class MU, class PQ>
        CPhysicalQuantity<MU, PQ>::CPhysicalQuantity(double value, MU unit) :
            m_value(unit.isNull() ? 0.0 : value), m_unit(unit)
        {
            Q_ASSERT_X(!std::isnan(value), Q_FUNC_INFO, "nan value");
        }

        template <class MU, class PQ>
        CPhysicalQuantity<MU, PQ>::CPhysicalQuantity(const QString &unitString) :
            m_value(0.0), m_unit(MU::nullUnit())
        {
            this->parseFromString(unitString);
        }

        template <class MU, class PQ>
        bool CPhysicalQuantity<MU, PQ>::equals(const CPhysicalQuantity<MU, PQ> &other) const
        {
            if (this == &other) return true;

            if (this->isNull()) return other.isNull();
            if (other.isNull()) return false;

            double diff = std::abs(m_value - other.value(m_unit));
            return diff <= m_unit.getEpsilon();
        }

        template <class MU, class PQ>
        CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator +=(const CPhysicalQuantity<MU, PQ> &other)
        {
            m_value += other.value(m_unit);
            return *this;
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::addValueSameUnit(double value)
        {
            m_value += value;
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::substractValueSameUnit(double value)
        {
            m_value -= value;
        }

        template <class MU, class PQ>
        CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator -=(const CPhysicalQuantity<MU, PQ> &other)
        {
            m_value -= other.value(m_unit);
            return *this;
        }

        template <class MU, class PQ>
        bool CPhysicalQuantity<MU, PQ>::isZeroEpsilonConsidered() const
        {
            return m_unit.isEpsilon(m_value);
        }

        template <class MU, class PQ>
        bool CPhysicalQuantity<MU, PQ>::isPositiveWithEpsilonConsidered() const
        {
            return !this->isZeroEpsilonConsidered() && m_value > 0;
        }

        template <class MU, class PQ>
        bool CPhysicalQuantity<MU, PQ>::isNegativeWithEpsilonConsidered() const
        {
            return !this->isZeroEpsilonConsidered() && m_value < 0;
        }

        template <class MU, class PQ>
        const PQ &CPhysicalQuantity<MU, PQ>::makePositive()
        {
            if (this->isNull() || qFuzzyIsNull(m_value)) { return *this->derived(); }
            if (m_value < 0) { m_value *= -1.0; }
            return *this->derived();
        }

        template <class MU, class PQ>
        const PQ &CPhysicalQuantity<MU, PQ>::makeNegative()
        {
            if (this->isNull() || qFuzzyIsNull(m_value)) { return *this->derived(); }
            if (m_value > 0) { m_value *= -1.0; }
            return *this->derived();
        }

        template<class MU, class PQ>
        PQ CPhysicalQuantity<MU, PQ>::abs() const
        {
            if (this->isNull() || qFuzzyIsNull(m_value)) { return *this->derived(); }
            if (m_value >= 0) { return *this->derived(); }
            PQ copy(*this->derived());
            return copy.makePositive();
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::marshallToDbus(QDBusArgument &argument) const
        {
            constexpr double NaN = std::numeric_limits<double>::quiet_NaN();
            argument << (this->isNull() ? NaN : this->value(UnitClass::defaultUnit()));
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> m_value;
            m_unit = UnitClass::defaultUnit();
            if (std::isnan(m_value))
            {
                this->setNull();
            }
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::marshallToDbus(QDBusArgument &argument, LosslessTag) const
        {
            argument << m_value;
            argument << m_unit;
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::unmarshallFromDbus(const QDBusArgument &argument, LosslessTag)
        {
            argument >> m_value;
            argument >> m_unit;
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::marshalToDataStream(QDataStream &stream) const
        {
            constexpr double NaN = std::numeric_limits<double>::quiet_NaN();
            stream << (this->isNull() ? NaN : this->value(UnitClass::defaultUnit()));
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::unmarshalFromDataStream(QDataStream &stream)
        {
            stream >> m_value;
            m_unit = UnitClass::defaultUnit();
            if (std::isnan(m_value))
            {
                this->setNull();
            }
        }

        template <class MU, class PQ>
        CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator *=(double factor)
        {
            m_value *= factor;
            return *this;
        }

        template <class MU, class PQ>
        PQ CPhysicalQuantity<MU, PQ>::operator *(double factor) const
        {
            PQ copy = *derived();
            copy *= factor;
            return copy;
        }

        template <class MU, class PQ>
        CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator /=(double divisor)
        {
            m_value /= divisor;
            return *this;
        }

        template <class MU, class PQ>
        PQ CPhysicalQuantity<MU, PQ>::operator /(double divisor) const
        {
            PQ copy = *derived();
            copy /= divisor;
            return copy;
        }

        template <class MU, class PQ>
        PQ CPhysicalQuantity<MU, PQ>::operator -() const
        {
            PQ copy = *derived();
            copy *= -1;
            return copy;
        }

        template <class MU, class PQ>
        bool CPhysicalQuantity<MU, PQ>::lessThan(const CPhysicalQuantity<MU, PQ> &other) const
        {
            if (*this == other) return false;

            if (isNull() < other.isNull()) { return true; }
            if (isNull() > other.isNull()) { return false; }
            if (isNull() && other.isNull()) { return false; }

            return (m_value < other.value(m_unit));
        }

        template <class MU, class PQ>
        PQ &CPhysicalQuantity<MU, PQ>::switchUnit(const MU &newUnit)
        {
            // NULL check: https://discordapp.com/channels/539048679160676382/539925070550794240/593151683698229258
            if (m_unit == newUnit || this->isNull()) { return *derived(); }
            if (newUnit.isNull())
            {
                this->setNull();
            }
            else
            {
                m_value = newUnit.convertFrom(m_value, m_unit);
                m_unit = newUnit;
            }
            return *derived();
        }

        template <class MU, class PQ>
        PQ CPhysicalQuantity<MU, PQ>::switchedUnit(const MU &newUnit) const
        {
            if (m_unit == newUnit || this->isNull()) { return *derived(); }
            PQ copy(*derived());
            copy.switchUnit(newUnit);
            return copy;
        }

        template <class MU, class PQ>
        bool CPhysicalQuantity<MU, PQ>::isNull() const
        {
            return m_unit.isNull();
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::setNull()
        {
            m_value = 0;
            m_unit = MU::nullUnit();
        }

        template <class MU, class PQ>
        double CPhysicalQuantity<MU, PQ>::value() const
        {
            if (this->isNull()) { return 0.0; }
            return m_value;
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::setCurrentUnitValue(double value)
        {
            if (!this->isNull())
            {
                m_value = value;
            }
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::setValueSameUnit(double baseValue)
        {
            m_value = baseValue;
        }

        template <class MU, class PQ>
        QString CPhysicalQuantity<MU, PQ>::valueRoundedWithUnit(const MU &unit, int digits, bool withGroupSeparator, bool i18n) const
        {
            Q_ASSERT_X(!unit.isNull(), Q_FUNC_INFO, "Cannot convert to null");
            if (this->isNull()) { return this->convertToQString(i18n); }
            return unit.makeRoundedQStringWithUnit(this->value(unit), digits, withGroupSeparator, i18n);
        }

        template <class MU, class PQ>
        QString CPhysicalQuantity<MU, PQ>::valueRoundedWithUnit(int digits, bool withGroupSeparator, bool i18n) const
        {
            if (this->isNull()) { return QStringLiteral("null"); }
            return this->valueRoundedWithUnit(m_unit, digits, withGroupSeparator, i18n);
        }

        template<class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::roundToEpsilon()
        {
            if (this->isNull()) { return; }
            m_value = m_unit.roundToEpsilon(m_value);
        }

        template <class MU, class PQ>
        double CPhysicalQuantity<MU, PQ>::valueRounded(MU unit, int digits) const
        {
            Q_ASSERT_X(!unit.isNull(), Q_FUNC_INFO, "Cannot convert to null");
            return unit.roundValue(this->value(unit), digits);
        }

        template <class MU, class PQ>
        int CPhysicalQuantity<MU, PQ>::valueInteger(MU unit) const
        {
            Q_ASSERT_X(!unit.isNull(), Q_FUNC_INFO, "Cannot convert to null");
            const double v = this->value(unit);
            return qRound(v);
        }

        template<class MU, class PQ>
        int CPhysicalQuantity<MU, PQ>::valueInteger() const
        {
            return this->valueInteger(m_unit);
        }

        template<class MU, class PQ>
        bool CPhysicalQuantity<MU, PQ>::isInteger() const
        {
            if (this->isNull()) { return false; }

            const double diff = std::abs(this->value() - this->valueInteger());
            return diff <= m_unit.getEpsilon();
        }

        template <class MU, class PQ>
        double CPhysicalQuantity<MU, PQ>::valueRounded(int digits) const
        {
            return this->valueRounded(m_unit, digits);
        }

        template<class MU, class PQ>
        QString CPhysicalQuantity<MU, PQ>::valueRoundedAsString(MU unit, int digits) const
        {
            if (this->isNull()) { return QStringLiteral("null"); }
            const double v = this->valueRounded(unit, digits);
            return QString::number(v, 'f', digits);
        }

        template <class MU, class PQ>
        double CPhysicalQuantity<MU, PQ>::value(MU unit) const
        {
            Q_ASSERT_X(!unit.isNull(), Q_FUNC_INFO, "Cannot convert to null");
            return unit.convertFrom(m_value, m_unit);
        }

        template <class MU, class PQ>
        QString CPhysicalQuantity<MU, PQ>::convertToQString(bool i18n) const
        {
            if (this->isNull()) { return QStringLiteral("null"); }
            return this->valueRoundedWithUnit(this->getUnit(), -1, i18n);
        }

        template<class MU, class PQ>
        const PQ &CPhysicalQuantity<MU, PQ>::maxValue(const PQ &pq1, const PQ &pq2)
        {
            if (pq1.isNull()) { return pq2; }
            if (pq2.isNull()) { return pq1; }
            return pq1 > pq2 ? pq1 : pq2;
        }

        template<class MU, class PQ>
        const PQ &CPhysicalQuantity<MU, PQ>::minValue(const PQ &pq1, const PQ &pq2)
        {
            if (pq1.isNull()) { return pq2; }
            if (pq2.isNull()) { return pq1; }
            return pq1 < pq2 ? pq1 : pq2;
        }

        template<class MU, class PQ>
        const PQ &CPhysicalQuantity<MU, PQ>::null()
        {
            static const PQ n(0, MU::nullUnit());
            return n;
        }

        template <class MU, class PQ>
        uint CPhysicalQuantity<MU, PQ>::getValueHash() const
        {
            // there is no double qHash
            // also unit and rounding has to be considered
            return qHash(this->valueRoundedWithUnit(MU::defaultUnit()));
        }

        template <class MU, class PQ>
        QJsonObject CPhysicalQuantity<MU, PQ>::toJson() const
        {
            QJsonObject json;
            json.insert("value", QJsonValue(m_value));
            json.insert("unit", QJsonValue(m_unit.getSymbol()));
            return json;
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::convertFromJson(const QJsonObject &json)
        {
            const QJsonValue unit = json.value("unit");
            const QJsonValue value = json.value("value");
            if (unit.isUndefined())  { throw CJsonException("Missing 'unit'"); }
            if (value.isUndefined()) { throw CJsonException("Missing 'value'"); }

            this->setUnitBySymbol(unit.toString());
            m_value = value.toDouble();
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::parseFromString(const QString &value, CPqString::SeparatorMode mode)
        {
            *this = CPqString::parse<PQ>(value, mode);
        }

        template<class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::parseFromString(const QString &value, CPqString::SeparatorMode mode, const MU &defaultUnitIfMissing)
        {
            if (is09OrSeparatorOnlyString(value))
            {
                const QString v = value + defaultUnitIfMissing.getSymbol();
                this->parseFromString(v, mode);
            }
            else
            {
                this->parseFromString(value, mode);
            }
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::parseFromString(const QString &value)
        {
            *this = CPqString::parse<PQ>(value, CPqString::SeparatorQtDefault);
        }

        template<class MU, class PQ>
        PQ CPhysicalQuantity<MU, PQ>::parsedFromString(const QString &value, CPqString::SeparatorMode mode, const MU &defaultUnitIfMissing)
        {
            QString v = value;
            if (is09OrSeparatorOnlyString(value))
            {
                v = value + defaultUnitIfMissing.getSymbol();
            }

            // no idea why I cannot call pq.parseFromString(v, mode, defaultUnitIfMissing);
            PQ pq;
            pq.parseFromString(v, mode);
            return pq;
        }

        template<class MU, class PQ>
        PQ CPhysicalQuantity<MU, PQ>::parsedFromString(const QString &value, CPqString::SeparatorMode mode)
        {
            PQ pq;
            pq.parseFromString(value, mode);
            return pq;
        }

        template <class MU, class PQ>
        QVariant CPhysicalQuantity<MU, PQ>::propertyByIndex(CPropertyIndexRef index) const
        {
            if (index.isMyself()) { return QVariant::fromValue(*derived()); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexValue: return QVariant::fromValue(m_value);
            case IndexUnit:  return QVariant::fromValue(m_unit);
            case IndexValueRounded0DigitsWithUnit: return QVariant::fromValue(this->valueRoundedWithUnit(0));
            case IndexValueRounded1DigitsWithUnit: return QVariant::fromValue(this->valueRoundedWithUnit(1));
            case IndexValueRounded2DigitsWithUnit: return QVariant::fromValue(this->valueRoundedWithUnit(2));
            case IndexValueRounded3DigitsWithUnit: return QVariant::fromValue(this->valueRoundedWithUnit(3));
            case IndexValueRounded6DigitsWithUnit: return QVariant::fromValue(this->valueRoundedWithUnit(6));
            default: return Mixin::Index<PQ>::propertyByIndex(index);
            }
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.value<PQ>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexValue:
                m_value = variant.toDouble();
                break;
            case IndexUnit:
                m_unit = variant.value<MU>();
                break;
            case IndexValueRounded0DigitsWithUnit:
            case IndexValueRounded1DigitsWithUnit:
            case IndexValueRounded2DigitsWithUnit:
            case IndexValueRounded3DigitsWithUnit:
            case IndexValueRounded6DigitsWithUnit:
                this->parseFromString(variant.toString());
                break;
            default:
                Mixin::Index<PQ>::setPropertyByIndex(index, variant);
                break;
            }
        }

        template <class MU, class PQ>
        int CPhysicalQuantity<MU, PQ>::comparePropertyByIndex(CPropertyIndexRef index, const PQ &pq) const
        {
            if (index.isMyself()) { return compareImpl(*derived(), pq); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexValue: return Compare::compare(m_value, pq.m_value);
            default: break;
            }
            BLACK_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable("No comparison for index " + index.toQString()));
            return 0;
        }

        template <class MU, class PQ>
        int CPhysicalQuantity<MU, PQ>::compareImpl(const PQ &a, const PQ &b)
        {
            if (a < b) { return -1; }
            else if (a > b) { return 1; }
            else { return 0; }
        }

        template <class MU, class PQ>
        PQ const *CPhysicalQuantity<MU, PQ>::derived() const
        {
            return static_cast<PQ const *>(this);
        }

        template <class MU, class PQ>
        PQ *CPhysicalQuantity<MU, PQ>::derived()
        {
            return static_cast<PQ *>(this);
        }

        // see here for the reason of thess forward instantiations
        // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
        //! \cond PRIVATE
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE CPhysicalQuantity<CLengthUnit, CLength>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE CPhysicalQuantity<CPressureUnit, CPressure>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE CPhysicalQuantity<CFrequencyUnit, CFrequency>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE CPhysicalQuantity<CMassUnit, CMass>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE CPhysicalQuantity<CTemperatureUnit, CTemperature>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE CPhysicalQuantity<CSpeedUnit, CSpeed>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE CPhysicalQuantity<CAngleUnit, CAngle>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE CPhysicalQuantity<CTimeUnit, CTime>;
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE CPhysicalQuantity<CAccelerationUnit, CAcceleration>;
        //! \endcond

    } // namespace
} // namespace
