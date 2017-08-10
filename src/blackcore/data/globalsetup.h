/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DATA_GLOBALSETUP_H
#define BLACKCORE_DATA_GLOBALSETUP_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/datacache.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/url.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>

namespace BlackCore
{
    namespace Data
    {
        //! Global settings for readers, debug flags, etc.
        //! \note also called the bootstrap file as it is required once to get information where all the data are located
        class BLACKCORE_EXPORT CGlobalSetup :
            public BlackMisc::CValueObject<CGlobalSetup>,
            public BlackMisc::ITimestampBased
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexDbRootDirectory = BlackMisc::CPropertyIndex::GlobalIndexCGlobalSetup,
                IndexDbHttpPort,
                IndexDbHttpsPort,
                IndexDbLoginService,
                IndexVatsimStatus,
                IndexVatsimBookings,
                IndexVatsimMetars,
                IndexVatsimData,
                IndexSwiftDbFiles,
                IndexSwiftMapUrls,
                IndexBootstrapFileUrls,
                IndexNewsUrls,
                IndexOnlineHelpUrls,
                IndexCrashReportServerUrl,
                IndexUpdateInfo,
                IndexWasLoaded,
                IndexSharedUrls
            };

            //! Default constructor
            CGlobalSetup();

            //! Destructor.
            ~CGlobalSetup() {}

            //! Has data loaded from web
            bool wasLoaded() const { return m_wasLoaded; }

            //! Mark as loaded
            void markAsLoaded(bool loaded) { this->m_wasLoaded = loaded; }

            //! Http port
            int getDbHttpPort() const { return m_dbHttpPort; }

            //! Https port
            int getDbHttpsPort() const { return m_dbHttpsPort; }

            //! Debug flag
            bool dbDebugFlag() const;

            //! Set debug flag
            void setServerDebugFlag(bool debug);

            //! Same type?
            bool hasSameType(const CGlobalSetup &otherSetup) const;

            //! Crash report server url
            BlackMisc::Network::CUrl getCrashReportServerUrl() const { return m_crashReportServerUrl; }

            //! Home page url
            BlackMisc::Network::CUrl getDbHomePageUrl() const;

            //! Help page URL
            BlackMisc::Network::CUrl getHelpPageUrl() const;

            //! Legal directory URL
            BlackMisc::Network::CUrl getLegalDirectoryUrl() const;

            //! Login service
            BlackMisc::Network::CUrl getDbLoginServiceUrl() const;

            //! Root directory of DB
            const BlackMisc::Network::CUrl &getDbRootDirectoryUrl() const { return m_dbRootDirectoryUrl; }

            //! ICAO reader URL
            BlackMisc::Network::CUrl getDbIcaoReaderUrl() const;

            //! Model reader URL
            BlackMisc::Network::CUrl getDbModelReaderUrl() const;

            //! Airport reader URL
            BlackMisc::Network::CUrl getDbAirportReaderUrl() const;

            //! Info data reader URL
            BlackMisc::Network::CUrl getDbInfoReaderUrl() const;

            //! Shared URLs
            const BlackMisc::Network::CUrlList &getSwiftSharedUrls() const;

            //! URL to read VATSIM bookings
            const BlackMisc::Network::CUrl &getVatsimBookingsUrl() const { return m_vatsimBookingsUrl; }

            //! VATSIM METAR URL
            const BlackMisc::Network::CUrlList &getVatsimMetarsUrls() const { return this->m_vatsimMetarsUrls; }

            //! VATSIM status file URLs
            const BlackMisc::Network::CUrlList &getVatsimStatusFileUrls() const { return m_vatsimStatusFileUrls; }

            //! VATSIM data file URLs
            const BlackMisc::Network::CUrlList &getVatsimDataFileUrls() const { return m_vatsimDataFileUrls; }

            //! Bootstrap URLs (where the data for the setup itself can be downloaded)
            BlackMisc::Network::CUrlList getBootstrapFileUrls() const;

            //! Version and download locations
            BlackMisc::Network::CUrlList getDistributionUrls() const;

            //! Alternative locations of swift DB data files
            BlackMisc::Network::CUrlList getSwiftDbDataFileLocationUrls() const;

            //! Locations of swift DB news
            const BlackMisc::Network::CUrlList &getSwiftLatestNewsUrls() const;

