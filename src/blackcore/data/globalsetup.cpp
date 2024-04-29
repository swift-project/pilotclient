// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackconfig/buildconfig.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/application.h"
#include "blackmisc/json.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/user.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/crashhandler.h"

#include <QVersionNumber>
#include <QJsonObject>
#include <QSysInfo>
#include <QStringList>
#include <QStringBuilder>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Json;
using namespace BlackMisc::Network;

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackCore::Data, CGlobalSetup)

namespace BlackCore::Data
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

    const CUrlList &CGlobalSetup::getSwiftSharedUrls() const
    {
        return m_sharedUrls;
    }

    CUrl CGlobalSetup::getDbHomePageUrl() const
    {
        return getDbRootDirectoryUrl().withAppendedPath("/page/index.php");
    }

    CUrl CGlobalSetup::getHelpPageUrl(const QString &context) const
    {
        const CUrlList urls(m_onlineHelpUrls);

        // we display in the standard browser,
        // so the user will realize if the URL does not work
        CUrl url = (urls.size() < 2) ? urls.frontOrDefault() : urls.getRandomUrl();
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

    CUrl CGlobalSetup::buildDbDataDirectoryUrl(const CUrl &candidate)
    {
        if (candidate.isEmpty()) return CUrl(); // not possible
        static const QString version(QString(schemaVersionString()).append("/"));
        if (candidate.pathEndsWith("dbdata") || candidate.pathEndsWith("dbdata/")) { return candidate; }
        CUrl url(candidate);
        if (candidate.pathEndsWith(schemaVersionString()) || candidate.pathEndsWith(version))
        {
            url.appendPath("/dbdata");
        }
        else if (candidate.pathEndsWith("shared") || candidate.pathEndsWith("shared/"))
        {
            url.appendPath(CGlobalSetup::schemaVersionString() + "/dbdata/");
        }
        else
        {
            url.appendPath("shared/" + CGlobalSetup::schemaVersionString() + "/dbdata/");
        }
        return url;
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

            % u"Help URLs: " % m_onlineHelpUrls.toQString(i18n) % separator;
        s +=
            u"DB root directory: " % getDbRootDirectoryUrl().toQString(i18n) % separator % u"ICAO DB reader: " % getDbIcaoReaderUrl().toQString(i18n) % separator % u"Model DB reader: " % getDbModelReaderUrl().toQString(i18n) % separator % u"Airport DB reader: " % getDbAirportReaderUrl().toQString(i18n) % separator % u"DB home page: " % getDbHomePageUrl().toQString(i18n) % separator % u"DB login service: " % getDbLoginServiceUrl().toQString(i18n) % separator;
        s +=
            u"VATSIM METARs: " % getVatsimMetarsUrls().toQString(i18n) % separator % u"VATSIM data file: " % getVatsimDataFileUrls().toQString(i18n) % separator % u"VATSIM server file: " % getVatsimServerFileUrl().toQString(i18n) % separator

            % u"Predefined servers: " % getPredefinedServers().toQString(i18n) % separator

            % u"AFV voice server URL: " % getAfvApiServerUrl().toQString(i18n) % separator

            % u"AFV map URL: " % getAfvMapUrl().toQString(i18n);

        return s;
    }

    QVariant CGlobalSetup::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return CVariant::fromValue(*this); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbRootDirectory: return QVariant::fromValue(m_dbRootDirectoryUrl);
        case IndexDbHttpPort: return QVariant::fromValue(m_dbHttpPort);
        case IndexDbHttpsPort: return QVariant::fromValue(m_dbHttpsPort);
        case IndexDbLoginService: return QVariant::fromValue(this->getDbLoginServiceUrl());
        case IndexVatsimStatus: return QVariant::fromValue(m_vatsimStatusFileUrls);
        case IndexVatsimData: return QVariant::fromValue(m_vatsimDataFileUrls);
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
        case IndexDbHttpPort: m_dbHttpPort = variant.toInt(); break;
        case IndexDbHttpsPort: m_dbHttpsPort = variant.toInt(); break;
        case IndexDbLoginService: break; // cannot be changed
        case IndexVatsimData: m_vatsimDataFileUrls = variant.value<CUrlList>(); break;
        case IndexVatsimServer: m_vatsimServerFileUrl = variant.value<CUrl>(); break;
        case IndexVatsimHttpFsd: m_vatsimFsdHttpUrl = variant.value<CUrl>(); break;
        case IndexVatsimMetars: m_vatsimMetarsUrls = variant.value<CUrlList>(); break;
        case IndexSharedUrls: m_sharedUrls = variant.value<CUrlList>(); break;
        case IndexOnlineHelpUrls: m_onlineHelpUrls = variant.value<CUrlList>(); break;
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
} // ns
