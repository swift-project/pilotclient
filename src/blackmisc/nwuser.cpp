#include "nwuser.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <tuple>

namespace BlackMisc
{
    namespace Network
    {
        /*
         * Convert to string
         */
        QString CUser::convertToQString(bool /** i18n **/) const
        {
            if (this->m_realname.isEmpty()) return "<no realname>";
            QString s = this->m_realname;
            if (this->hasValidId())
            {
                s.append(" (").append(this->m_id).append(')');
            }
            if (this->hasValidCallsign())
            {
                s.append(this->getCallsign().getStringAsSet());
            }
            return s;
        }

        /*
         * metaTypeId
         */
        int CUser::getMetaTypeId() const
        {
            return qMetaTypeId<CUser>();
        }

        /*
         * is a
         */
        bool CUser::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CUser>()) { return true; }

            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CUser::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CUser &>(otherBase);

            const auto lhs = std::tie(this->m_id, this->m_realname, this->m_email, this->m_password);
            const auto rhs = std::tie(other.m_id, other.m_realname, other.m_email, other.m_password);

            if (lhs < rhs) { return -1; }
            if (lhs > rhs) { return 1; }
            return 0;
        }

        /*
         * Marshall to DBus
         */
        void CUser::marshallToDbus(QDBusArgument &argument) const
        {
            argument << this->m_id;
            argument << this->m_realname;
            argument << this->m_email;
            argument << this->m_password;
            argument << this->m_callsign;
        }

        /*
         * Unmarshall from DBus
         */
        void CUser::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> this->m_id;
            argument >> this->m_realname;
            argument >> this->m_email;
            argument >> this->m_password;
            argument >> this->m_callsign;
        }

        /*
         * Equal?
         */
        bool CUser::operator ==(const CUser &other) const
        {
            if (this == &other) return true;
            return (this->m_id == other.m_id &&
                    this->m_realname == other.m_realname &&
                    this->m_email == other.m_email &&
                    this->m_callsign == other.m_callsign);
        }

        /*
         * Exchange data
         */
        void CUser::syncronizeData(CUser &otherUser)
        {
            if (otherUser == (*this)) return;

            if (this->hasValidRealName())
                otherUser.setRealName(this->getRealName());
            else if (otherUser.hasValidRealName())
                this->setRealName(otherUser.getRealName());

            if (this->hasValidId())
                otherUser.setId(this->getId());
            else if (otherUser.hasValidId())
                this->setId(otherUser.getId());

            if (this->hasValidEmail())
                otherUser.setEmail(this->getEmail());
            else if (otherUser.hasValidEmail())
                this->setEmail(otherUser.getEmail());

            if (this->hasValidCallsign())
                otherUser.setCallsign(this->getCallsign());
            else if (otherUser.hasValidCallsign())
                this->setCallsign(otherUser.getCallsign());
        }

        /*
         * Unequal?
         */
        bool CUser::operator !=(const CUser &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CUser::getValueHash() const
        {
            QList<uint> hashs;
            hashs << qHash(this->m_id);
            hashs << qHash(this->m_realname);
            hashs << qHash(this->m_email);
            hashs << qHash(this->m_callsign.getValueHash());
            return BlackMisc::calculateHash(hashs, "CUser");
        }

        /*
         * Register metadata
         */
        void CUser::registerMetadata()
        {
            qRegisterMetaType<CUser>();
            qDBusRegisterMetaType<CUser>();
        }

        /*
         * Property by index
         */
        QVariant CUser::propertyByIndex(int index) const
        {
            switch (index)
            {
            case IndexEmail:
                return QVariant(this->m_email);
            case IndexId:
                return QVariant(this->m_id);
            case IndexPassword:
                return QVariant(this->m_password);
            case IndexRealName:
                return QVariant(this->m_realname);
            case IndexCallsign:
                return this->m_callsign.toQVariant();
            case IndexCallsignIcon:
                return this->m_callsign.toIcon();
            default:
                break;
            }

            Q_ASSERT_X(false, "CUser", "index unknown");
            QString m = QString("no property, index ").append(QString::number(index));
            return QVariant::fromValue(m);
        }

        /*
         * Set property as index
         */
        void CUser::setPropertyByIndex(const QVariant &variant, int index)
        {
            switch (index)
            {
            case IndexEmail:
                this->setEmail(variant.value<QString>());
                break;
            case IndexId:
                this->setId(variant.value<QString>());
                break;
            case IndexPassword:
                this->setPassword(variant.value<QString>());
                break;
            case IndexRealName:
                this->setRealName(variant.value<QString>());
                break;
            case IndexCallsign:
                this->setCallsign(variant.value<BlackMisc::Aviation::CCallsign>());
                break;
            default:
                Q_ASSERT_X(false, "CUser", "index unknown (setter)");
                break;
            }
        }
    } // namespace
} // namespace
