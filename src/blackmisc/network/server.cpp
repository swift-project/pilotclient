/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/server.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/variant.h"

#include <Qt>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Network
    {
        CServer::CServer(const QString &name, const QString &description, const QString &address, int port, const CUser &user, bool isAcceptingConnections)
            : m_name(name), m_description(description), m_address(address), m_port(port), m_user(user), m_isAcceptingConnections(isAcceptingConnections) {}

        QString CServer::convertToQString(bool i18n) const
        {
            QString s(this->m_name);
            s.append(' ').append(this->m_description);
            s.append(' ').append(this->m_address);
            s.append(' ').append(QString::number(this->m_port));
            s.append(' ').append(this->m_user.toQString(i18n));
            s.append(' ').append(boolToYesNo(this->m_isAcceptingConnections));
            s.append(' ').append(this->m_fsdSetup.toQString(i18n));

            if (this->isConnected())
            {
                s.append(" connected: ").append(this->getFormattedUtcTimestampHms());
            }
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

        bool CServer::isConnected() const
        {
            return this->m_timestampMSecsSinceEpoch >= 0;
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
            msgs.push_back(this->getFsdSetup().validate());
            msgs.addCategories(cats);
            return msgs;
        }

        QString CServer::getServerSessionId() const
        {
            if (!this->isConnected()) { return ""; }
            const QString session("%1 %2:%3 %4 %5");
            return session.arg(this->getName(), this->getAddress()).arg(this->getPort()).arg(this->getUser().getRealName(), this->getFormattedUtcTimestampHms());
        }

        CVariant CServer::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
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
            case IndexFsdSetup:
                return this->m_fsdSetup.propertyByIndex(index.copyFrontRemoved());
            case IndexIsAcceptingConnections:
                return CVariant::fromValue(this->m_isAcceptingConnections);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CServer::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CServer>(); return; }
            if (ITimestampBased::canHandleIndex(index)) { ITimestampBased::setPropertyByIndex(index, variant); return; }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
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
            case IndexFsdSetup:
                this->m_fsdSetup.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexIsAcceptingConnections:
                this->setIsAcceptingConnections(variant.value<bool>());
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        int CServer::comparePropertyByIndex(const CPropertyIndex &index, const CServer &compareValue) const
        {
            if (index.isMyself()) { return this->getName().compare(compareValue.getName()); }
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::comparePropertyByIndex(index, compareValue);}
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAddress:
                return this->getAddress().compare(compareValue.getAddress(), Qt::CaseInsensitive);
            case IndexDescription:
                return this->getDescription().compare(compareValue.getDescription(), Qt::CaseInsensitive);
            case IndexFsdSetup:
                return this->getFsdSetup().toQString().compare(compareValue.getFsdSetup().toQString());
            case IndexName:
                return this->getName().compare(compareValue.getName(), Qt::CaseInsensitive);
            case IndexIsAcceptingConnections:
                return Compare::compare(this->isAcceptingConnections(), compareValue.isAcceptingConnections());
            case IndexPort:
                return Compare::compare(this->getPort(), compareValue.getPort());
            case IndexUser:
                return this->getUser().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getUser());
            default:
                break;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "No compare function");
            return 0;
        }
    } // namespace
} // namespace
