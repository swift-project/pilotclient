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
            QString s(this->m_designator);
            s.append(" ").append(this->m_type);
            if (this->hasAirline()) s.append(" ").append(this->m_airline);
            if (this->hasAirline()) s.append(" ").append(this->m_airline);
            if (this->hasLivery()) s.append(" ").append(this->m_livery);
            if (this->hasColor()) s.append(" ").append(this->m_color);
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

            const auto lhs = std::tie(this->m_designator, this->m_color, this->m_airline, this->m_livery);
            const auto rhs = std::tie(other.m_designator, other.m_color, other.m_airline, other.m_livery);

            if (lhs < rhs) { return -1; }
            if (lhs > rhs) { return 1; }
            return 0;
        }

        /*
         * Marshall to DBus
         */
        void CAircraftIcao::marshallToDbus(QDBusArgument &argument) const
        {
            argument << this->m_designator;
            argument << this->m_airline;
            argument << this->m_livery;
            argument << this->m_type;
            argument << this->m_color;
        }

        /*
         * Unmarshall from DBus
         */
        void CAircraftIcao::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> this->m_designator;
            argument >> this->m_airline;
            argument >> this->m_livery;
            argument >> this->m_type;
            argument >> this->m_color;
        }

        /*
         * Equal?
         */
        QString CAircraftIcao::asString() const
        {
            if (this->m_designator.isEmpty()) return "";
            QString s(this->m_designator);
            if (!this->m_airline.isEmpty())
            {
                s.append(" (").append(this->m_airline).append(")");
                return s;
            }
            if (!this->m_color.isEmpty())
            {
                s.append(" (").append(this->m_color).append(")");
                return s;
            }
            return s;
        }

        bool CAircraftIcao::operator ==(const CAircraftIcao &other) const
        {
            if (this == &other) return true;
            return this->getValueHash() == other.getValueHash();
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
            QList<uint> hashs;
            hashs << qHash(this->m_designator);
            hashs << qHash(this->m_airline);
            hashs << qHash(this->m_type);
            hashs << qHash(this->m_color);
            return BlackMisc::calculateHash(hashs, "CAircraftIcao");
        }

        /*
         * Property by index
         */
        QVariant CAircraftIcao::propertyByIndex(int index) const
        {
            switch (index)
            {
            case IndexIcaoDesignator:
                return QVariant::fromValue(this->m_designator);
            case IndexAirline:
                return QVariant::fromValue(this->m_airline);
            case IndexType:
                return QVariant::fromValue(this->m_type);
            case IndexColor:
                return QVariant::fromValue(this->m_color);
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
            case IndexIcaoDesignator:
                this->setDesignator(variant.value<QString>());
                break;
            case IndexAirline:
                this->setAirline(variant.value<QString>());
                break;
            case IndexType:
                this->setType(variant.value<QString>());
                break;
            case IndexColor:
                this->setColor(variant.value<QString>());
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