            //! Online help URLs
            const BlackMisc::Network::CUrlList &getOnlineHelpUrls() const;

            //! swift map URLs
            const BlackMisc::Network::CUrlList &getSwiftMapUrls() const;

            //! FSD test servers
            const BlackMisc::Network::CServerList &getFsdTestServers() const { return m_fsdTestServers; }

            //! FSD test servers plus hardcoded
            BlackMisc::Network::CServerList getFsdTestServersPlusHardcodedServers() const;

            //! Is server a development server?
            bool isDevelopment() const { return m_development; }

            //! Productive settings?
            void setDevelopment(bool development) { m_development  = development; }

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! To string
            QString convertToQString(const QString &separator, bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const BlackMisc::CVariant &variant);

            //! Schema version
            static const QString &versionString();

            //! Build bootstrap file URL
            static QString buildBootstrapFileUrl(const QString &candidate);

            //! Build the full dbdata directory URL
            static BlackMisc::Network::CUrl buildDbDataDirectory(const BlackMisc::Network::CUrl &candidate);

            //! Object initialized by JSON file
            static CGlobalSetup fromJsonFile(const QString &fileNameAndPath);

        private:
            bool                            m_wasLoaded = false;    //!< Loaded from web
            int                             m_dbHttpPort = 80;      //!< port
            int                             m_dbHttpsPort = 443;    //!< SSL port
            bool                            m_development = false;  //!< dev. version?
            BlackMisc::Network::CUrl        m_crashReportServerUrl; //!< crash report server
            BlackMisc::Network::CUrl        m_dbRootDirectoryUrl;   //!< Root directory of DB
            BlackMisc::Network::CUrl        m_vatsimBookingsUrl;    //!< ATC bookings
            BlackMisc::Network::CUrlList    m_vatsimMetarsUrls;     //!< METAR data
            BlackMisc::Network::CUrlList    m_vatsimStatusFileUrls; //!< Status file, where to find the VATSIM files (METAR, data, ATIS, other status files)
            BlackMisc::Network::CUrlList    m_vatsimDataFileUrls;   //!< Overall VATSIM data file / merely for bootstrapping the first time
            BlackMisc::Network::CUrlList    m_sharedUrls;           //!< where we can obtain shared info files such as bootstrap, ..
            BlackMisc::Network::CUrlList    m_newsUrls;             //!< where we can obtain latest news
            BlackMisc::Network::CUrlList    m_onlineHelpUrls;       //!< online help URLs
            BlackMisc::Network::CUrlList    m_mapUrls;              //!< swift map URLs
            BlackMisc::Network::CServerList m_fsdTestServers;       //!< FSD test servers

            // transient members, to be switched on/off via GUI or set from reader
            bool m_dbDebugFlag = false; //!< can trigger DEBUG on the server, so you need to know what you are doing

            BLACK_METACLASS(
                CGlobalSetup,
                BLACK_METAMEMBER(wasLoaded),
                BLACK_METAMEMBER(timestampMSecsSinceEpoch),
                BLACK_METAMEMBER(crashReportServerUrl),
                BLACK_METAMEMBER(dbRootDirectoryUrl),
                BLACK_METAMEMBER(dbHttpPort),
                BLACK_METAMEMBER(dbHttpsPort),
                BLACK_METAMEMBER(vatsimStatusFileUrls),
                BLACK_METAMEMBER(vatsimDataFileUrls),
                BLACK_METAMEMBER(vatsimBookingsUrl),
                BLACK_METAMEMBER(vatsimMetarsUrls),
                BLACK_METAMEMBER(sharedUrls),
                BLACK_METAMEMBER(newsUrls),
                BLACK_METAMEMBER(onlineHelpUrls),
                BLACK_METAMEMBER(mapUrls),
                BLACK_METAMEMBER(fsdTestServers),
                BLACK_METAMEMBER(development),
                BLACK_METAMEMBER(dbDebugFlag, BlackMisc::DisabledForJson)
            );
        };

        //! Trait for global setup data
        struct TGlobalSetup : public BlackMisc::TDataTrait<CGlobalSetup>
        {
            //! Key in data cache
            static const char *key() { return "bootstrap"; }

            //! First load is synchronous
            static constexpr bool isPinned() { return true; }
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackCore::Data::CGlobalSetup)

#endif // guard
