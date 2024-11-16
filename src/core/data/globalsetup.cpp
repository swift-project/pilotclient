// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/data/globalsetup.h"

#include <QJsonObject>
#include <QStringBuilder>
#include <QStringList>
#include <QSysInfo>
#include <QVersionNumber>

#include "config/buildconfig.h"
#include "core/application.h"
#include "misc/crashhandler.h"
#include "misc/directoryutils.h"
#include "misc/json.h"
#include "misc/network/server.h"
#include "misc/network/user.h"
#include "misc/stringutils.h"
#include "misc/swiftdirectories.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::json;
using namespace swift::misc::network;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::core::data, CGlobalSetup)

namespace swift::core::data
{
    CGlobalSetup::CGlobalSetup() : CIdentifiable("CGlobalSetup")
    {}

    CUrl CGlobalSetup::getDbIcaoReaderUrl() const
    {
        return getDbRootDirectoryUrl();
    }

    CUrl CGlobalSetup::getDbModelReaderUrl() const
    {
        return getDbRootDirectoryUrl();
    }

    CUrl CGlobalSetup::getDbAirportReaderUrl() const
    {
        return getDbRootDirectoryUrl();
    }

    CUrl CGlobalSetup::getDbInfoReaderUrl() const
    {
        return getDbRootDirectoryUrl();
    }

    const CUrl &CGlobalSetup::getSwiftSharedUrl() const
    {
        return m_sharedUrl;
    }
    CUrl CGlobalSetup::getDbHomePageUrl() const
    {
        return getDbRootDirectoryUrl().withAppendedPath("/page/index.php");
    }

    CUrl CGlobalSetup::getHelpPageUrl(const QString &context) const
    {
        // we display in the standard browser,
        // so the user will realize if the URL does not work
        CUrl url = m_onlineHelpUrl;
        if (url.isEmpty()) { return url; }

        // context string something like "application.moreSpecific.evenMoreSpecific"
        QString c = "client";
        if (QCoreApplication::instance())
        {
            c = QCoreApplication::instance()->applicationName();
        }
        if (!context.isEmpty()) { c += "." + context; }
        url.appendQuery("context", c);
        return url;
    }

    CUrl CGlobalSetup::getLegalDirectoryUrl() const
    {
        return getDbRootDirectoryUrl().withAppendedPath("/legal/");
    }

    CUrl CGlobalSetup::getDbLoginServiceUrl() const
    {
        return getDbRootDirectoryUrl().withAppendedPath("/service/jsonauthenticate.php").withSwitchedScheme("https", m_dbHttpsPort);
    }

    bool CGlobalSetup::dbDebugFlag() const
    {
        return m_dbDebugFlag && CBuildConfig::isLocalDeveloperDebugBuild();
    }

    void CGlobalSetup::setServerDebugFlag(bool debug)
    {
        m_dbDebugFlag = debug;
    }

    CUrl CGlobalSetup::getSharedDbDataDirectoryUrl()
    {
        return m_sharedUrl.withAppendedPath(CGlobalSetup::schemaVersionString() + "/dbdata/");
    }

    CServerList CGlobalSetup::getPredefinedServersPlusHardcodedServers() const
    {
        static const CServerList hardcoded(
            { CServer::fscFsdServer(),
              CServer::esTowerView() });
        CServerList testServers(m_predefinedServers);
        testServers.addIfAddressNotExists(hardcoded);
        return testServers;
    }

    bool CGlobalSetup::isSwiftVersionMinimumMappingVersion() const
    {
        if (m_mappingMinimumVersion.isEmpty()) { return false; }
        const QVersionNumber min = QVersionNumber::fromString(this->getMappingMinimumVersionString());
        return CBuildConfig::getVersion() >= min;
    }

    QString CGlobalSetup::convertToQString(bool i18n) const
    {
        return convertToQString(", ", i18n);
    }

