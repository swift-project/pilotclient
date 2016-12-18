/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/comparefunctions.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/pq/measurementunit.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/pq/pqstring.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/variant.h"
#include "blackmisc/verify.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/pressure.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/mass.h"
#include "blackmisc/pq/temperature.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/acceleration.h"

#include <QCoreApplication>
#include <QDBusArgument>
#include <QHash>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QString>
#include <QtGlobal>

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        template <class MU, class PQ>
        MU CPhysicalQuantity<MU, PQ>::getUnit() const
        {
            return this->m_unit;
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::setUnitBySymbol(const QString &unitName)
        {
            this->m_unit = CMeasurementUnit::unitFromSymbol<MU>(unitName);
        }

        template <class MU, class PQ>
        QString CPhysicalQuantity<MU, PQ>::getUnitSymbol() const { return this->m_unit.getSymbol(true); }

        template <class MU, class PQ>
        CPhysicalQuantity<MU, PQ>::CPhysicalQuantity(double value, const MU &unit) :
            m_value(unit.isNull() ? 0.0 : value), m_unit(unit)
        { }

        template <class MU, class PQ>
        CPhysicalQuantity<MU, PQ>::CPhysicalQuantity(const QString &unitString) :
            m_value(0.0), m_unit(MU::nullUnit())
        {
            this->parseFromString(unitString);
        }

        template <class MU, class PQ>
        bool CPhysicalQuantity<MU, PQ>::operator ==(const CPhysicalQuantity<MU, PQ> &other) const
        {
            if (this == &other) return true;

            if (this->isNull()) return other.isNull();
            if (other.isNull()) return false;

            double diff = std::abs(this->m_value - other.value(this->m_unit));
            return diff <= this->m_unit.getEpsilon();
        }

        template <class MU, class PQ>
        bool CPhysicalQuantity<MU, PQ>::operator !=(const CPhysicalQuantity<MU, PQ> &other) const
        {
            return !((*this) == other);
        }

        template <class MU, class PQ>
        CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator +=(const CPhysicalQuantity<MU, PQ> &other)
        {
            this->m_value += other.value(this->m_unit);
            return *this;
        }

        template <class MU, class PQ>
        PQ CPhysicalQuantity<MU, PQ>::operator +(const PQ &other) const
        {
            PQ copy(other);
            copy += *this;
            return copy;
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::addValueSameUnit(double value)
        {
            this->m_value += value;
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::substractValueSameUnit(double value)
        {
            this->m_value -= value;
        }

        template <class MU, class PQ>
        CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator -=(const CPhysicalQuantity<MU, PQ> &other)
        {
            this->m_value -= other.value(this->m_unit);
            return *this;
        }

        template <class MU, class PQ>
        PQ CPhysicalQuantity<MU, PQ>::operator -(const PQ &other) const
        {
            PQ copy = *derived();
            copy -= other;
            return copy;
        }

        template <class MU, class PQ>
        bool CPhysicalQuantity<MU, PQ>::isZeroEpsilonConsidered() const
        {
            return this->m_unit.isEpsilon(this->m_value);
        }

        template <class MU, class PQ>
        bool CPhysicalQuantity<MU, PQ>::isPositiveWithEpsilonConsidered() const
        {
            return !this->isZeroEpsilonConsidered() && this->m_value > 0;
        }

        template <class MU, class PQ>
        bool CPhysicalQuantity<MU, PQ>::isNegativeWithEpsilonConsidered() const
        {
            return !this->isZeroEpsilonConsidered() && this->m_value < 0;
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::makePositive()
        {
            if (this->m_value < 0) { this->m_value *= -1.0; }
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::makeNegative()
        {
            if (this->m_value > 0) { this->m_value *= -1.0; }
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::marshallToDbus(QDBusArgument &argument) const
        {
            argument << this->value(UnitClass::defaultUnit());
            // argument << this->m_value;
            // argument << this->m_unit;
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> this->m_value;
            // argument >> this->m_unit;
            this->m_unit = UnitClass::defaultUnit();
        }

        template <class MU, class PQ>
        CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator *=(double factor)
        {
            this->m_value *= factor;
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
            this->m_value /= divisor;
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
        bool CPhysicalQuantity<MU, PQ>::operator <(const CPhysicalQuantity<MU, PQ> &other) const
        {
            if (*this == other) return false;
            if (this->isNull() || other.isNull()) return false;

            return (this->m_value < other.value(this->m_unit));
        }

        template <class MU, class PQ>
        bool CPhysicalQuantity<MU, PQ>::operator >(const CPhysicalQuantity<MU, PQ> &other) const
        {
            return other < *this;
        }

        template <class MU, class PQ>
        bool CPhysicalQuantity<MU, PQ>::operator >=(const CPhysicalQuantity<MU, PQ> &other) const
        {
            if (*this == other) return true;
            return *this > other;
        }

        template <class MU, class PQ>
        bool CPhysicalQuantity<MU, PQ>::operator <=(const CPhysicalQuantity<MU, PQ> &other) const
        {
            if (*this == other) return true;
            return *this < other;
        }

        template <class MU, class PQ>
        PQ &CPhysicalQuantity<MU, PQ>::switchUnit(const MU &newUnit)
        {
            if (this->m_unit != newUnit)
            {
                this->m_value = newUnit.convertFrom(this->m_value, this->m_unit);
                this->m_unit = newUnit;
            }
            return *derived();
        }

        template <class MU, class PQ>
        bool CPhysicalQuantity<MU, PQ>::isNull() const
        {
            return this->m_unit.isNull();
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::setNull()
        {
            this->m_unit = MU::nullUnit();
        }

        template <class MU, class PQ>
        double CPhysicalQuantity<MU, PQ>::value() const
        {
            if (this->isNull())
            {
                return 0.0;
            }
            return this->m_value;
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::setCurrentUnitValue(double value)
        {
            if (!this->isNull())
            {
                this->m_value = value;
            }
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::setValueSameUnit(double baseValue)
        {
            this->m_value = baseValue;
        }

        template <class MU, class PQ>
        QString CPhysicalQuantity<MU, PQ>::valueRoundedWithUnit(const MU &unit, int digits, bool i18n) const
        {
            return unit.makeRoundedQStringWithUnit(this->value(unit), digits, i18n);
        }

        template <class MU, class PQ>
        QString CPhysicalQuantity<MU, PQ>::valueRoundedWithUnit(int digits, bool i18n) const
        {
            return this->valueRoundedWithUnit(this->m_unit, digits, i18n);
        }

        template <class MU, class PQ>
        double CPhysicalQuantity<MU, PQ>::valueRounded(const MU &unit, int digits) const
        {
            return unit.roundValue(this->value(unit), digits);
        }

        template <class MU, class PQ>
        int CPhysicalQuantity<MU, PQ>::valueInteger(const MU &unit) const
        {
            double v = unit.roundValue(this->value(unit), 0);
            return static_cast<int>(v);
        }

        template <class MU, class PQ>
        double CPhysicalQuantity<MU, PQ>::valueRounded(int digits) const
        {
            return this->valueRounded(this->m_unit, digits);
        }

        template <class MU, class PQ>
        double CPhysicalQuantity<MU, PQ>::value(const MU &unit) const
        {
            return unit.convertFrom(this->m_value, this->m_unit);
        }

        template <class MU, class PQ>
        QString CPhysicalQuantity<MU, PQ>::convertToQString(bool i18n) const
        {
            if (this->isNull())
            {
                return i18n ? QCoreApplication::translate("CPhysicalQuantity", "undefined") : "undefined";
            }
            return this->valueRoundedWithUnit(this->getUnit(), -1, i18n);
        }

        template <class MU, class PQ>
        uint CPhysicalQuantity<MU, PQ>::getValueHash() const
        {
            QList<uint> hashs;
            // there is no double qHash
            // also unit and rounding has to be considered
            hashs << qHash(this->valueRoundedWithUnit(MU::defaultUnit()));
            return BlackMisc::calculateHash(hashs, "PQ");
        }

        template <class MU, class PQ>
        QJsonObject CPhysicalQuantity<MU, PQ>::toJson() const
        {
            QJsonObject json;
            json.insert("value", QJsonValue(this->m_value));
            json.insert("unit", QJsonValue(this->m_unit.getSymbol()));
            return json;
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::convertFromJson(const QJsonObject &json)
        {
            const QJsonValue unit = json.value("unit");
            const QJsonValue value = json.value("value");
            if (unit.isUndefined()) { throw CJsonException("Missing 'unit'"); }
            if (value.isUndefined()) { throw CJsonException("Missing 'value'"); }

            this->setUnitBySymbol(unit.toString());
            this->m_value = value.toDouble();
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::parseFromString(const QString &value, CPqString::SeparatorMode mode)
        {
            *this = CPqString::parse<PQ>(value, mode);
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::parseFromString(const QString &value)
        {
            *this = CPqString::parse<PQ>(value, CPqString::SeparatorsCLocale);
        }

        template <class MU, class PQ>
        CVariant CPhysicalQuantity<MU, PQ>::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*derived()); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexValue:
                return CVariant::from(this->m_value);
            case IndexUnit:
                return CVariant::from(this->m_unit);
            case IndexValueRounded0DigitsWithUnit:
                return CVariant::from(this->valueRoundedWithUnit(0));
            case IndexValueRounded1DigitsWithUnit:
                return CVariant::from(this->valueRoundedWithUnit(1));
            case IndexValueRounded2DigitsWithUnit:
                return CVariant::from(this->valueRoundedWithUnit(2));
            case IndexValueRounded3DigitsWithUnit:
                return CVariant::from(this->valueRoundedWithUnit(3));
            case IndexValueRounded6DigitsWithUnit:
                return CVariant::from(this->valueRoundedWithUnit(6));
            default:
                return Mixin::Index<PQ>::propertyByIndex(index);
            }
        }

        template <class MU, class PQ>
        void CPhysicalQuantity<MU, PQ>::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<PQ>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexValue:
                this->m_value = variant.toDouble();
                break;
            case IndexUnit:
                this->m_unit = variant.to<MU>();
                break;
            case IndexValueRounded0DigitsWithUnit:
            case IndexValueRounded1DigitsWithUnit:
            case IndexValueRounded2DigitsWithUnit:
            case IndexValueRounded3DigitsWithUnit:
            case IndexValueRounded6DigitsWithUnit:
                this->parseFromString(variant.toQString());
                break;
            default:
                Mixin::Index<PQ>::setPropertyByIndex(index, variant);
                break;
            }
        }

        template <class MU, class PQ>
        int CPhysicalQuantity<MU, PQ>::comparePropertyByIndex(const CPropertyIndex &index, const PQ &pq) const
        {
            if (index.isMyself()) { return compareImpl(*derived(), pq); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexValue:
                return Compare::compare(this->m_value, pq.m_value);
            default:
                break;
            }
            BLACK_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable("No comparison for index " + index.toQString()));
            return 0;
        }

        template <class MU, class PQ>
        int CPhysicalQuantity<MU, PQ>::compareImpl(const PQ &a, const PQ &b)
        {
            if (a.isNull() > b.isNull()) { return -1; }
            if (a.isNull() < b.isNull()) { return 1; }

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
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
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
