#include "pqstring.h"
#include "tuple.h"
#include "pqallquantities.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        /*
         * Convert to string
         */
        QString CPqString::convertToQString(bool /** i18n **/) const
        {
            return this->m_string;
        }

        /*
         * is a
         */
        bool CPqString::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CPqString>()) { return true; }
            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Meta id
         */
        int CPqString::getMetaTypeId() const
        {
            return qMetaTypeId<CPqString>();
        }

        /*
         * Compare
         */
        int CPqString::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CPqString &>(otherBase);
            return compare(TupleConverter<CPqString>::toTuple(*this), TupleConverter<CPqString>::toTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void CPqString::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CPqString>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CPqString::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CPqString>::toTuple(*this);
        }

        /*
         * Equal?
         */
        bool CPqString::operator ==(const CPqString &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CPqString>::toTuple(*this) == TupleConverter<CPqString>::toTuple(other);
        }

        /*
         * Unequal?
         */
        bool CPqString::operator !=(const CPqString &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CPqString::getValueHash() const
        {
            return qHash(TupleConverter<CPqString>::toTuple(*this));
        }

        /*
         * Register metadata
         */
        void CPqString::registerMetadata()
        {
            qRegisterMetaType<CPqString>();
            qDBusRegisterMetaType<CPqString>();
        }

        QVariant CPqString::parse(const QString &value)
        {
            QVariant v;
            if (value.isEmpty()) return v;
            QRegExp rx("^([-+]?[0-9]*\\.?[0-9]+)\\s*(\\D*)$");
            if (rx.indexIn(value) < 0) return v;
            QString number = rx.cap(1);
            QString unit = rx.cap(2);
            if (unit.isEmpty() || number.isEmpty()) return v;
            bool success;
            double numberD = number.toDouble(&success);
            if (!success) return v;

            if (CMeasurementUnit::isValidUnitSymbol<CAccelerationUnit>(unit))
            {
                CAcceleration pq(numberD, CMeasurementUnit::unitFromSymbol<CAccelerationUnit>(unit, false));
                return pq.toQVariant();
            }

            if (CMeasurementUnit::isValidUnitSymbol<CAngleUnit>(unit))
            {
                CAngle pq(numberD, CMeasurementUnit::unitFromSymbol<CAngleUnit>(unit, false));
                return pq.toQVariant();
            }

            if (CMeasurementUnit::isValidUnitSymbol<CFrequencyUnit>(unit))
            {
                CFrequency pq(numberD, CMeasurementUnit::unitFromSymbol<CFrequencyUnit>(unit, false));
                return pq.toQVariant();
            }

            if (CMeasurementUnit::isValidUnitSymbol<CLengthUnit>(unit))
            {
                CLength pq(numberD, CMeasurementUnit::unitFromSymbol<CLengthUnit>(unit, false));
                return pq.toQVariant();
            }

            if (CMeasurementUnit::isValidUnitSymbol<CMassUnit>(unit))
            {
                CMass pq(numberD, CMeasurementUnit::unitFromSymbol<CMassUnit>(unit, false));
                return pq.toQVariant();
            }

            if (CMeasurementUnit::isValidUnitSymbol<CPressureUnit>(unit))
            {
                CPressure pq(numberD, CMeasurementUnit::unitFromSymbol<CPressureUnit>(unit, false));
                return pq.toQVariant();
            }

            if (CMeasurementUnit::isValidUnitSymbol<CSpeedUnit>(unit))
            {
                CSpeed pq(numberD, CMeasurementUnit::unitFromSymbol<CSpeedUnit>(unit, false));
                return pq.toQVariant();
            }

            if (CMeasurementUnit::isValidUnitSymbol<CTimeUnit>(unit))
            {
                CTime pq(numberD, CMeasurementUnit::unitFromSymbol<CTimeUnit>(unit, false));
                return pq.toQVariant();
            }

            if (CMeasurementUnit::isValidUnitSymbol<CTemperatureUnit>(unit))
            {
                CTemperature pq(numberD, CMeasurementUnit::unitFromSymbol<CTemperatureUnit>(unit, false));
                return pq.toQVariant();
            }
            return v;
        }

    } // namespace
} // namespace
