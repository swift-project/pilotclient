#include "nwuser.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Network
    {
        /*
         * Convert to string
         */
        QString CUser::convertToQString(bool /** i18n **/) const
        {
            if (this->m_realname.isEmpty()) return "Unknown??";
            QString s = this->m_realname;
            if (this->m_id >= 0)
            {
                s = s.append(" (").append(this->m_id).append(')');
            }
            return s;
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
        }

        /*
         * Equal?
         */
        bool CUser::operator ==(const CUser &other) const
        {
            if (this == &other) return true;
            return (this->m_id == other.m_id &&
                    this->m_realname == other.m_realname &&
                    this->m_email == other.m_email);
        }

        /*
         * Exchange data
         */
        void CUser::syncronizeData(CUser &otherUser)
        {
            if (otherUser == (*this)) return;

            if (this->hasValidRealname())
                otherUser.setRealname(this->getRealname());
            else if (otherUser.hasValidRealname())
                this->setRealname(otherUser.getRealname());

            if (this->hasValidId())
                otherUser.setId(this->getId());
            else if (otherUser.hasValidId())
                this->setId(otherUser.getId());

            if (this->hasValidEmail())
                otherUser.setEmail(this->getEmail());
            else if (otherUser.hasValidEmail())
                this->setEmail(otherUser.getEmail());
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

    } // namespace
} // namespace