    QString CGlobalSetup::convertToQString(const QString &separator, bool i18n) const
    {
        QString s =
            u"Global setup loaded: " % separator % u"Mapping min.version: " % this->getMappingMinimumVersionString() % separator

            % u"Help URL: " % m_onlineHelpUrl.toQString(i18n) % separator;
        s +=
            u"DB root directory: " % getDbRootDirectoryUrl().toQString(i18n) % separator % u"ICAO DB reader: " % getDbIcaoReaderUrl().toQString(i18n) % separator % u"Model DB reader: " % getDbModelReaderUrl().toQString(i18n) % separator % u"Airport DB reader: " % getDbAirportReaderUrl().toQString(i18n) % separator % u"DB home page: " % getDbHomePageUrl().toQString(i18n) % separator % u"DB login service: " % getDbLoginServiceUrl().toQString(i18n) % separator;
        s +=
            u"VATSIM METARs: " % getVatsimMetarsUrl().toQString(i18n) % separator % u"VATSIM data file: " % getVatsimDataFileUrl().toQString(i18n) % separator % u"VATSIM server file: " % getVatsimServerFileUrl().toQString(i18n) % separator

            % u"Predefined servers: " % getPredefinedServers().toQString(i18n) % separator

            % u"AFV voice server URL: " % getAfvApiServerUrl().toQString(i18n) % separator

            % u"AFV map URL: " % getAfvMapUrl().toQString(i18n);

        return s;
    }

    QVariant CGlobalSetup::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return CVariant::fromValue(*this); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbRootDirectory: return QVariant::fromValue(m_dbRootDirectoryUrl);
        case IndexDbHttpsPort: return QVariant::fromValue(m_dbHttpsPort);
        case IndexDbLoginService: return QVariant::fromValue(this->getDbLoginServiceUrl());
        case IndexVatsimStatus: return QVariant::fromValue(m_vatsimStatusFileUrl);
        case IndexVatsimData: return QVariant::fromValue(m_vatsimDataFileUrl);
        case IndexVatsimServer: return QVariant::fromValue(m_vatsimServerFileUrl);
        case IndexVatsimHttpFsd: return QVariant::fromValue(m_vatsimFsdHttpUrl);
        case IndexVatsimMetars: return QVariant::fromValue(m_vatsimMetarsUrl);
        case IndexSharedUrl: return QVariant::fromValue(m_sharedUrl);
        case IndexOnlineHelpUrl: return QVariant::fromValue(m_onlineHelpUrl);
        case IndexMappingMinimumVersion: return QVariant::fromValue(m_mappingMinimumVersion);
        case IndexPredefinedServers: return QVariant::fromValue(m_predefinedServers);
        case IndexAfvApiServerUrl: return QVariant::fromValue(m_afvApiServerUrl);
        case IndexAfvMapUrl: return QVariant::fromValue(m_afvMapUrl);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CGlobalSetup::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CGlobalSetup>();
            return;
        }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbRootDirectory: m_dbRootDirectoryUrl.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexDbHttpsPort: m_dbHttpsPort = variant.toInt(); break;
        case IndexDbLoginService: break; // cannot be changed
        case IndexVatsimData: m_vatsimDataFileUrl = variant.value<CUrl>(); break;
        case IndexVatsimServer: m_vatsimServerFileUrl = variant.value<CUrl>(); break;
        case IndexVatsimHttpFsd: m_vatsimFsdHttpUrl = variant.value<CUrl>(); break;
        case IndexVatsimMetars: m_vatsimMetarsUrl = variant.value<CUrl>(); break;
        case IndexSharedUrl: m_sharedUrl = variant.value<CUrl>(); break;
        case IndexOnlineHelpUrl: m_onlineHelpUrl = variant.value<CUrl>(); break;
        case IndexMappingMinimumVersion: m_mappingMinimumVersion = variant.toString(); break;
        case IndexPredefinedServers: m_predefinedServers = variant.value<CServerList>(); break;
        case IndexAfvApiServerUrl: m_afvApiServerUrl = variant.value<CUrl>(); break;
        case IndexAfvMapUrl: m_afvMapUrl = variant.value<CUrl>(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    const QString &CGlobalSetup::schemaVersionString()
    {
        // This is not the current swift version, but the schema version
        static const QString v("0.7.0");
        return v;
    }
} // namespace swift::core::data
