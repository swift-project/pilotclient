/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/server.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"
#include <tuple>

namespace BlackMisc
{
    namespace Network
    {
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

        bool CServer::matchesName(const QString &name) const
        {
            return  m_name.length() == name.length() &&
                    m_name.startsWith(name, Qt::CaseInsensitive);
        }

        bool CServer::matchesAddressPort(const CServer &server) const
        {
            return server.getPort() == this->getPort() &&
                   server.matchesAddress(this->getAddress());
        }

        bool CServer::matchesAddress(const QString &address) const
        {
            return  m_address.length() == address.length() &&
                    m_address.startsWith(address, Qt::CaseInsensitive);
        }

        bool CServer::isValidForLogin() const
        {
            return this->m_user.hasValidCredentials() && this->hasAddressAndPort() && this->isAcceptingConnections();
        }

        bool CServer::hasAddressAndPort() const
        {
            return m_port > 0 && !m_address.isEmpty();
        }

        CStatusMessageList CServer::validate() const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation()}));
            CStatusMessageList msgs;
            if (this->getName().isEmpty()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, "Missing name")); }
            if (this->getAddress().isEmpty()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, "Missing address")); }
            if (this->getDescription().isEmpty()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, "Missing description")); }
            if (this->getPort() < 1 || this->getPort() > 65535) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, "Wrong port")); }
            msgs.push_back(this->getUser().validate());
            msgs.addCategories(cats);
            return msgs;
        }

        CVariant CServer::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAddress:
                return CVariant::fromValue(this->m_address);
            case IndexDescription:
                return CVariant::fromValue(this->m_description);
            case IndexName:
                return CVariant::fromValue(this->m_name);
            case IndexPort:
                return CVariant::fromValue(this->m_port);
            case IndexUser:
                return this->m_user.propertyByIndex(index.copyFrontRemoved());
            case IndexIsAcceptingConnections:
                return CVariant::fromValue(this->m_isAcceptingConnections);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CServer::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CServer>(); return; }
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
                this->m_user.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexIsAcceptingConnections:
                this->setIsAcceptingConnections(variant.value<bool>());
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

    } // namespace
} // namespace
