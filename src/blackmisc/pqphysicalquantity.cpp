/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/pqallquantities.h"
#include "blackmiscfreefunctions.h"
#include <QCoreApplication>

namespace BlackMisc
{
    namespace PhysicalQuantities
    {

        /*
         * Constructor by double
         */
        template <class MU, class PQ> CPhysicalQuantity<MU, PQ>::CPhysicalQuantity(double value, const MU &unit) :
            m_value(unit.isNull() ? 0.0 : value), m_unit(unit)
        {
            // void
        }

        /*
         * Equal operator ==
         */
        template <class MU, class PQ> bool CPhysicalQuantity<MU, PQ>::operator ==(const CPhysicalQuantity<MU, PQ> &other) const
        {
            if (this == &other) return true;

            if (this->isNull()) return other.isNull();
            if (other.isNull()) return false;

            double diff = std::abs(this->m_value - other.value(this->m_unit));
            return diff <= this->m_unit.getEpsilon();
        }

        /*
         * Not equal
         */
        template <class MU, class PQ> bool CPhysicalQuantity<MU, PQ>::operator !=(const CPhysicalQuantity<MU, PQ> &other) const
        {
            return !((*this) == other);
        }

        /*
         * Plus operator
         */
        template <class MU, class PQ> CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator +=(const CPhysicalQuantity<MU, PQ> &other)
        {
            this->m_value += other.value(this->m_unit);
            return *this;
        }

        /*
         * Plus operator
         */
        template <class MU, class PQ> PQ CPhysicalQuantity<MU, PQ>::operator +(const PQ &other) const
        {
            PQ copy(other);
            copy += *this;
            return copy;
        }

        /*
         * Explicit plus
         */
        template <class MU, class PQ> void CPhysicalQuantity<MU, PQ>::addValueSameUnit(double value)
        {
            this->m_value += value;
        }

        /*
         * Explicit minus
         */
        template <class MU, class PQ> void CPhysicalQuantity<MU, PQ>::substractValueSameUnit(double value)
        {
            this->m_value -= value;
        }

        /*
         * Minus operator
         */
        template <class MU, class PQ> CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator -=(const CPhysicalQuantity<MU, PQ> &other)
        {
            this->m_value -= other.value(this->m_unit);
            return *this;
        }

        /*
         * Minus operator
         */
        template <class MU, class PQ> PQ CPhysicalQuantity<MU, PQ>::operator -(const PQ &other) const
        {
            PQ copy = *derived();
            copy -= other;
            return copy;
        }

        /*
         * Marshall
         */
        template <class MU, class PQ> void CPhysicalQuantity<MU, PQ>::marshallToDbus(QDBusArgument &argument) const
        {
            argument << this->value(UnitClass::defaultUnit());
            argument << this->m_value;
            argument << this->m_unit;
        }

        /*
         * Unmarshall
         */
        template <class MU, class PQ> void CPhysicalQuantity<MU, PQ>::unmarshallFromDbus(const QDBusArgument &argument)
        {
            double ignore;
            argument >> ignore;
            argument >> this->m_value;
            argument >> this->m_unit;
        }

        /*
         * Multiply operator
         */
        template <class MU, class PQ> CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator *=(double factor)
        {
            this->m_value *= factor;
            return *this;
        }

        /*
         * Multiply operator
         */
        template <class MU, class PQ> PQ CPhysicalQuantity<MU, PQ>::operator *(double factor) const
        {
            PQ copy = *derived();
            copy *= factor;
            return copy;
        }

        /*
         * Divide operator /=
         */
        template <class MU, class PQ> CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator /=(double divisor)
        {
            this->m_value /= divisor;
            return *this;
        }

        /*
         * Divide operator /
         */
        template <class MU, class PQ> PQ CPhysicalQuantity<MU, PQ>::operator /(double divisor) const
        {
            PQ copy = *derived();
            copy /= divisor;
            return copy;
        }

        /*
         * Less operator <
         */
        template <class MU, class PQ> bool CPhysicalQuantity<MU, PQ>::operator <(const CPhysicalQuantity<MU, PQ> &other) const
        {
            if (*this == other) return false;
            if (this->isNull() || other.isNull()) return false;

            return (this->m_value < other.value(this->m_unit));
        }

        /*
         * Greater than
         */
        template <class MU, class PQ> bool CPhysicalQuantity<MU, PQ>::operator >(const CPhysicalQuantity<MU, PQ> &other) const
        {
            return other < *this;
        }

        /*
         * Greater / Equal
         */
        template <class MU, class PQ> bool CPhysicalQuantity<MU, PQ>::operator >=(const CPhysicalQuantity<MU, PQ> &other) const
        {
            if (*this == other) return true;
            return *this > other;
        }

        /*
         * Less equal
         */
        template <class MU, class PQ> bool CPhysicalQuantity<MU, PQ>::operator <=(const CPhysicalQuantity<MU, PQ> &other) const
        {
            if (*this == other) return true;
            return *this < other;
        }

        /*
         * Switch to another unit
         */
        template <class MU, class PQ> PQ &CPhysicalQuantity<MU, PQ>::switchUnit(const MU &newUnit)
        {
            if (this->m_unit != newUnit)
            {
                this->m_value = newUnit.convertFrom(this->m_value, this->m_unit);
                this->m_unit = newUnit;
            }
            return *derived();
        }

