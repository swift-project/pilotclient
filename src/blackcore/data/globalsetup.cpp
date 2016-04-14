/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "globalsetup.h"
#include "blackmisc/buildconfig.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/stringutils.h"
#include <QStringList>

using namespace BlackMisc;
using namespace BlackMisc::Json;
using namespace BlackMisc::Math;
using namespace BlackMisc::Network;

namespace BlackCore
{
    namespace Data
    {
        CGlobalSetup::CGlobalSetup() :
            ITimestampBased(0),
            m_dbRootDirectoryUrl("http://ubuntu12/swiftdatastore/public"),
            m_vatsimBookingsUrl("http://vatbook.euroutepro.com/xml2.php"),
            m_vatsimMetarsUrls( {"http://metar.vatsim.net/metar.php"}),
            m_vatsimStatusFileUrls({ "https://status.vatsim.net" }),
            m_vatsimDataFileUrls({ "http://info.vroute.net/vatsim-data.txt" }),
            m_sharedUrls(CBuildConfig::swiftTeamDefaultServers()),
            m_newsUrls(QStringList({ "http://swift-project.org/" }))
        { }

        CUrl CGlobalSetup::getDbIcaoReaderUrl() const
        {
            return getDbRootDirectoryUrl();
        }

        CUrl CGlobalSetup::getDbModelReaderUrl() const
        {
            return getDbRootDirectoryUrl();
        }

        CUrl CGlobalSetup::getDbHomePageUrl() const
        {
            return getDbRootDirectoryUrl().withAppendedPath("/page/index.php");
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
            bool f = isDevelopment();
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
            static const QString version(QString(versionString()).append("/"));
            if (candidate.endsWith("bootstrap.json")) { return candidate; }
            CUrl url(candidate);
            if (candidate.contains("/bootstrap"))
            {
                url.appendPath("bootstrap.json");
            }
            else if (candidate.endsWith(versionString()) || candidate.endsWith(version))
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

        CGlobalSetup CGlobalSetup::fromJsonFile(const QString &fileNameAndPath)
        {
            CGlobalSetup setup;
            loadFromJsonFile(setup, fileNameAndPath);
            return setup;
        }

        CUrlList CGlobalSetup::getUpdateInfoFileUrls() const
        {
            const CUrlList urls(m_sharedUrls);
            return urls.appendPath(CGlobalSetup::versionString() + "/updateinfo/updateinfo.json");
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
            QString s("timestamp: ");
            s.append(this->getFormattedUtcTimestampYmdhms());
            s.append(separator);

            s.append("Loaded: ");
            s.append(boolToYesNo(this->wasLoaded()));
            s.append(separator);

            s.append("For development: ");
            s.append(boolToYesNo(isDevelopment()));
            s.append(separator);

            s.append("Update info URLs: ");
            s.append(getUpdateInfoFileUrls().toQString(i18n));
            s.append(separator);
            s.append("Bootstrap URLs: ");
            s.append(getBootstrapFileUrls().toQString(i18n));
            s.append(separator);
            s.append("News URLs: ");
            s.append(getSwiftLatestNewsUrls().toQString(i18n));
            s.append(separator);

            s.append("DB root directory: ");
            s.append(getDbRootDirectoryUrl().toQString(i18n));
            s.append(separator);
            s.append("ICAO DB reader: ");
            s.append(getDbIcaoReaderUrl().toQString(i18n));
            s.append(separator);
            s.append("Model DB reader: ");
            s.append(getDbModelReaderUrl().toQString(i18n));
            s.append(separator);
            s.append("DB home page: ");
            s.append(getDbHomePageUrl().toQString(i18n));
            s.append(separator);
            s.append("DB login service: ");
            s.append(getDbLoginServiceUrl().toQString(i18n));
            s.append(separator);
            s.append("swift DB datafile locations: ");
            s.append(getSwiftDbDataFileLocationUrls().toQString(i18n));
            s.append(separator);

            s.append("VATSIM bookings: ");
            s.append(getVatsimBookingsUrl().toQString(i18n));
            s.append(separator);
            s.append("VATSIM METARs: ");
            s.append(getVatsimMetarsUrls().toQString(i18n));
            s.append(separator);
            s.append("VATSIM data file: ");
            s.append(getVatsimDataFileUrls().toQString(i18n));
            s.append(separator);

            s.append("FSD test servers: ");
            s.append(getFsdTestServers().toQString(i18n));
            return s;
        }

        CVariant CGlobalSetup::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
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
                return CVariant::fromValue(this->getUpdateInfoFileUrls());
            case IndexBootstrap:
                return CVariant::fromValue(this->getBootstrapFileUrls());
            case IndexSwiftDbFiles:
                return CVariant::fromValue(this->getSwiftDbDataFileLocationUrls());
            case IndexShared:
                return CVariant::fromValue(this->m_sharedUrls);
            case IndexWasLoaded:
                return CVariant::fromValue(this->m_wasLoaded);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CGlobalSetup::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself()) { (*this) = variant.to<CGlobalSetup>(); return; }
            if (ITimestampBased::canHandleIndex(index))
            {
                ITimestampBased::setPropertyByIndex(variant, index);
                return;
            }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDbRootDirectory:
                this->m_dbRootDirectoryUrl.setPropertyByIndex(variant, index.copyFrontRemoved());
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
                this->m_vatsimBookingsUrl.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexVatsimMetars:
                this->m_vatsimMetarsUrls = variant.value<CUrlList>();
                break;
            case IndexShared:
                this->m_sharedUrls = variant.value<CUrlList>();
                break;
            case IndexWasLoaded:
                this->m_wasLoaded = variant.toBool();
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

        const QString &CGlobalSetup::versionString()
        {
            static const QString v("0.7.0");
            return v;
        }

    } // ns
} // ns
