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
            IndexDbClientPingService,
            IndexVatsimStatus,
            IndexVatsimMetars,
            IndexVatsimData,
            IndexVatsimServer,
            IndexVatsimHttpFsd,
            IndexSwiftDbFiles,
            IndexUpdateInfoFileUrls,
            IndexOnlineHelpUrls,
            IndexCrashReportServerUrl,
            IndexSharedUrls,
            IndexMappingMinimumVersion,
            IndexPredefinedServers
        };

        //! Add info when pinging
        enum PingTypeFlag
        {
            PingUnspecific = 0,
            PingLogoff = 1 << 0,
            PingStarted = 1 << 1,
            PingShutdown = 1 << 2,
            PingCompleteShutdown = PingLogoff | PingShutdown
        };
        Q_DECLARE_FLAGS(PingType, PingTypeFlag)

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

        //! Crash report server url
        //! \deprecated NOT used anymore as by RR's info: https://discordapp.com/channels/539048679160676382/539925070550794240/586879411002015756
        BlackMisc::Network::CUrl getCrashReportServerUrl() const { return m_crashReportServerUrl; }

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

        //! DB ping service
        //! \remark based on getDbRootDirectoryUrl
        BlackMisc::Network::CUrl getDbClientPingServiceUrl() const;

        //! Ping the DB server, fire and forget (no feedback etc)
        BlackMisc::Network::CUrl getDbClientPingServiceUrl(PingType type) const;

        //! Seconds between pings
        qint64 getDbClientPingIntervalSecs() const { return m_pingIntervalSecs; }

        //! alpha XSwiftBus files available
        BlackMisc::Network::CUrl getAlphaXSwiftBusFilesServiceUrl() const;

        //! Shared URLs
        const BlackMisc::Network::CUrlList &getSwiftSharedUrls() const;

        //! Distribution URLs
        //! \remark based on getSwiftSharedUrls
        BlackMisc::Network::CUrlList getSwiftUpdateInfoFileUrls() const;

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

        //! NCEP GFS Forecasts (0.50 degree grid) data url
        BlackMisc::Network::CUrl getNcepGlobalForecastSystemUrl() const { return m_ncepGlobalForecastSystemUrl; }

        //! NCEP GFS Forecasts (0.25 degree grid) data url
        BlackMisc::Network::CUrl getNcepGlobalForecastSystemUrl25() const { return m_ncepGlobalForecastSystemUrl25; }

        //! COM/NAV equipment code help URL
        BlackMisc::Network::CUrl getComNavEquipmentHelpUrl() const { return m_comNavEquipmentHelpUrl; }

        //! SSR equipment code help URL
        BlackMisc::Network::CUrl getSsrEquipmentHelpUrl() const { return m_ssrEquipmentHelpUrl; }

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
        qint64 m_pingIntervalSecs = 180; //!< seconds between datastore pings
        QString m_mappingMinimumVersion; //!< minimum version
        BlackMisc::Network::CUrl m_crashReportServerUrl; //!< crash report server
        BlackMisc::Network::CUrl m_dbRootDirectoryUrl; //!< Root directory of DB
        BlackMisc::Network::CUrlList m_vatsimMetarsUrls; //!< METAR data
        BlackMisc::Network::CUrlList m_vatsimStatusFileUrls; //!< Status file, where to find the VATSIM files (METAR, data, ATIS, other status files)
        BlackMisc::Network::CUrlList m_vatsimDataFileUrls; //!< Overall VATSIM data file / merely for bootstrapping the first time
        BlackMisc::Network::CUrl m_vatsimServerFileUrl; //!< URL to list of VATSIM servers
        BlackMisc::Network::CUrl m_vatsimFsdHttpUrl; //!< URL to HTTP FSD server (for load-balancing and automatic server selection)
        BlackMisc::Network::CUrlList m_sharedUrls; //!< where we can obtain shared info files such as bootstrap, ..
        BlackMisc::Network::CUrlList m_onlineHelpUrls; //!< online help URLs
        BlackMisc::Network::CServerList m_predefinedServers; //!< Predefined servers loaded from setup file
        BlackMisc::Network::CUrl m_ncepGlobalForecastSystemUrl; //!< NCEP GFS url 0.5 degree resolution
        BlackMisc::Network::CUrl m_ncepGlobalForecastSystemUrl25; //!< NCEP GFS url 0.25 degree resolution
        BlackMisc::Network::CUrl m_comNavEquipmentHelpUrl; //!< Help URL for COM/NAV equipment codes
        BlackMisc::Network::CUrl m_ssrEquipmentHelpUrl; //!< Help URL for SSR equipment codes
        bool m_dbDebugFlag = false; //!< can trigger DEBUG on the server, so you need to know what you are doing. Only works with CBuildConfig::isLocalDeveloperDebugBuild

        BLACK_METACLASS(
            CGlobalSetup,
            BLACK_METAMEMBER(crashReportServerUrl),
            BLACK_METAMEMBER(dbRootDirectoryUrl),
            BLACK_METAMEMBER(dbHttpPort),
            BLACK_METAMEMBER(dbHttpsPort),
            BLACK_METAMEMBER(pingIntervalSecs),
            BLACK_METAMEMBER(vatsimStatusFileUrls),
            BLACK_METAMEMBER(vatsimDataFileUrls),
            BLACK_METAMEMBER(vatsimServerFileUrl),
            BLACK_METAMEMBER(vatsimFsdHttpUrl),
            BLACK_METAMEMBER(vatsimMetarsUrls),
            BLACK_METAMEMBER(sharedUrls),
            BLACK_METAMEMBER(onlineHelpUrls),
            BLACK_METAMEMBER(predefinedServers),
            BLACK_METAMEMBER(mappingMinimumVersion),
            BLACK_METAMEMBER(ncepGlobalForecastSystemUrl),
            BLACK_METAMEMBER(ncepGlobalForecastSystemUrl25),
            BLACK_METAMEMBER(comNavEquipmentHelpUrl),
            BLACK_METAMEMBER(ssrEquipmentHelpUrl),
            BLACK_METAMEMBER(dbDebugFlag)
        );
    };
} // ns

Q_DECLARE_METATYPE(BlackCore::Data::CGlobalSetup)
Q_DECLARE_METATYPE(BlackCore::Data::CGlobalSetup::PingTypeFlag)
Q_DECLARE_METATYPE(BlackCore::Data::CGlobalSetup::PingType)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackCore::Data::CGlobalSetup::PingType)

#endif // guard
