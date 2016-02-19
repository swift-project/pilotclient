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
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"
#include <QStringList>

namespace BlackCore
{
    namespace Data
    {
        //! Global settings for readers, debug flags, etc.
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
                IndexVatsimBookings,
                IndexVatsimMetars,
                IndexVatsimData,
                IndexSwiftDbFiles,
                IndexBootstrap,
                IndexUpdateInfo,
                IndexWasLoaded,
                IndexShared
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
            int dbHttpPort() const { return m_dbHttpPort; }

            //! Https port
            int dbHttpsPort() const { return m_dbHttpsPort; }

            //! Debug flag
            bool dbDebugFlag() const;

            //! Set debug flag
            void setServerDebugFlag(bool debug);

            //! Same type?
            bool hasSameType(const CGlobalSetup &otherSetup) const;

            //! Home page url
            BlackMisc::Network::CUrl dbHomePageUrl() const;

            //! Login service
            BlackMisc::Network::CUrl dbLoginServiceUrl() const;

            //! Root directory of DB
            const BlackMisc::Network::CUrl &dbRootDirectoryUrl() const { return m_dbRootDirectoryUrl; }

            //! ICAO Reader location
            BlackMisc::Network::CUrl dbIcaoReaderUrl() const;

            //! Model Reader protocol
            BlackMisc::Network::CUrl dbModelReaderUrl() const;

            //! URL to read VATSIM bookings
            const BlackMisc::Network::CUrl &vatsimBookingsUrl() const { return m_vatsimBookingsUrl; }

            //! VATSIM METAR URL
            BlackMisc::Network::CUrl vatsimMetarsUrl() const;

            //! VATSIM data file URLs
            const BlackMisc::Network::CUrlList &vatsimDataFileUrls() const { return m_vatsimDataFileUrls; }

            //! Bootstrap URLs (where the data for the setup itself can be downloaded)
            BlackMisc::Network::CUrlList bootstrapFileUrls() const;

            //! Version and download locations
            BlackMisc::Network::CUrlList updateInfoFileUrls() const;

            //! Alternative locations of swift DB data files
            BlackMisc::Network::CUrlList swiftDbDataFileLocationUrls() const;

            //! Locations of swift DB news
            const BlackMisc::Network::CUrlList &swiftLatestNewsUrls() const { return m_newsUrls; }

            //! FSD test servers
            const BlackMisc::Network::CServerList &fsdTestServers() const { return m_fsdTestServers; }

            //! FSD test servers plus hardcoded
            BlackMisc::Network::CServerList fsdTestServersPlusHardcodedServers() const;

            //! Productive settings?
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
            void setPropertyByIndex(const BlackMisc::CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! Schema version
            static const QString &versionString();

            //! Build bootstrap file URL
            static QString buildBootstrapFileUrl(const QString &candidate);

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(BlackCore::Data::CGlobalSetup)

            bool                            m_wasLoaded = false;    //!< Loaded from web
            int                             m_dbHttpPort = 80;      //!< port
            int                             m_dbHttpsPort = 443;    //!< SSL port
            bool                            m_development = false;  //!< dev. version?
            BlackMisc::Network::CUrl        m_dbRootDirectoryUrl;   //!< Root directory of DB
            BlackMisc::Network::CUrl        m_vatsimBookingsUrl;    //!< ATC bookings
            BlackMisc::Network::CUrl        m_vatsimMetarsUrl;      //!< METAR data
            BlackMisc::Network::CUrlList    m_vatsimDataFileUrls;   //!< Overall VATSIM data file
            BlackMisc::Network::CUrlList    m_sharedUrls;           //!< where we can obtain shared info files such as bootstrap, ..
            BlackMisc::Network::CUrlList    m_newsUrls;             //!< where we can obtain latest news
            BlackMisc::Network::CServerList m_fsdTestServers;       //!< FSD test servers

            // transient members, to be switched on/off via GUI or set from reader
            bool m_dbDebugFlag = false; //!< can trigger DEBUG on the server, so you need to know what you are doing
        };

        //! Trait for global setup data
        struct GlobalSetup : public BlackMisc::CDataTrait<CGlobalSetup>
        {
            //! Key in data cache
            static const char *key() { return "bootstrap"; }

            //! Default value
            static const CGlobalSetup &defaultValue()
            {
                static const CGlobalSetup gs;
                return gs;
            }
        };

    } // ns
} // ns

Q_DECLARE_METATYPE(BlackCore::Data::CGlobalSetup)
BLACK_DECLARE_TUPLE_CONVERSION(BlackCore::Data::CGlobalSetup, (
                                   attr(o.m_wasLoaded),
                                   attr(o.m_timestampMSecsSinceEpoch),
                                   attr(o.m_dbRootDirectoryUrl),
                                   attr(o.m_dbHttpPort),
                                   attr(o.m_dbHttpsPort),
                                   attr(o.m_vatsimBookingsUrl),
                                   attr(o.m_vatsimMetarsUrl),
                                   attr(o.m_vatsimDataFileUrls),
                                   attr(o.m_sharedUrls),
                                   attr(o.m_newsUrls),
                                   attr(o.m_fsdTestServers),
                                   attr(o.m_development),
                                   attr(o.m_dbDebugFlag, flags < DisabledForJson > ())
                               ))
#endif // guard
