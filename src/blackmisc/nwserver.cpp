#include "nwserver.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <tuple>

namespace BlackMisc
{
    namespace Network
    {
        /*
         * Convert to string
         */
        QString CServer::convertToQString(bool i18n) const
        {
            QString s(this->m_name);
            s.append(" ").append(this->m_description);
            s.append(" ").append(this->m_address);
            s.append(" ").append(QString::number(this->m_port));
            s.append(" ").append(this->m_user.toQString(i18n));
            return s;
        }

        /*
         * metaTypeId
         */
        int CServer::getMetaTypeId() const
        {
            return qMetaTypeId<CServer>();
        }

        /*
         * is a
         */
        bool CServer::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CServer>()) { return true; }

            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CServer::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CServer &>(otherBase);

            const auto lhs = std::tie(this->m_name, this->m_description, this->m_address, this->m_port);
            const auto rhs = std::tie(other.m_name, other.m_description, other.m_address, other.m_port);

            if (lhs < rhs) { return -1; }
            if (lhs > rhs) { return 1; }
            return compare(this->m_user, other.m_user);
        }

        /*
         * Marshall to DBus
         */
        void CServer::marshallToDbus(QDBusArgument &argument) const
        {
            argument << this->m_name;
            argument << this->m_description;
            argument << this->m_address;
            argument << this->m_port;
            argument << this->m_user;
        }

        /*
         * Unmarshall from DBus
         */
        void CServer::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> this->m_name;
            argument >> this->m_description;
            argument >> this->m_address;
            argument >> this->m_port;
            argument >> this->m_user;
        }

        /*
         * Valid for login
         */
        bool CServer::isValidForLogin() const
        {
            return this->m_user.hasValidCredentials() && this->m_port > 0 && !this->m_address.isEmpty();
        }

        /*
         * Equal?
         */
        bool CServer::operator ==(const CServer &other) const
        {
            if (this == &other) return true;
            return this->getValueHash() == other.getValueHash();
        }

        /*
         * Unequal?
         */
        bool CServer::operator !=(const CServer &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CServer::getValueHash() const
        {
            QList<uint> hashs;
            hashs << qHash(this->m_name);
            hashs << qHash(this->m_address);
            hashs << qHash(this->m_port);
            hashs << qHash(this->m_user.getValueHash());
            return BlackMisc::calculateHash(hashs, "CServer");
        }

        /*
         * Property by index
         */
        QVariant CServer::propertyByIndex(int index) const
        {
            switch (index)
            {
            case IndexAddress:
                return QVariant::fromValue(this->m_address);
            case IndexDescription:
                return QVariant::fromValue(this->m_description);
            case IndexName:
                return QVariant::fromValue(this->m_name);
            case IndexPort:
                return QVariant::fromValue(this->m_port);
            case IndexUserId:
                return QVariant::fromValue(this->m_user.getId());
            case IndexUserRealName:
                return QVariant::fromValue(this->m_user.getRealName());
            default:
                break;
            }

            Q_ASSERT_X(false, "CServer", "index unknown");
            QString m = QString("no property, index ").append(QString::number(index));
            return QVariant::fromValue(m);
        }

        /*
         * Property as string by index
         */
        QString CServer::propertyByIndexAsString(int index, bool i18n) const
        {
            QVariant qv = this->propertyByIndex(index);
            return BlackMisc::qVariantToString(qv, i18n);
        }

        /*
         * Property by index (setter)
         */
        void CServer::propertyByIndex(const QVariant &variant, int index)
        {
            switch (index)
            {
            case IndexAddress:
                this->setAddress(variant.value<QString>());
                break;
            case IndexPort:
                this->setPort(variant.value<qint32>());
                break;
            case IndexDescription:
                this->setDescription(variant.value<QString>());
                break;
            case IndexName:
                this->setName(variant.value<QString>());
                break;
            case IndexUserId:
                this->m_user.setId(variant.value<QString>());
                break;
            case IndexUserPassword:
                this->m_user.setPassword(variant.value<QString>());
                break;
            case IndexUserRealName:
                this->m_user.setRealName(variant.value<QString>());
                break;
            default:
                Q_ASSERT_X(false, "CServer", "index unknown");
                break;
            }
        }



        /*
         * Register metadata
         */
        void CServer::registerMetadata()
        {
            qRegisterMetaType<CServer>();
            qDBusRegisterMetaType<CServer>();
        }

    } // namespace
} // namespace