        /*
         * Init by double
         */
        template <class MU, class PQ> void CPhysicalQuantity<MU, PQ>::setValueSameUnit(double baseValue)
        {
            this->m_value = baseValue;
        }

        /*
         * Value rounded in unit
         */
        template <class MU, class PQ> QString CPhysicalQuantity<MU, PQ>::valueRoundedWithUnit(const MU &unit, int digits, bool i18n) const
        {
            return unit.makeRoundedQStringWithUnit(this->value(unit), digits, i18n);
        }

        /*
         * Value rounded in unit
         */
        template <class MU, class PQ> double CPhysicalQuantity<MU, PQ>::valueRounded(const MU &unit, int digits) const
        {
            return unit.roundValue(this->value(unit), digits);
        }

        /*
         * Value in unit
         */
        template <class MU, class PQ> double CPhysicalQuantity<MU, PQ>::value(const MU &unit) const
        {
            return unit.convertFrom(this->m_value, this->m_unit);
        }

        /*
         * Convert to string
         */
        template <class MU, class PQ> QString CPhysicalQuantity<MU, PQ>::convertToQString(bool i18n) const
        {
            if (this->isNull())
            {
                return i18n ? QCoreApplication::translate("CPhysicalQuantity", "undefined") : "undefined";
            }
            return this->valueRoundedWithUnit(this->getUnit(), -1, i18n);
        }

        /*
         * Hash
         */
        template <class MU, class PQ> uint CPhysicalQuantity<MU, PQ>::getValueHash() const
        {
            QList<uint> hashs;
            // there is no double qHash
            // also unit and rounding has to be considered
            hashs << qHash(this->valueRoundedWithUnit(MU::defaultUnit()));
            return BlackMisc::calculateHash(hashs, "PQ");
        }

        /*
         * JSON Object
         */
        template <class MU, class PQ> QJsonObject CPhysicalQuantity<MU, PQ>::toJson() const
        {
            QJsonObject json;
            json.insert("value", QJsonValue(this->m_value));
            json.insert("unit", QJsonValue(this->m_unit.getSymbol()));
            return json;
        }

        /*
         * JSON Object
         */
        template <class MU, class PQ> void CPhysicalQuantity<MU, PQ>::convertFromJson(const QJsonObject &json)
        {
            const QString unitSymbol = json.value("unit").toString();
            this->setUnitBySymbol(unitSymbol);
            this->m_value = json.value("value").toDouble();
        }

        /*
         * Property
         */
        template <class MU, class PQ>  QVariant CPhysicalQuantity<MU, PQ>::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toQVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexValue:
                return QVariant(this->m_value);
            case IndexUnit:
                return this->m_unit.toQVariant();
            case IndexValueRounded0DigitsWithUnit:
                return QVariant(this->valueRoundedWithUnit(0));
            case IndexValueRounded1DigitsWithUnit:
                return QVariant(this->valueRoundedWithUnit(1));
            case IndexValueRounded2DigitsWithUnit:
                return QVariant(this->valueRoundedWithUnit(2));
            case IndexValueRounded3DigitsWithUnit:
                return QVariant(this->valueRoundedWithUnit(3));
            case IndexValueRounded6DigitsWithUnit:
                return QVariant(this->valueRoundedWithUnit(6));
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        /*
         * Property
         */
        template <class MU, class PQ> void CPhysicalQuantity<MU, PQ>::setPropertyByIndex(const QVariant &variant, const CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromQVariant(variant);
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexValue:
                this->m_value = variant.toDouble();
                break;
            case IndexUnit:
                this->m_unit.fromQVariant(variant);
                break;
            case IndexValueRounded0DigitsWithUnit:
            case IndexValueRounded1DigitsWithUnit:
            case IndexValueRounded2DigitsWithUnit:
            case IndexValueRounded3DigitsWithUnit:
            case IndexValueRounded6DigitsWithUnit:
                this->parseFromString(variant.toString());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

        /*
         * Compare
         */
        template <class MU, class PQ> int CPhysicalQuantity<MU, PQ>::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CPhysicalQuantity &>(otherBase);

            if (this->isNull() > other.isNull()) { return -1; }
            if (this->isNull() < other.isNull()) { return 1; }

            if (*this < other) { return -1; }
            else if (*this > other) { return 1; }
            else { return 0; }
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class CPhysicalQuantity<CLengthUnit, CLength>;
        template class CPhysicalQuantity<CPressureUnit, CPressure>;
        template class CPhysicalQuantity<CFrequencyUnit, CFrequency>;
        template class CPhysicalQuantity<CMassUnit, CMass>;
        template class CPhysicalQuantity<CTemperatureUnit, CTemperature>;
        template class CPhysicalQuantity<CSpeedUnit, CSpeed>;
        template class CPhysicalQuantity<CAngleUnit, CAngle>;
        template class CPhysicalQuantity<CTimeUnit, CTime>;
        template class CPhysicalQuantity<CAccelerationUnit, CAcceleration>;

    } // namespace
} // namespace
