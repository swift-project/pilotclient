#include "avaircrafticao.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <tuple>

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Convert to string
         */
        QString CAircraftIcao::convertToQString(bool /** i18n **/) const
        {
            QString s(this->m_aircraftDesignator);
            s.append(" ").append(this->m_aircraftCombinedType);
            if (this->hasAirlineDesignator()) s.append(" ").append(this->m_airlineDesignator);
            if (this->hasLivery()) s.append(" ").append(this->m_livery);
            if (this->hasAircraftColor()) s.append(" ").append(this->m_aircraftColor);
            return s;
        }

        /*
         * metaTypeId
         */
        int CAircraftIcao::getMetaTypeId() const
        {
            return qMetaTypeId<CAircraftIcao>();
        }

        /*
         * is a
         */
        bool CAircraftIcao::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CAircraftIcao>()) { return true; }
            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CAircraftIcao::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CAircraftIcao &>(otherBase);
            return compare(TupleConverter<CAircraftIcao>::toTuple(*this), TupleConverter<CAircraftIcao>::toTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void CAircraftIcao::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CAircraftIcao>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CAircraftIcao::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CAircraftIcao>::toTuple(*this);
        }

        /*
         * As string?
         */
        QString CAircraftIcao::asString() const
        {
            if (this->m_aircraftDesignator.isEmpty()) return "";
            QString s(this->m_aircraftDesignator);
            if (!this->m_airlineDesignator.isEmpty())
            {
                s.append(" (").append(this->m_airlineDesignator).append(")");
                return s;
            }
            if (!this->m_aircraftColor.isEmpty())
            {
                s.append(" (").append(this->m_aircraftColor).append(")");
                return s;
            }
            return s;
        }

        bool CAircraftIcao::operator ==(const CAircraftIcao &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CAircraftIcao>::toTuple(*this) == TupleConverter<CAircraftIcao>::toTuple(other);
        }

        /*
         * Unequal?
         */
        bool CAircraftIcao::operator !=(const CAircraftIcao &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CAircraftIcao::getValueHash() const
        {
            return qHash(TupleConverter<CAircraftIcao>::toTuple(*this));
        }

        /*
         * Property by index
         */
        QVariant CAircraftIcao::propertyByIndex(int index) const
        {
            switch (index)
            {
            case IndexAircraftDesignator:
                return QVariant::fromValue(this->m_aircraftDesignator);
            case IndexAirlineDesignator:
                return QVariant::fromValue(this->m_airlineDesignator);
            case IndexCombinedAircraftType:
                return QVariant::fromValue(this->m_aircraftCombinedType);
            case IndexAircraftColor:
                return QVariant::fromValue(this->m_aircraftColor);
            case IndexAsString:
                return QVariant::fromValue(this->asString());
            default:
                break;
            }

            Q_ASSERT_X(false, "CAircraftIcao", "index unknown");
            QString m = QString("no property, index ").append(QString::number(index));
            return QVariant::fromValue(m);
        }

        /*
         * Property as string by index
         */
        QString CAircraftIcao::propertyByIndexAsString(int index, bool i18n) const
        {
            QVariant qv = this->propertyByIndex(index);
            return BlackMisc::qVariantToString(qv, i18n);
        }

        /*
         * Property by index
         */
        void CAircraftIcao::setPropertyByIndex(const QVariant &variant, int index)
        {
            switch (index)
            {
            case IndexAircraftDesignator:
                this->setAircraftDesignator(variant.value<QString>());
                break;
            case IndexAirlineDesignator:
                this->setAirlineDesignator(variant.value<QString>());
                break;
            case IndexCombinedAircraftType:
                this->setAircraftCombinedType(variant.value<QString>());
                break;
            case IndexAircraftColor:
                this->setAircraftColor(variant.value<QString>());
                break;
            default:
                Q_ASSERT_X(false, "CAircraftIcao", "index unknown");
                break;
            }
        }

        /*
         * Register metadata
         */
        void CAircraftIcao::registerMetadata()
        {
            qRegisterMetaType<CAircraftIcao>();
            qDBusRegisterMetaType<CAircraftIcao>();
        }

    } // namespace
} // namespace
