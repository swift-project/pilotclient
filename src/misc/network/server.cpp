// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/server.h"
#include "misc/logcategories.h"
#include "misc/stringutils.h"
#include "misc/obfuscation.h"
#include "misc/propertyindexref.h"
#include "misc/statusmessage.h"
#include "misc/comparefunctions.h"
#include "misc/obfuscation.h"
#include "misc/verify.h"

#include <Qt>
#include <QtGlobal>

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::network, CServer)

namespace swift::misc::network
{
    const QList<int> &CServer::allServerTypes()
    {
        static const QList<int> all({ FSDServerVatsim, VoiceServerVatsim, FSDServer, VoiceServer, WebService, Unspecified });
        return all;
    }

    CServer::CServer(const QString &name, const QString &description, const QString &address, int port, const CUser &user,
                     const CFsdSetup &fsdSetup, const CEcosystem &ecosytem, ServerType serverType, bool isAcceptingConnections)
        : m_name(CObfuscation::decode(name)), m_description(CObfuscation::decode(description)), m_address(CObfuscation::decode(address)), m_port(port), m_user(user),
          m_ecosystem(ecosytem),
          m_serverType(serverType), m_isAcceptingConnections(isAcceptingConnections),
          m_fsdSetup(fsdSetup)
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
        return QStringLiteral("%1 %2 %3:%4 %5 %6 accepting: %7 FSD: %8 con.since: %9").arg(m_name, m_description, m_address).arg(m_port).arg(m_user.toQString(i18n), m_ecosystem.getSystemString(), boolToYesNo(m_isAcceptingConnections), m_fsdSetup.toQString(i18n), this->isConnected() ? this->getFormattedUtcTimestampHms() : "not con.");
    }

    const CServer &CServer::fscFsdServer()
    {
        static const CServer fsc = [] {
            CServer s = CServer("FSC", "FSC e.V.", "OBF:AwJIKfgkQDJEIRnno29DJlB+UK0=", 6809,
                                CUser(),
                                CFsdSetup(), CEcosystem(CEcosystem::privateFsd()), CServer::FSDServer);
            s.removeSendReceiveDetails(CFsdSetup::AllInterimPositions);
            return s;
        }();
        return fsc;
    }

    const CServer &CServer::esTowerView()
    {
        static const CServer s = CServer("ES Tower", "Euroscope Tower view", "localhost", 6809,
                                         CUser(),
                                         CFsdSetup::vatsimStandard(), CEcosystem(CEcosystem::vatsim()), CServer::VoiceServerVatsim);
        return s;
    }

    bool CServer::matchesName(const QString &name) const
    {
        return m_name.length() == name.length() &&
               m_name.startsWith(name, Qt::CaseInsensitive);
    }

    bool CServer::matchesAddressPort(const CServer &server) const
    {
        return server.getPort() == this->getPort() &&
               server.matchesAddress(this->getAddress());
    }

    bool CServer::matchesAddress(const QString &address) const
    {
        return m_address.length() == address.length() &&
               m_address.startsWith(address, Qt::CaseInsensitive);
    }

    bool CServer::setEcosystem(const CEcosystem &ecosystem)
    {
        if (m_ecosystem == ecosystem) { return false; } // avoid cross dependency
        m_ecosystem = ecosystem;

        // cross dependency
        if (ecosystem.isSystem(CEcosystem::VATSIM)) { m_serverType = FSDServerVatsim; }
        if (ecosystem.isSystem(CEcosystem::SwiftTest)) { m_serverType = FSDServerVatsim; }
        return true;
    }

    bool CServer::isValidForLogin() const
    {
        return m_user.hasCredentials() && this->hasAddressAndPort() && this->isAcceptingConnections();
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
        // disabled x-dependency
        m_serverType = static_cast<int>(serverType);
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
        static const CLogCategoryList cats(CLogCategoryList(this).withValidation());
        CStatusMessageList msgs;
        if (this->getName().isEmpty()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityError, u"Missing name")); }
        if (this->getAddress().isEmpty()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityError, u"Missing address")); }
        if (this->getDescription().isEmpty()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, u"Missing description")); }
        if (this->getPort() < 1 || this->getPort() > 65535) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityError, u"Wrong port")); }
        msgs.push_back(this->getUser().validate());
        msgs.push_back(this->getFsdSetup().validate());
        msgs.addCategories(cats);
        msgs.sortBySeverity();
        return msgs;
    }

    QString CServer::getServerSessionId(bool onlyConnected) const
    {
        const bool isConnected = this->isConnected();
        if (onlyConnected && !isConnected) { return {}; }
        static const QString session("%1 %2 %3:%4 [%5] %6 %7");
        return session.arg(isConnected ? u"connected" : u"disconnected").arg(this->getName(), this->getAddress()).arg(this->getPort()).arg(this->getEcosystem().getSystemString(), this->getUser().getRealName(), this->getFormattedUtcTimestampHms());
    }

    QVariant CServer::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexAddress: return QVariant::fromValue(m_address);
        case IndexDescription: return QVariant::fromValue(m_description);
        case IndexName: return QVariant::fromValue(m_name);
        case IndexPort: return QVariant::fromValue(m_port);
        case IndexUser: return m_user.propertyByIndex(index.copyFrontRemoved());
        case IndexFsdSetup: return m_fsdSetup.propertyByIndex(index.copyFrontRemoved());
        case IndexEcosystem: return m_ecosystem.propertyByIndex(index.copyFrontRemoved());
        case IndexIsAcceptingConnections: return QVariant::fromValue(m_isAcceptingConnections);
        case IndexServerType: return QVariant::fromValue(m_serverType);
        case IndexServerTypeAsString: return QVariant::fromValue(getServerTypeAsString());
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CServer::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CServer>();
            return;
        }
        if (ITimestampBased::canHandleIndex(index))
        {
            ITimestampBased::setPropertyByIndex(index, variant);
            return;
        }

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
        case IndexServerType: this->setServerType(static_cast<ServerType>(variant.toInt())); break;
        case IndexIsAcceptingConnections: this->setIsAcceptingConnections(variant.value<bool>()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CServer::comparePropertyByIndex(CPropertyIndexRef index, const CServer &compareValue) const
    {
        if (index.isMyself()) { return this->getName().compare(compareValue.getName()); }
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::comparePropertyByIndex(index, compareValue); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexAddress: return this->getAddress().compare(compareValue.getAddress(), Qt::CaseInsensitive);
        case IndexDescription: return this->getDescription().compare(compareValue.getDescription(), Qt::CaseInsensitive);
        case IndexFsdSetup: return m_fsdSetup.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getFsdSetup());
        case IndexName: return this->getName().compare(compareValue.getName(), Qt::CaseInsensitive);
        case IndexPort: return Compare::compare(this->getPort(), compareValue.getPort());
        case IndexUser: return this->getUser().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getUser());
        case IndexEcosystem: return this->getEcosystem().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getEcosystem());
        case IndexIsAcceptingConnections: return Compare::compare(this->isAcceptingConnections(), compareValue.isAcceptingConnections());
        case IndexServerType:
        case IndexServerTypeAsString:
            return this->getServerTypeAsString().compare(compareValue.getServerTypeAsString(), Qt::CaseInsensitive);
        default: break;
        }
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable("No comparison for index " + index.toQString()));
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

    void CServer::setAddress(const QString &address)
    {
        m_address = CObfuscation::decode(address);
    }

    void CServer::setName(const QString &name)
    {
        m_name = CObfuscation::decode(name);
    }

    void CServer::setDescription(const QString &description)
    {
        m_description = CObfuscation::decode(description).simplified();
    }
} // namespace
