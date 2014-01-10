#include "avinformationmessage.h"
#include "blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Convert to string
         */
        QString CInformationMessage::convertToQString(bool /** i18n **/) const
        {
            return this->m_message;
        }

        /*
         * Marshall to DBus
         */
        void CInformationMessage::marshallToDbus(QDBusArgument &argument) const
        {
            argument << this->m_message;
            argument << static_cast<int>(this->m_type);
            argument << this->m_receivedTimestamp;
        }

        /*
         * Unmarshall from DBus
         */
        void CInformationMessage::unmarshallFromDbus(const QDBusArgument &argument)
        {
            uint type;
            argument >> this->m_message;
            argument >> type;
            argument >> this->m_receivedTimestamp;
            this->m_type = static_cast<InformationType>(type);
        }

        /*
         * Equal?
         */
        bool CInformationMessage::operator ==(const CInformationMessage &other) const
        {
            if (this == &other) return true;
            return this->m_message == other.m_message &&
                   this->m_receivedTimestamp == other.m_receivedTimestamp &&
                   this->m_type == other.m_type;
        }

        /*
         * Unequal?
         */
        bool CInformationMessage::operator !=(const CInformationMessage &other) const
        {
            return !((*this) == other);
        }

        /*
         * Register metadata
         */
        void CInformationMessage::registerMetadata()
        {
            qRegisterMetaType<CInformationMessage>();
            qDBusRegisterMetaType<CInformationMessage>();
        }

        /*
         * Hash
         */
        uint CInformationMessage::getValueHash() const
        {
            QList<uint> hashs;
            hashs << qHash(this->m_message);
            return BlackMisc::calculateHash(hashs, "CAtis");
        }

        /*
         * Type as string
         */
        const QString &CInformationMessage::getTypeAsString() const
        {
            switch (this->m_type)
            {
            case ATIS:
                {
                    static const QString atis("ATIS");
                    return atis;
                }
            case METAR:
                {
                    static const QString metar("METAR");
                    return metar;
                }
            case TAF:
                {
                    static const QString taf("TAF");
                    return taf;
                }
            default:
                {
                    static const QString ds("unknown");
                    return ds;
                }
            }
        }
    } // namespace
} // namespace
