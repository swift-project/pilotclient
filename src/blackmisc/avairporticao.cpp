#include "avairporticao.h"

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Convert to string
         */
        QString CAirportIcao::convertToQString(bool /** i18n **/) const
        {
            return this->m_icaoCode;
        }

        /*
         * Equals callsign?
         */
        bool CAirportIcao::equalsString(const QString &icaoCode) const
        {
            CAirportIcao other(icaoCode);
            return other == (*this);
        }

        /*
         * Compare
         */
        int CAirportIcao::compareImpl(const CValueObject &otherBase) const
        {
            // intentionally compare on string only!
            const auto &other = static_cast<const CAirportIcao &>(otherBase);
            return this->m_icaoCode.compare(other.m_icaoCode, Qt::CaseInsensitive);
        }

        /*
         * Marshall to DBus
         */
        void CAirportIcao::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CAirportIcao>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CAirportIcao::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CAirportIcao>::toTuple(*this);
        }

        /*
         * To JSON
         */
        QJsonObject CAirportIcao::toJson() const
        {
            return BlackMisc::serializeJson(CAirportIcao::jsonMembers(), TupleConverter<CAirportIcao>::toTuple(*this));
        }

        /*
         * To JSON
         */
        void CAirportIcao::convertFromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CAirportIcao::jsonMembers(), TupleConverter<CAirportIcao>::toTuple(*this));
        }

        /*
         * Members
         */
        const QStringList &CAirportIcao::jsonMembers()
        {
            return TupleConverter<CAirportIcao>::jsonMembers();
        }

        /*
         * Unify ICAO code
         */
        QString CAirportIcao::unifyAirportCode(const QString &icaoCode)
        {
            QString code = icaoCode.trimmed().toUpper();
            if (code.length() != 4) return "";
            QRegExp reg("[A-Z]{4}");
            return (reg.exactMatch(code)) ? code : "";
        }

        /*
         * Valid ICAO designator?
         */
        bool CAirportIcao::isValidIcaoDesignator(const QString &icaoCode)
        {
            QString icao = unifyAirportCode(icaoCode);
            return icao.length() == 4;
        }

        /*
         * Equal?
         */
        bool CAirportIcao::operator ==(const CAirportIcao &other) const
        {
            if (this == &other) return true;
            // intentionally not via Tupel converter, compare on string only
            return this->asString().compare(other.asString(), Qt::CaseInsensitive) == 0;
        }

        /*
         * Unequal?
         */
        bool CAirportIcao::operator !=(const CAirportIcao &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CAirportIcao::getValueHash() const
        {
            return qHash(TupleConverter<CAirportIcao>::toTuple(*this));
        }

        /*
         * Less than?
         */
        bool CAirportIcao::operator <(const CAirportIcao &other) const
        {
            return this->m_icaoCode < other.m_icaoCode;
        }

        /*
         * metaTypeId
         */
        int CAirportIcao::getMetaTypeId() const
        {
            return qMetaTypeId<CAirportIcao>();
        }

        /*
         * is a
         */
        bool CAirportIcao::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CAirportIcao>()) { return true; }

            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Register metadata
         */
        void CAirportIcao::registerMetadata()
        {
            qRegisterMetaType<CAirportIcao>();
            qDBusRegisterMetaType<CAirportIcao>();
        }

    } // namespace
} // namespace
