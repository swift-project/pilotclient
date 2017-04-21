/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/application.h"
#include "blackmisc/json.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/user.h"
#include "blackmisc/stringutils.h"

#include <QJsonObject>
#include <QStringList>
#include <QStringBuilder>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Json;
using namespace BlackMisc::Network;

namespace BlackCore
{
    namespace Data
    {
        CGlobalSetup::CGlobalSetup() :
            ITimestampBased(0),
            m_dbRootDirectoryUrl("https://datastore.swift-project.org/"), m_vatsimBookingsUrl("http://vatbook.euroutepro.com/xml2.php"),
            m_vatsimMetarsUrls( {"http://metar.vatsim.net/metar.php"}), m_vatsimStatusFileUrls({ "https://status.vatsim.net" }),
            m_vatsimDataFileUrls({ "http://info.vroute.net/vatsim-data.txt" }), m_sharedUrls({"https://datastore.swift-project.org/shared", "https://vatsim-germany.org:50443/datastore/shared"}),
            m_newsUrls(QStringList({ "http://swift-project.org/" })), m_onlineHelpUrls(QStringList({ "help.swift-project.org/" })),
            m_mapUrls(QStringList({ "map.swift-project.org/" }))
        { }

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
            return this->m_sharedUrls;
        }

        CUrl CGlobalSetup::getDbHomePageUrl() const
        {
            return getDbRootDirectoryUrl().withAppendedPath("/page/index.php");
        }

        CUrl CGlobalSetup::getHelpPageUrl() const
        {
            const CUrlList urls(this->m_onlineHelpUrls);
            CUrl url = urls.getRandomWorkingUrl();
            if (sApp)
            {
                const QString a = sApp->getApplicationNameVersionBetaDev();
                url.appendQuery("swift", a);
            }
            return url;
        }

        CUrl CGlobalSetup::getLegalDirectoryUrl() const
        {
            return getDbRootDirectoryUrl().withAppendedPath("/legal/");
        }

        CUrl CGlobalSetup::getDbLoginServiceUrl() const
        {
            return getDbRootDirectoryUrl().
                   withAppendedPath("/service/jsonauthenticate.php").
                   withSwitchedScheme("https", m_dbHttpsPort);
        }

