// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_DATA_GLOBALSETUP_H
#define BLACKCORE_DATA_GLOBALSETUP_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/url.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/datacache.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackCore::Data, CGlobalSetup)

namespace BlackCore::Data
{
    //! Global settings for readers, debug flags, etc.
    //! \note also called the bootstrap file as it is required once to get information where all the data are located
    class BLACKCORE_EXPORT CGlobalSetup :
        public BlackMisc::CValueObject<CGlobalSetup>,
        public BlackMisc::CIdentifiable
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexDbRootDirectory = BlackMisc::CPropertyIndexRef::GlobalIndexCGlobalSetup,
            IndexDbHttpPort,
            IndexDbHttpsPort,
            IndexDbLoginService,
            IndexVatsimStatus,
            IndexVatsimMetars,
            IndexVatsimData,
            IndexVatsimServer,
            IndexVatsimHttpFsd,
            IndexOnlineHelpUrls,
            IndexSharedUrls,
            IndexMappingMinimumVersion,
            IndexPredefinedServers,
            IndexAfvApiServerUrl,
            IndexAfvMapUrl
        };

        //! Default constructor
        CGlobalSetup();

        //! Http port
        int getDbHttpPort() const { return m_dbHttpPort; }

        //! Https port
        int getDbHttpsPort() const { return m_dbHttpsPort; }

        //! Debug flag
        bool dbDebugFlag() const;

        //! Set debug flag
        void setServerDebugFlag(bool debug);

        //! Root directory of DB
        const BlackMisc::Network::CUrl &getDbRootDirectoryUrl() const { return m_dbRootDirectoryUrl; }

        //! ICAO reader URL
        //! \remark based on getDbRootDirectoryUrl
        BlackMisc::Network::CUrl getDbIcaoReaderUrl() const;

        //! Model reader URL
        //! \remark based on getDbRootDirectoryUrl
        BlackMisc::Network::CUrl getDbModelReaderUrl() const;

        //! Airport reader URL
        //! \remark based on getDbRootDirectoryUrl
        BlackMisc::Network::CUrl getDbAirportReaderUrl() const;

        //! Info data reader URL
        //! \remark based on getDbRootDirectoryUrl
        BlackMisc::Network::CUrl getDbInfoReaderUrl() const;

        //! Home page url
        //! \remark based on getDbRootDirectoryUrl
        BlackMisc::Network::CUrl getDbHomePageUrl() const;

        //! Legal directory URL
        //! \remark based on getDbRootDirectoryUrl
        BlackMisc::Network::CUrl getLegalDirectoryUrl() const;

        //! Login service
        //! \remark based on getDbRootDirectoryUrl
        BlackMisc::Network::CUrl getDbLoginServiceUrl() const;

        //! Shared URLs
        const BlackMisc::Network::CUrlList &getSwiftSharedUrls() const;

        //! VATSIM METAR URL
        const BlackMisc::Network::CUrlList &getVatsimMetarsUrls() const { return m_vatsimMetarsUrls; }

        //! VATSIM status file URLs
        const BlackMisc::Network::CUrlList &getVatsimStatusFileUrls() const { return m_vatsimStatusFileUrls; }

        //! VATSIM data file URLs
        const BlackMisc::Network::CUrlList &getVatsimDataFileUrls() const { return m_vatsimDataFileUrls; }

        //! VATSIM server file URL
        BlackMisc::Network::CUrl getVatsimServerFileUrl() const { return m_vatsimServerFileUrl; }

        //! VATSIM server file URL
        BlackMisc::Network::CUrl getVatsimFsdHttpUrl() const { return m_vatsimFsdHttpUrl; }

        //! Help page URL
        //! \remark working URL evaluated at runtime, based on getOnlineHelpUrls
        BlackMisc::Network::CUrl getHelpPageUrl(const QString &context = {}) const;

        //! Predefined servers
        const BlackMisc::Network::CServerList &getPredefinedServers() const { return m_predefinedServers; }

        //! Predefined plus hardcoded
        BlackMisc::Network::CServerList getPredefinedServersPlusHardcodedServers() const;

        //! Creating mappings requires at least this version or higher
        //! \remark only valid if wasLoaded() is \c true
        const QString &getMappingMinimumVersionString() const { return m_mappingMinimumVersion; }

        //! Meets the minimum mapping version
        //! \remark only valid if wasLoaded() is \c true
        bool isSwiftVersionMinimumMappingVersion() const;

        //! NCEP GFS Forecasts (0.25 degree grid) data url
        BlackMisc::Network::CUrl getNcepGlobalForecastSystemUrl25() const { return m_ncepGlobalForecastSystemUrl25; }

        //! COM/NAV equipment code help URL
        BlackMisc::Network::CUrl getComNavEquipmentHelpUrl() const { return m_comNavEquipmentHelpUrl; }

        //! SSR equipment code help URL
        BlackMisc::Network::CUrl getSsrEquipmentHelpUrl() const { return m_ssrEquipmentHelpUrl; }

        //! AFV voice server URL
        BlackMisc::Network::CUrl getAfvApiServerUrl() const { return m_afvApiServerUrl; }

        //! AFV map URL
        BlackMisc::Network::CUrl getAfvMapUrl() const { return m_afvMapUrl; }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! To string with separator
        QString convertToQString(const QString &separator, bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! Schema version (shared files, bootstrap file)
        static const QString &schemaVersionString();

        //! Build the full dbdata directory URL from shared URL
        static BlackMisc::Network::CUrl buildDbDataDirectoryUrl(const BlackMisc::Network::CUrl &candidate);

    private:
        int m_dbHttpPort = 80; //!< port
        int m_dbHttpsPort = 443; //!< SSL port
        QString m_mappingMinimumVersion; //!< minimum version
        BlackMisc::Network::CUrl m_dbRootDirectoryUrl; //!< Root directory of DB
        BlackMisc::Network::CUrlList m_vatsimMetarsUrls; //!< METAR data
        BlackMisc::Network::CUrlList m_vatsimStatusFileUrls; //!< Status file, where to find the VATSIM files (METAR, data, ATIS, other status files)
        BlackMisc::Network::CUrlList m_vatsimDataFileUrls; //!< Overall VATSIM data file / merely for bootstrapping the first time
        BlackMisc::Network::CUrl m_vatsimServerFileUrl; //!< URL to list of VATSIM servers
        BlackMisc::Network::CUrl m_vatsimFsdHttpUrl; //!< URL to HTTP FSD server (for load-balancing and automatic server selection)
        BlackMisc::Network::CUrlList m_sharedUrls; //!< where we can obtain shared info files such as bootstrap, ..
        BlackMisc::Network::CUrlList m_onlineHelpUrls; //!< online help URLs
        BlackMisc::Network::CServerList m_predefinedServers; //!< Predefined servers loaded from setup file
        BlackMisc::Network::CUrl m_ncepGlobalForecastSystemUrl25; //!< NCEP GFS url 0.25 degree resolution
        BlackMisc::Network::CUrl m_comNavEquipmentHelpUrl; //!< Help URL for COM/NAV equipment codes
        BlackMisc::Network::CUrl m_ssrEquipmentHelpUrl; //!< Help URL for SSR equipment codes
        BlackMisc::Network::CUrl m_afvApiServerUrl; //!< AFV API server URL
        BlackMisc::Network::CUrl m_afvMapUrl; //!< AFV map URL
        bool m_dbDebugFlag = false; //!< can trigger DEBUG on the server, so you need to know what you are doing. Only works with CBuildConfig::isLocalDeveloperDebugBuild

        BLACK_METACLASS(
            CGlobalSetup,
            BLACK_METAMEMBER(dbRootDirectoryUrl, 0, RequiredForJson),
            BLACK_METAMEMBER(dbHttpPort, 0, RequiredForJson),
            BLACK_METAMEMBER(dbHttpsPort, 0, RequiredForJson),
            BLACK_METAMEMBER(vatsimStatusFileUrls, 0, RequiredForJson),
            BLACK_METAMEMBER(vatsimDataFileUrls, 0, RequiredForJson),
            BLACK_METAMEMBER(vatsimServerFileUrl, 0, RequiredForJson),
            BLACK_METAMEMBER(vatsimFsdHttpUrl, 0, RequiredForJson),
            BLACK_METAMEMBER(vatsimMetarsUrls, 0, RequiredForJson),
            BLACK_METAMEMBER(sharedUrls, 0, RequiredForJson),
            BLACK_METAMEMBER(onlineHelpUrls, 0, RequiredForJson),
            BLACK_METAMEMBER(predefinedServers, 0, RequiredForJson),
            BLACK_METAMEMBER(mappingMinimumVersion, 0, RequiredForJson),
            BLACK_METAMEMBER(ncepGlobalForecastSystemUrl25, 0, RequiredForJson),
            BLACK_METAMEMBER(comNavEquipmentHelpUrl, 0, RequiredForJson),
            BLACK_METAMEMBER(ssrEquipmentHelpUrl, 0, RequiredForJson),
            BLACK_METAMEMBER(dbDebugFlag, 0, RequiredForJson),
            BLACK_METAMEMBER(afvApiServerUrl, 0, RequiredForJson),
            BLACK_METAMEMBER(afvMapUrl, 0, RequiredForJson)
        );
    };
} // ns

Q_DECLARE_METATYPE(BlackCore::Data::CGlobalSetup)

#endif // guard
