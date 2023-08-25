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
    CGlobalSetup::CGlobalSetup() : CIdentifiable("CGlobalSetup"),
                                   ITimestampBased(0)
    {
        this->initDefaultValues();
    }

    bool CGlobalSetup::wasLoaded() const
    {
        return this->wasLoadedFromFile();
    }

    void CGlobalSetup::initDefaultValues()
    {
        m_mappingMinimumVersion = CBuildConfig::getVersionString();
        m_dbRootDirectoryUrl = CUrl("https://datastore.swift-project.org/");
        m_vatsimBookingsUrl = CUrl("http://vatbook.euroutepro.com/xml2.php");
        m_vatsimMetarsUrls = CUrlList { "http://metar.vatsim.net/metar.php" };
        m_vatsimStatusFileUrls = CUrlList { "https://status.vatsim.net" };
        m_vatsimDataFileUrls = CUrlList { "https://data.vatsim.net/v3/vatsim-data.json" };
        m_vatsimServerFileUrl = CUrl("https://data.vatsim.net/v3/vatsim-servers.json");
        m_vatsimFsdHttpUrl = CUrl("http://fsd-http.connect.vatsim.net");
        m_sharedUrls = CUrlList {
            "http://download.swift-project.org/shared/",
        };

        m_onlineHelpUrls = CUrlList {
            "https://datastore.swift-project.org/page/swifthelpdispatcher.html",
        };
        m_ncepGlobalForecastSystemUrl = CUrl("http://nomads.ncep.noaa.gov/cgi-bin/filter_gfs_0p50.pl");
        m_ncepGlobalForecastSystemUrl25 = CUrl("http://nomads.ncep.noaa.gov/cgi-bin/filter_gfs_0p25.pl");

        // hardcoded based on RR's information
        // https://discordapp.com/channels/539048679160676382/539925070550794240/586879411002015756
        m_crashReportServerUrl = CUrl();
    }

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

    CUrl CGlobalSetup::getCorrespondingSharedUrl(const CUrl &candidate) const
    {
        CUrlList sameHosts = this->getSwiftSharedUrls().findByHost(candidate.getHost());
        return sameHosts.frontOrDefault();
    }

    CUrlList CGlobalSetup::getSwiftBootstrapFileUrls() const
    {
        return getSwiftSharedUrls().withAppendedPath(CGlobalSetup::schemaVersionString() + "/bootstrap/" + CSwiftDirectories::bootstrapFileName());
    }

    CUrlList CGlobalSetup::getSwiftUpdateInfoFileUrls() const
    {
        return getSwiftSharedUrls().withAppendedPath(CGlobalSetup::schemaVersionString() + "/updateinfo/updateinfo.json");
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

    CUrl CGlobalSetup::getDbClientPingServiceUrl() const
    {
        return getDbRootDirectoryUrl().withAppendedPath("/service/clientping.php").withSwitchedScheme("https", m_dbHttpsPort);
    }

    CUrl CGlobalSetup::getDbClientPingServiceUrl(PingType type) const
    {
        CUrl pingUrl = this->getDbClientPingServiceUrl();
        if (pingUrl.isEmpty()) { CUrl(); }

        pingUrl.appendQuery("uuid", QSysInfo::machineUniqueId());
        if (type.testFlag(PingLogoff)) { pingUrl.appendQuery("logoff", "true"); }
        if (type.testFlag(PingShutdown)) { pingUrl.appendQuery("shutdown", "true"); }
        if (type.testFlag(PingStarted)) { pingUrl.appendQuery("started", "true"); }
        pingUrl.appendQuery("os", CBuildConfig::getPlatformString());
        if (CBuildConfig::isLocalDeveloperDebugBuild()) { pingUrl.appendQuery("dev", "true"); }
        if (sApp)
        {
            const CCrashInfo ci = CCrashHandler::instance()->getCrashInfo();
            pingUrl.appendQuery("application", sApp->getApplicationNameAndVersion());
            if (!ci.getSimulatorString().isEmpty()) { pingUrl.appendQuery("fs", ci.getSimulatorString()); }
            if (!ci.getFlightNetworkString().isEmpty()) { pingUrl.appendQuery("network", ci.getFlightNetworkString()); }
        }
        return pingUrl;
    }

    CUrl CGlobalSetup::getAlphaXSwiftBusFilesServiceUrl() const
    {
        return getDbRootDirectoryUrl().withAppendedPath("/service/jsonalphaxswiftbusfiles.php").withSwitchedScheme("https", m_dbHttpsPort);
    }

    bool CGlobalSetup::dbDebugFlag() const
    {
        if (!m_dbDebugFlag) { return false; }

        // further checks could go here
        const bool f = this->isDevelopment() || CBuildConfig::isLocalDeveloperDebugBuild();
        return f;
    }

    void CGlobalSetup::setServerDebugFlag(bool debug)
    {
        m_dbDebugFlag = debug;
    }

    bool CGlobalSetup::hasSameType(const CGlobalSetup &otherSetup) const
    {
        return this->isDevelopment() == otherSetup.isDevelopment();
    }

    QString CGlobalSetup::buildBootstrapFileUrl(const QString &candidate)
    {
        if (candidate.isEmpty()) return {}; // not possible
        static const QString version(QString(CGlobalSetup::schemaVersionString()).append("/"));
        if (candidate.endsWith(CSwiftDirectories::bootstrapFileName())) { return candidate; }
        CUrl url(candidate);
        if (candidate.contains("/bootstrap"))
        {
            url.appendPath(CSwiftDirectories::bootstrapFileName());
        }
        else if (candidate.endsWith(CGlobalSetup::schemaVersionString()) || candidate.endsWith(version))
        {
            url.appendPath("/bootstrap/" + CSwiftDirectories::bootstrapFileName());
        }
        else if (candidate.endsWith("shared") || candidate.endsWith("shared/"))
        {
            url.appendPath(CGlobalSetup::schemaVersionString() + "/bootstrap/" + CSwiftDirectories::bootstrapFileName());
        }
        else
        {
            url.appendPath("shared/" + CGlobalSetup::schemaVersionString() + "/bootstrap/" + CSwiftDirectories::bootstrapFileName());
        }
        return url.getFullUrl();
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

    CGlobalSetup CGlobalSetup::fromJsonFile(const QString &fileNameAndPath, bool acceptCacheFormat)
    {
        CGlobalSetup setup;
        loadFromJsonFile(setup, fileNameAndPath, acceptCacheFormat);
        return setup;
    }

    CServerList CGlobalSetup::getPredefinedServersPlusHardcodedServers() const
    {
        static const CServerList hardcoded(
            {
                CServer::swiftFsdTestServer(),
                CServer::fscFsdServer(),
                CServer::esTowerView()
                // CServer::afvFsdTestServer(),
            });
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
            u"timestamp: " % this->getFormattedUtcTimestampYmdhms() % separator % u"Global setup loaded: " % boolToYesNo(this->wasLoadedFromFile()) % separator

            % u"For development: " % boolToYesNo(isDevelopment()) % separator

            % u"Mapping min.version: " % this->getMappingMinimumVersionString() % separator

            % u"Distribution URLs: " % getSwiftUpdateInfoFileUrls().toQString(i18n) % separator % u"Bootstrap URLs: " % getSwiftBootstrapFileUrls().toQString(i18n) % separator % u"Help URLs: " % m_onlineHelpUrls.toQString(i18n) % separator;
        s +=
            u"DB root directory: " % getDbRootDirectoryUrl().toQString(i18n) % separator % u"ICAO DB reader: " % getDbIcaoReaderUrl().toQString(i18n) % separator % u"Model DB reader: " % getDbModelReaderUrl().toQString(i18n) % separator % u"Airport DB reader: " % getDbAirportReaderUrl().toQString(i18n) % separator % u"DB home page: " % getDbHomePageUrl().toQString(i18n) % separator % u"DB login service: " % getDbLoginServiceUrl().toQString(i18n) % separator % u"DB client ping service: " % getDbClientPingServiceUrl().toQString(i18n);
        s +=
            u"VATSIM bookings: " % getVatsimBookingsUrl().toQString(i18n) % separator % u"VATSIM METARs: " % getVatsimMetarsUrls().toQString(i18n) % separator % u"VATSIM data file: " % getVatsimDataFileUrls().toQString(i18n) % separator % u"VATSIM server file: " % getVatsimServerFileUrl().toQString(i18n) % separator

            % u"Predefined servers: " % getPredefinedServers().toQString(i18n) % separator

            % u"Crash report server: " % getCrashReportServerUrl().toQString(i18n);

        return s;
    }

    QVariant CGlobalSetup::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return CVariant::fromValue(*this); }
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbRootDirectory: return QVariant::fromValue(m_dbRootDirectoryUrl);
        case IndexDbHttpPort: return QVariant::fromValue(m_dbHttpPort);
        case IndexDbHttpsPort: return QVariant::fromValue(m_dbHttpsPort);
        case IndexDbLoginService: return QVariant::fromValue(this->getDbLoginServiceUrl());
        case IndexDbClientPingService: return QVariant::fromValue(this->getDbClientPingServiceUrl());
        case IndexVatsimStatus: return QVariant::fromValue(m_vatsimStatusFileUrls);
        case IndexVatsimData: return QVariant::fromValue(m_vatsimDataFileUrls);
        case IndexVatsimBookings: return QVariant::fromValue(m_vatsimBookingsUrl);
        case IndexVatsimServer: return QVariant::fromValue(m_vatsimServerFileUrl);
        case IndexVatsimHttpFsd: return QVariant::fromValue(m_vatsimFsdHttpUrl);
        case IndexVatsimMetars: return QVariant::fromValue(m_vatsimMetarsUrls);
        case IndexBootstrapFileUrls: return QVariant::fromValue(this->getSwiftBootstrapFileUrls());
        case IndexUpdateInfoFileUrls: return QVariant::fromValue(this->getSwiftUpdateInfoFileUrls());
        case IndexSharedUrls: return QVariant::fromValue(m_sharedUrls);
        case IndexOnlineHelpUrls: return QVariant::fromValue(m_onlineHelpUrls);
        case IndexCrashReportServerUrl: return QVariant::fromValue(m_crashReportServerUrl);
        case IndexWasLoadedFromFile: return QVariant::fromValue(m_wasLoadedFromFile);
        case IndexMappingMinimumVersion: return QVariant::fromValue(m_mappingMinimumVersion);
        case IndexPredefinedServers: return QVariant::fromValue(m_predefinedServers);
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
        if (ITimestampBased::canHandleIndex(index))
        {
            ITimestampBased::setPropertyByIndex(index, variant);
            return;
        }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDbRootDirectory: m_dbRootDirectoryUrl.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexDbHttpPort: m_dbHttpPort = variant.toInt(); break;
        case IndexDbHttpsPort: m_dbHttpsPort = variant.toInt(); break;
        case IndexDbLoginService: break; // cannot be changed
        case IndexDbClientPingService: break; // cannot be changed
        case IndexVatsimData: m_vatsimDataFileUrls = variant.value<CUrlList>(); break;
        case IndexVatsimBookings: m_vatsimBookingsUrl.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexVatsimServer: m_vatsimServerFileUrl = variant.value<CUrl>(); break;
        case IndexVatsimHttpFsd: m_vatsimFsdHttpUrl = variant.value<CUrl>(); break;
        case IndexVatsimMetars: m_vatsimMetarsUrls = variant.value<CUrlList>(); break;
        case IndexSharedUrls: m_sharedUrls = variant.value<CUrlList>(); break;
        case IndexOnlineHelpUrls: m_onlineHelpUrls = variant.value<CUrlList>(); break;
        case IndexCrashReportServerUrl: m_crashReportServerUrl = variant.value<CUrl>(); break;
        case IndexWasLoadedFromFile: m_wasLoadedFromFile = variant.toBool(); break;
        case IndexMappingMinimumVersion: m_mappingMinimumVersion = variant.toString(); break;
        case IndexPredefinedServers: m_predefinedServers = variant.value<CServerList>(); break;
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