        bool CGlobalSetup::dbDebugFlag() const
        {
            if (!m_dbDebugFlag) { return false; }

            // further checks could go here
            const bool f = isDevelopment();
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

        CUrlList CGlobalSetup::getBootstrapFileUrls() const
        {
            CUrlList urls(m_sharedUrls);
            return urls.appendPath(CGlobalSetup::versionString() + "/bootstrap/bootstrap.json");
        }

        QString CGlobalSetup::buildBootstrapFileUrl(const QString &candidate)
        {
            static const QString version(QString(CGlobalSetup::versionString()).append("/"));
            if (candidate.endsWith("bootstrap.json")) { return candidate; }
            CUrl url(candidate);
            if (candidate.contains("/bootstrap"))
            {
                url.appendPath("bootstrap.json");
            }
            else if (candidate.endsWith(CGlobalSetup::versionString()) || candidate.endsWith(version))
            {
                url.appendPath("/bootstrap/bootstrap.json");
            }
            else if (candidate.endsWith("shared") || candidate.endsWith("shared/"))
            {
                url.appendPath(CGlobalSetup::versionString() + "/bootstrap/bootstrap.json");
            }
            else
            {
                url.appendPath("shared/" + CGlobalSetup::versionString() + "/bootstrap/bootstrap.json");
            }
            return url.getFullUrl();
        }

        CUrl CGlobalSetup::buildDbDataDirectory(const CUrl &candidate)
        {
            static const QString version(QString(versionString()).append("/"));
            if (candidate.pathEndsWith("dbdata") || candidate.pathEndsWith("dbdata/")) { return candidate; }
            CUrl url(candidate);
            if (candidate.pathEndsWith(versionString()) || candidate.pathEndsWith(version))
            {
                url.appendPath("/dbdata");
            }
            else if (candidate.pathEndsWith("shared") || candidate.pathEndsWith("shared/"))
            {
                url.appendPath(CGlobalSetup::versionString() + "/dbdata/");
            }
            else
            {
                url.appendPath("shared/" + CGlobalSetup::versionString() + "/dbdata/");
            }
            return url;
        }

        CGlobalSetup CGlobalSetup::fromJsonFile(const QString &fileNameAndPath)
        {
            CGlobalSetup setup;
            loadFromJsonFile(setup, fileNameAndPath);
            return setup;
        }

        CUrlList CGlobalSetup::getDistributionUrls() const
        {
            const CUrlList urls(m_sharedUrls);
            return urls.appendPath(CGlobalSetup::versionString() + "/updateinfo/distribution.json");
        }

        CUrlList CGlobalSetup::getSwiftDbDataFileLocationUrls() const
        {
            const CUrlList urls(m_sharedUrls);
            return urls.appendPath(CGlobalSetup::versionString() + "/dbdata/");
        }

        CServerList CGlobalSetup::getFsdTestServersPlusHardcodedServers() const
        {
            static const CServerList hardcoded({ CServer("swift", "swift Testserver", "vatsim-germany.org", 6809, CUser("1234567", "swift Test User", "", "123456"), true) });
            CServerList testServers(m_fsdTestServers);
            testServers.addIfAddressNotExists(hardcoded);
            return testServers;
        }

        QString CGlobalSetup::convertToQString(bool i18n) const
        {
            return convertToQString(", ", i18n);
        }

        QString CGlobalSetup::convertToQString(const QString &separator, bool i18n) const
        {
            QString s =
                "timestamp: "
                % this->getFormattedUtcTimestampYmdhms()
                % separator
                % "Global setup loaded: "
                % boolToYesNo(this->wasLoaded())
                % separator

                % "For development: "
                % boolToYesNo(isDevelopment())
                % separator

                % "Distribution URLs: "
                % getDistributionUrls().toQString(i18n)
                % separator
                % "Bootstrap URLs: "
                % getBootstrapFileUrls().toQString(i18n)
                % separator
                % "News URLs: "
                % getSwiftLatestNewsUrls().toQString(i18n)
                % separator
                % "Help URLs: "
                % getOnlineHelpUrls().toQString(i18n)
                % separator
                % "swift map URLs: "
                % getSwiftMapUrls().toQString(i18n)
                % separator;
            s +=
                "DB root directory: "
                % getDbRootDirectoryUrl().toQString(i18n)
                % separator
                % "ICAO DB reader: "
                % getDbIcaoReaderUrl().toQString(i18n)
                % separator
                % "Model DB reader: "
                % getDbModelReaderUrl().toQString(i18n)
                % separator
                % "Airport DB reader: "
                % getDbAirportReaderUrl().toQString(i18n)
                % separator
                % "DB home page: "
                % getDbHomePageUrl().toQString(i18n)
                % separator
                % "DB login service: "
                % getDbLoginServiceUrl().toQString(i18n)
                % separator
                % "swift DB datafile locations: "
                % getSwiftDbDataFileLocationUrls().toQString(i18n)
                % separator;

            s +=
                "VATSIM bookings: "
                % getVatsimBookingsUrl().toQString(i18n)
                % separator
                % "VATSIM METARs: "
                % getVatsimMetarsUrls().toQString(i18n)
                % separator
                % "VATSIM data file: "
                % getVatsimDataFileUrls().toQString(i18n)
                % separator

                % "FSD test servers: "
                % getFsdTestServers().toQString(i18n)
                % separator

                % "Crash report server: "
                % getCrashReportServerUrl().toQString(i18n);

            return s;
        }

        CVariant CGlobalSetup::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDbRootDirectory:
                return CVariant::fromValue(this->m_dbRootDirectoryUrl);
            case IndexDbHttpPort:
                return CVariant::fromValue(this->m_dbHttpPort);
            case IndexDbHttpsPort:
                return CVariant::fromValue(this->m_dbHttpsPort);
            case IndexDbLoginService:
                return CVariant::fromValue(this->getDbLoginServiceUrl());
            case IndexVatsimStatus:
                return CVariant::fromValue(this->m_vatsimStatusFileUrls);
            case IndexVatsimData:
                return CVariant::fromValue(this->m_vatsimDataFileUrls);
            case IndexVatsimBookings:
                return CVariant::fromValue(this->m_vatsimDataFileUrls);
            case IndexVatsimMetars:
                return CVariant::fromValue(this->m_vatsimMetarsUrls);
            case IndexUpdateInfo:
                return CVariant::fromValue(this->getDistributionUrls());
            case IndexBootstrapFileUrls:
                return CVariant::fromValue(this->getBootstrapFileUrls());
            case IndexSwiftDbFiles:
                return CVariant::fromValue(this->getSwiftDbDataFileLocationUrls());
            case IndexSharedUrls:
                return CVariant::fromValue(this->m_sharedUrls);
            case IndexNewsUrls:
                return CVariant::fromValue(this->m_newsUrls);
            case IndexSwiftMapUrls:
                return CVariant::fromValue(this->m_mapUrls);
            case IndexOnlineHelpUrls:
                return CVariant::fromValue(this->m_onlineHelpUrls);
            case IndexCrashReportServerUrl:
                return CVariant::fromValue(this->m_crashReportServerUrl);
            case IndexWasLoaded:
                return CVariant::fromValue(this->m_wasLoaded);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CGlobalSetup::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CGlobalSetup>(); return; }
            if (ITimestampBased::canHandleIndex(index))
            {
                ITimestampBased::setPropertyByIndex(index, variant);
                return;
            }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDbRootDirectory:
                this->m_dbRootDirectoryUrl.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexDbHttpPort:
                this->m_dbHttpPort = variant.toInt();
                break;
            case IndexDbHttpsPort:
                this->m_dbHttpsPort = variant.toInt();
                break;
            case IndexDbLoginService:
                break;
            case IndexVatsimData:
                this->m_vatsimDataFileUrls = variant.value<CUrlList>();
                break;
            case IndexVatsimBookings:
                this->m_vatsimBookingsUrl.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexVatsimMetars:
                this->m_vatsimMetarsUrls = variant.value<CUrlList>();
                break;
            case IndexSharedUrls:
                this->m_sharedUrls = variant.value<CUrlList>();
                break;
            case IndexNewsUrls:
                this->m_newsUrls = variant.value<CUrlList>();
                break;
            case IndexOnlineHelpUrls:
                this->m_onlineHelpUrls = variant.value<CUrlList>();
                break;
            case IndexSwiftMapUrls:
                this->m_mapUrls = variant.value<CUrlList>();
                break;
            case IndexCrashReportServerUrl:
                this->m_crashReportServerUrl = variant.value<CUrl>();
                break;
            case IndexWasLoaded:
                this->m_wasLoaded = variant.toBool();
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        const QString &CGlobalSetup::versionString()
        {
            // This is not the current swift version, but the schema version
            static const QString v("0.7.0");
            return v;
        }
    } // ns
} // ns
