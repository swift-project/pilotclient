/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "globalsetup.h"
#include "blackmisc/project.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QStringList>

using namespace BlackMisc;
using namespace BlackMisc::Math;
using namespace BlackMisc::Network;

namespace BlackCore
{
    namespace Data
    {
        CGlobalSetup::CGlobalSetup() :
            ITimestampBased(0),
            m_dbHttpPort(80),
            m_dbHttpsPort(443),
            m_dbRootDirectoryUrl("http://ubuntu12/swiftdatastore/public"),
            m_vatsimBookingsUrl("http://vatbook.euroutepro.com/xml2.php"),
            m_vatsimMetarsUrl("http://metar.vatsim.net/metar.php"),
            m_vatsimDataFileUrls(QStringList({ "http://info.vroute.net/vatsim-data.txt" })),
            m_sharedUrls(CProject::swiftTeamDefaultServers()),
            m_newsUrls(QStringList({ "http://swift-project.org/" })),
            m_fsdTestServers({ CServer("swift", "swift Testserver", "vatsim-germany.org", 6809, CUser("1234567", "swift Test User", "", "123456"), true) })
        { }

        CUrl CGlobalSetup::dbIcaoReaderUrl() const
        {
            return dbRootDirectoryUrl();
        }

        CUrl CGlobalSetup::dbModelReaderUrl() const
        {
            return dbRootDirectoryUrl();
        }

        CUrl CGlobalSetup::dbHomePageUrl() const
        {
            return dbRootDirectoryUrl().withAppendedPath("/page/index.php");
        }

        CUrl CGlobalSetup::dbLoginServiceUrl() const
        {
            return dbRootDirectoryUrl().
                   withAppendedPath("/service/jsonauthenticate.php").
                   withSwitchedScheme("https", m_dbHttpsPort);
        }

        bool CGlobalSetup::dbDebugFlag() const
        {
            if (!m_dbDebugFlag) { return false; }

            // further checks could go here
            return isDevelopment();
        }

        void CGlobalSetup::setServerDebugFlag(bool debug)
        {
            m_dbDebugFlag = debug;
        }

        bool CGlobalSetup::hasSameType(const CGlobalSetup &otherSetup) const
        {
            return this->isDevelopment() == otherSetup.isDevelopment();
        }

        CUrl CGlobalSetup::vatsimMetarsUrl() const
        {
            return this->m_vatsimMetarsUrl.withAppendedQuery("id=all");
        }

        CUrlList CGlobalSetup::bootstrapUrls() const
        {
            CUrlList urls(m_sharedUrls);
            return urls.appendPath(isDevelopment() ?
                                   CGlobalSetup::versionString() + "/development/bootstrap/bootstrap.json" :
                                   CGlobalSetup::versionString() + "/productive/bootstrap/bootstrap.json");
        }

        CUrlList CGlobalSetup::updateInfoUrls() const
        {
            CUrlList urls(m_sharedUrls);
            return urls.appendPath(isDevelopment() ?
                                   CGlobalSetup::versionString() + "/development/updateinfo/updateinfo.json" :
                                   CGlobalSetup::versionString() + "/productive/updateinfo/updateinfo.json");
        }

        CUrlList CGlobalSetup::swiftDbDataFileLocationUrls() const
        {
            CUrlList urls(m_sharedUrls);
            return urls.appendPath(isDevelopment() ?
                                   CGlobalSetup::versionString() + "/development/dbdata/" :
                                   CGlobalSetup::versionString() + "/productive/dbdata/");
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
            s.append("For development: ");
            s.append(boolToYesNo(isDevelopment()));
            s.append(separator);

            s.append("Update info URLs: ");
            s.append(updateInfoUrls().toQString(i18n));
            s.append(separator);
            s.append("Bootstrap URLs: ");
            s.append(bootstrapUrls().toQString(i18n));
            s.append(separator);
            s.append("News URLs: ");
            s.append(swiftLatestNewsUrls().toQString(i18n));
            s.append(separator);

            s.append("DB root directory: ");
            s.append(dbRootDirectoryUrl().toQString(i18n));
            s.append(separator);
            s.append("ICAO DB reader: ");
            s.append(dbIcaoReaderUrl().toQString(i18n));
            s.append(separator);
            s.append("Model DB reader: ");
            s.append(dbModelReaderUrl().toQString(i18n));
            s.append(separator);
            s.append("DB home page: ");
            s.append(dbHomePageUrl().toQString(i18n));
            s.append(separator);
            s.append("DB login service: ");
            s.append(dbLoginServiceUrl().toQString(i18n));
            s.append(separator);
            s.append("swift DB datafile locations: ");
            s.append(swiftDbDataFileLocationUrls().toQString(i18n));
            s.append(separator);

            s.append("VATSIM bookings: ");
            s.append(vatsimBookingsUrl().toQString(i18n));
            s.append(separator);
            s.append("VATSIM METARs: ");
            s.append(vatsimMetarsUrl().toQString(i18n));
            s.append(separator);
            s.append("VATSIM data file: ");
            s.append(vatsimDataFileUrls().toQString(i18n));
            s.append(separator);

            s.append("FSD test servers: ");
            s.append(fsdTestServers().toQString(i18n));
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
                return CVariant::fromValue(this->dbLoginServiceUrl());
            case IndexVatsimData:
                return CVariant::fromValue(this->m_vatsimDataFileUrls);
            case IndexVatsimBookings:
                return CVariant::fromValue(this->m_vatsimDataFileUrls);
            case IndexVatsimMetars:
                return CVariant::fromValue(this->m_vatsimMetarsUrl);
            case IndexUpdateInfo:
                return CVariant::fromValue(this->updateInfoUrls());
            case IndexBootstrap:
                return CVariant::fromValue(this->bootstrapUrls());
            case IndexSwiftDbFiles:
                return CVariant::fromValue(this->swiftDbDataFileLocationUrls());
            case IndexShared:
                return CVariant::fromValue(this->m_sharedUrls);
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
                this->m_vatsimMetarsUrl.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexShared:
                this->m_sharedUrls = variant.value<CUrlList>();
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

        const QString &CGlobalSetup::versionString()
        {
            static const QString v("0.6.1");
            return v;
        }
    } // ns
} // ns
