/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "nwserver.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/propertyindex.h"
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
            s.append(" ").append(this->m_isAcceptingConnections ? "true" : "false");
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
            return compare(TupleConverter<CServer>::toTuple(*this), TupleConverter<CServer>::toTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void CServer::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CServer>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CServer::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CServer>::toTuple(*this);
        }

        /*
         * Valid for login
         */
        bool CServer::isValidForLogin() const
        {
            return this->m_user.hasValidCredentials() && this->m_port > 0 && !this->m_address.isEmpty() && this->isAcceptingConnections();
        }

        /*
         * Equal?
         */
        bool CServer::operator ==(const CServer &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CServer>::toTuple(*this) == TupleConverter<CServer>::toTuple(other);
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
            return qHash(TupleConverter<CServer>::toTuple(*this));
        }

        /*
         * Property by index
         */
        QVariant CServer::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toQVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAddress:
                return QVariant::fromValue(this->m_address);
            case IndexDescription:
                return QVariant::fromValue(this->m_description);
            case IndexName:
                return QVariant::fromValue(this->m_name);
            case IndexPort:
                return QVariant::fromValue(this->m_port);
            case IndexUser:
                return this->m_user.propertyByIndex(index.copyFrontRemoved());
            case IndexIsAcceptingConnections:
                return QVariant::fromValue(this->m_isAcceptingConnections);
            default:
                break;
            }

            Q_ASSERT_X(false, "CServer", "index unknown");
            QString m = QString("no property, index ").append(index.toQString());
            return QVariant::fromValue(m);
        }

        /*
         * Property by index (setter)
         */
        void CServer::setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->fromQVariant(variant);
                return;
            }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
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
            case IndexUser:
                this->m_user.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexIsAcceptingConnections:
                this->setIsAcceptingConnections(variant.value<bool>());
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

        /*
         * Members
         */
        const QStringList &CServer::jsonMembers()
        {
            return TupleConverter<CServer>::jsonMembers();
        }

        /*
         * To JSON
         */
        QJsonObject CServer::toJson() const
        {
            return BlackMisc::serializeJson(CServer::jsonMembers(), TupleConverter<CServer>::toTuple(*this));
        }

        /*
         * From Json
         */
        void CServer::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CServer::jsonMembers(), TupleConverter<CServer>::toTuple(*this));
        }

    } // namespace
} // namespace
