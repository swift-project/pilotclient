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
#include "blackmisc/verify.h"

#include <Qt>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Network
    {
        const QList<int> &CServer::allServerTypes()
        {
            static const QList<int> all({ FSDServerVatsim, VoiceServerVatsim, FSDServer, VoiceServer, WebService, Unspecified });
            return all;
        }

        CServer::CServer(
            const QString &name, const QString &description, const QString &address, int port, const CUser &user,
            const CFsdSetup &setup, const CEcosystem &ecosytem, ServerType serverType, bool isAcceptingConnections)
            : m_name(name), m_description(description), m_address(address), m_port(port), m_user(user),
              m_fsdSetup(setup), m_ecosystem(ecosytem),
              m_serverType(serverType), m_isAcceptingConnections(isAcceptingConnections)
        {}

        CServer::CServer(
            const QString &address, int port, const CUser &user)
            : m_name("no name"), m_description("min.configuration"), m_address(address), m_port(port), m_user(user)
        {}

        CServer::CServer(const CEcosystem &ecosystem)
        {
            this->setEcosystem(ecosystem);
        }

        CServer::CServer(CServer::ServerType serverType)
        {
            this->setServerType(serverType);
        }

        QString CServer::convertToQString(bool i18n) const
        {
            static const QString str("%1 %2 %3:%4 %5 %6 accepting: %7 FSD: %8 con.since: %9");
            return str.
                   arg(m_name, m_description, m_address).arg(m_port).
                   arg(m_user.toQString(i18n), m_ecosystem.getSystemString(),
                       boolToYesNo(m_isAcceptingConnections), m_fsdSetup.toQString(i18n), this->isConnected() ? this->getFormattedUtcTimestampHms() : "not con.");
        }

        const CServer &CServer::swiftFsdTestServer(bool withPw)
        {
            // CUser("guest", "Guest Client project", "", "guest")
            static const CServer dvp("Testserver", "Client project testserver", "fsd.swift-project.org", 6809,
                                     CUser("1234567", "Test User", "", "123456"),
                                     CFsdSetup(), CEcosystem(CEcosystem::swiftTest()), CServer::FSDServerVatsim);
            static const CServer dvnp("Testserver", "Client project testserver", "fsd.swift-project.org", 6809,
                                      CUser("1234567", "Test User", "", ""),
                                      CFsdSetup(), CEcosystem(CEcosystem::swiftTest()), CServer::FSDServerVatsim);
            return withPw ? dvp : dvnp;
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

        bool CServer::setEcosystem(const CEcosystem &ecosystem)
        {
            if (m_ecosystem == ecosystem) { return false; } // avoid cross dependency
            m_ecosystem = ecosystem;

            // cross dependency
            if (ecosystem.isSystem(CEcosystem::VATSIM)) { m_serverType = FSDServerVatsim; }
            if (ecosystem.isSystem(CEcosystem::PrivateFSD)) { m_serverType = FSDServer; }
            if (ecosystem.isSystem(CEcosystem::SwiftTest)) { m_serverType = FSDServerVatsim; }
            return true;
        }

        bool CServer::isValidForLogin() const
        {
            return m_user.hasValidCredentials() && this->hasAddressAndPort() && this->isAcceptingConnections();
        }

        bool CServer::hasAddressAndPort() const
        {
            return m_port > 0 && !m_address.isEmpty();
        }

        bool CServer::isFsdServer() const
        {
            return (this->getServerType() == FSDServerVatsim ||
                    this->getServerType() == FSDServer);
        }

        bool CServer::setServerType(CServer::ServerType serverType)
        {
            if (m_serverType == serverType) { return false; } // avoid x-dependency
            m_serverType = static_cast<int>(serverType);
            switch (m_serverType)
            {
            case FSDServerVatsim : m_ecosystem = CEcosystem(CEcosystem::VATSIM); break;
            case FSDServer: m_ecosystem = CEcosystem(CEcosystem::PrivateFSD); break;
            case VoiceServerVatsim: m_ecosystem = CEcosystem(CEcosystem::VATSIM); break;
            case VoiceServer: m_ecosystem = CEcosystem(CEcosystem::PrivateFSD); break;
            case WebService: m_ecosystem = CEcosystem(CEcosystem::NoSystem); break;
            case Unspecified: m_ecosystem = CEcosystem(CEcosystem::Unspecified); break;
            default: break;
            }
            return true;
        }

        bool CServer::hasUnspecifiedServerType() const
        {
            return this->getServerType() == Unspecified;
        }

        const QString &CServer::getServerTypeAsString() const
        {
            return CServer::serverTypeToString(getServerType());
        }

        bool CServer::isConnected() const
        {
            return m_timestampMSecsSinceEpoch >= 0;
        }

        bool CServer::isNull() const
        {
            return this->hasUnspecifiedServerType() && !this->hasName() && m_port < 0;
        }

        CStatusMessageList CServer::validate() const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation()}));
            CStatusMessageList msgs;
            if (this->getName().isEmpty()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityError, "Missing name")); }
            if (this->getAddress().isEmpty()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityError, "Missing address")); }
            if (this->getDescription().isEmpty()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, "Missing description")); }
            if (this->getPort() < 1 || this->getPort() > 65535) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityError, "Wrong port")); }
            msgs.push_back(this->getUser().validate());
            msgs.push_back(this->getFsdSetup().validate());
            msgs.addCategories(cats);
            msgs.sortBySeverity();
            return msgs;
        }

        QString CServer::getServerSessionId() const
        {
            if (!this->isConnected()) { return ""; }
            static const QString session("%1 %2:%3 %4 %5");
            return session.arg(this->getName(), this->getAddress()).arg(this->getPort()).arg(this->getUser().getRealName(), this->getFormattedUtcTimestampHms());
        }

        CVariant CServer::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAddress: return CVariant::fromValue(m_address);
            case IndexDescription: return CVariant::fromValue(m_description);
            case IndexName: return CVariant::fromValue(m_name);
            case IndexPort: return CVariant::fromValue(m_port);
            case IndexUser: return m_user.propertyByIndex(index.copyFrontRemoved());
            case IndexFsdSetup: return m_fsdSetup.propertyByIndex(index.copyFrontRemoved());
            case IndexEcosystem: return m_ecosystem.propertyByIndex(index.copyFrontRemoved());
            case IndexIsAcceptingConnections: return CVariant::fromValue(m_isAcceptingConnections);
            case IndexServerType: return CVariant::fromValue(m_serverType);
            case IndexServerTypeAsString: return CVariant::fromValue(getServerTypeAsString());
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CServer::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CServer>(); return; }
            if (ITimestampBased::canHandleIndex(index)) { ITimestampBased::setPropertyByIndex(index, variant); return; }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAddress: this->setAddress(variant.value<QString>()); break;
            case IndexPort: this->setPort(variant.value<qint32>()); break;
            case IndexDescription: this->setDescription(variant.value<QString>()); break;
            case IndexName: this->setName(variant.value<QString>()); break;
            case IndexUser: m_user.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexFsdSetup: m_fsdSetup.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexEcosystem: m_ecosystem.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexIsAcceptingConnections: this->setIsAcceptingConnections(variant.value<bool>()); break;
            case IndexServerType: this->setServerType(static_cast<ServerType>(variant.toInt())); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        int CServer::comparePropertyByIndex(const CPropertyIndex &index, const CServer &compareValue) const
        {
            if (index.isMyself()) { return this->getName().compare(compareValue.getName()); }
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::comparePropertyByIndex(index, compareValue);}
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAddress: return this->getAddress().compare(compareValue.getAddress(), Qt::CaseInsensitive);
            case IndexDescription: return this->getDescription().compare(compareValue.getDescription(), Qt::CaseInsensitive);
            case IndexFsdSetup: return this->getFsdSetup().toQString().compare(compareValue.getFsdSetup().toQString());
            case IndexName: return this->getName().compare(compareValue.getName(), Qt::CaseInsensitive);
            case IndexIsAcceptingConnections: return Compare::compare(this->isAcceptingConnections(), compareValue.isAcceptingConnections());
            case IndexPort: return Compare::compare(this->getPort(), compareValue.getPort());
            case IndexUser: return this->getUser().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getUser());
            case IndexEcosystem: return this->getEcosystem().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getEcosystem());
            case IndexServerType:
            case IndexServerTypeAsString:
                return this->getServerTypeAsString().compare(compareValue.getServerTypeAsString(), Qt::CaseInsensitive);
            default: break;
            }
            BLACK_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable("No comparison for index " + index.toQString()));
            return 0;
        }

        const QString &CServer::serverTypeToString(CServer::ServerType server)
        {
            static const QString fsdVatsim("FSD [VATSIM]");
            static const QString voiceVatsim("voice [VATSIM]");
            static const QString fsdLegacy("FSD (legacy)");
            static const QString voice("voice");
            static const QString webService("web service");
            static const QString unspecified("unspecified");

            switch (server)
            {
            case FSDServerVatsim: return fsdVatsim;
            case VoiceServerVatsim: return voiceVatsim;
            case FSDServer: return fsdLegacy;
            case VoiceServer: return voice;
            case WebService: return webService;
            case Unspecified:
            default: return unspecified;
            }
        }
    } // namespace
} // namespace
