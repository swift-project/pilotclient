// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_DATA_GLOBALSETUP_H
#define SWIFT_CORE_DATA_GLOBALSETUP_H

#include "core/swiftcoreexport.h"
#include "misc/network/serverlist.h"
#include "misc/network/url.h"
#include "misc/identifiable.h"
#include "misc/datacache.h"
#include "misc/metaclass.h"
#include "misc/propertyindex.h"
#include "misc/valueobject.h"
#include "misc/variant.h"

#include <QMetaType>
#include <QString>

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::core::data, CGlobalSetup)

namespace swift::core::data
{
    //! Global settings for readers, debug flags, etc.
    //! \note also called the bootstrap file as it is required once to get information where all the data are located
    class SWIFT_CORE_EXPORT CGlobalSetup :
        public swift::misc::CValueObject<CGlobalSetup>,
        public swift::misc::CIdentifiable
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexDbRootDirectory = swift::misc::CPropertyIndexRef::GlobalIndexCGlobalSetup,
            IndexDbHttpsPort,
            IndexDbLoginService,
            IndexVatsimStatus,
            IndexVatsimMetars,
            IndexVatsimData,
            IndexVatsimServer,
            IndexVatsimHttpFsd,
            IndexOnlineHelpUrl,
            IndexSharedUrl,
            IndexMappingMinimumVersion,
            IndexPredefinedServers,
            IndexAfvApiServerUrl,
            IndexAfvMapUrl
        };

        //! Default constructor
        CGlobalSetup();

        //! Https port
        int getDbHttpsPort() const { return m_dbHttpsPort; }

        //! Debug flag
        bool dbDebugFlag() const;

        //! Set debug flag
        void setServerDebugFlag(bool debug);

        //! Root directory of DB
        const swift::misc::network::CUrl &getDbRootDirectoryUrl() const { return m_dbRootDirectoryUrl; }

        //! ICAO reader URL
        //! \remark based on getDbRootDirectoryUrl
        swift::misc::network::CUrl getDbIcaoReaderUrl() const;

        //! Model reader URL
        //! \remark based on getDbRootDirectoryUrl
        swift::misc::network::CUrl getDbModelReaderUrl() const;

        //! Airport reader URL
        //! \remark based on getDbRootDirectoryUrl
        swift::misc::network::CUrl getDbAirportReaderUrl() const;

        //! Info data reader URL
        //! \remark based on getDbRootDirectoryUrl
        swift::misc::network::CUrl getDbInfoReaderUrl() const;

        //! Home page url
        //! \remark based on getDbRootDirectoryUrl
        swift::misc::network::CUrl getDbHomePageUrl() const;

        //! Legal directory URL
        //! \remark based on getDbRootDirectoryUrl
        swift::misc::network::CUrl getLegalDirectoryUrl() const;

        //! Login service
        //! \remark based on getDbRootDirectoryUrl
        swift::misc::network::CUrl getDbLoginServiceUrl() const;

        //! Shared URL
        const swift::misc::network::CUrl &getSwiftSharedUrl() const;

        //! VATSIM METAR URL
        const swift::misc::network::CUrl &getVatsimMetarsUrl() const { return m_vatsimMetarsUrl; }

        //! VATSIM status file URL
        const swift::misc::network::CUrl &getVatsimStatusFileUrl() const { return m_vatsimStatusFileUrl; }

        //! VATSIM data file URL
        const swift::misc::network::CUrl &getVatsimDataFileUrl() const { return m_vatsimDataFileUrl; }

        //! VATSIM server file URL
        swift::misc::network::CUrl getVatsimServerFileUrl() const { return m_vatsimServerFileUrl; }

        //! VATSIM server file URL
        swift::misc::network::CUrl getVatsimFsdHttpUrl() const { return m_vatsimFsdHttpUrl; }

        //! Help page URL
        swift::misc::network::CUrl getHelpPageUrl(const QString &context = {}) const;

        //! Predefined servers
        const swift::misc::network::CServerList &getPredefinedServers() const { return m_predefinedServers; }

        //! Predefined plus hardcoded
        swift::misc::network::CServerList getPredefinedServersPlusHardcodedServers() const;

        //! Creating mappings requires at least this version or higher
        //! \remark only valid if wasLoaded() is \c true
        const QString &getMappingMinimumVersionString() const { return m_mappingMinimumVersion; }

        //! Meets the minimum mapping version
        //! \remark only valid if wasLoaded() is \c true
        bool isSwiftVersionMinimumMappingVersion() const;

        //! COM/NAV equipment code help URL
        swift::misc::network::CUrl getComNavEquipmentHelpUrl() const { return m_comNavEquipmentHelpUrl; }

        //! SSR equipment code help URL
        swift::misc::network::CUrl getSsrEquipmentHelpUrl() const { return m_ssrEquipmentHelpUrl; }

        //! AFV voice server URL
        swift::misc::network::CUrl getAfvApiServerUrl() const { return m_afvApiServerUrl; }

        //! AFV map URL
        swift::misc::network::CUrl getAfvMapUrl() const { return m_afvMapUrl; }

        //! VATSIM auth URL
        swift::misc::network::CUrl getVatsimAuthUrl() const { return m_vatsimAuthUrl; }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! To string with separator
        QString convertToQString(const QString &separator, bool i18n = false) const;

        //! Get shared DB data directory URL
        swift::misc::network::CUrl getSharedDbDataDirectoryUrl();

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! Schema version (shared files, bootstrap file)
        static const QString &schemaVersionString();

    private:
        int m_dbHttpsPort = 443; //!< SSL port
        QString m_mappingMinimumVersion; //!< minimum version
        swift::misc::network::CUrl m_dbRootDirectoryUrl; //!< Root directory of DB
        swift::misc::network::CUrl m_vatsimMetarsUrl; //!< METAR data
        swift::misc::network::CUrl m_vatsimStatusFileUrl; //!< Status file, where to find the VATSIM files (METAR, data, ATIS, other status files)
        swift::misc::network::CUrl m_vatsimDataFileUrl; //!< Overall VATSIM data file / merely for bootstrapping the first time
        swift::misc::network::CUrl m_vatsimServerFileUrl; //!< URL to list of VATSIM servers
        swift::misc::network::CUrl m_vatsimFsdHttpUrl; //!< URL to HTTP FSD server (for load-balancing and automatic server selection)
        swift::misc::network::CUrl m_sharedUrl; //!< where we can obtain shared info files such as bootstrap, ..
        swift::misc::network::CUrl m_onlineHelpUrl; //!< online help URL
        swift::misc::network::CServerList m_predefinedServers; //!< Predefined servers loaded from setup file
        swift::misc::network::CUrl m_comNavEquipmentHelpUrl; //!< Help URL for COM/NAV equipment codes
        swift::misc::network::CUrl m_ssrEquipmentHelpUrl; //!< Help URL for SSR equipment codes
        swift::misc::network::CUrl m_afvApiServerUrl; //!< AFV API server URL
        swift::misc::network::CUrl m_afvMapUrl; //!< AFV map URL
        swift::misc::network::CUrl m_vatsimAuthUrl; //!< VATSIM Auth URL
        bool m_dbDebugFlag = false; //!< can trigger DEBUG on the server, so you need to know what you are doing. Only works with CBuildConfig::isLocalDeveloperDebugBuild

        SWIFT_METACLASS(
            CGlobalSetup,
            SWIFT_METAMEMBER(dbRootDirectoryUrl, 0, RequiredForJson),
            SWIFT_METAMEMBER(dbHttpsPort, 0, RequiredForJson),
            SWIFT_METAMEMBER(vatsimStatusFileUrl, 0, RequiredForJson),
            SWIFT_METAMEMBER(vatsimDataFileUrl, 0, RequiredForJson),
            SWIFT_METAMEMBER(vatsimServerFileUrl, 0, RequiredForJson),
            SWIFT_METAMEMBER(vatsimFsdHttpUrl, 0, RequiredForJson),
            SWIFT_METAMEMBER(vatsimMetarsUrl, 0, RequiredForJson),
            SWIFT_METAMEMBER(sharedUrl, 0, RequiredForJson),
            SWIFT_METAMEMBER(onlineHelpUrl, 0, RequiredForJson),
            SWIFT_METAMEMBER(predefinedServers, 0, RequiredForJson),
            SWIFT_METAMEMBER(mappingMinimumVersion, 0, RequiredForJson),
            SWIFT_METAMEMBER(comNavEquipmentHelpUrl, 0, RequiredForJson),
            SWIFT_METAMEMBER(ssrEquipmentHelpUrl, 0, RequiredForJson),
            SWIFT_METAMEMBER(dbDebugFlag, 0, RequiredForJson),
            SWIFT_METAMEMBER(afvApiServerUrl, 0, RequiredForJson),
            SWIFT_METAMEMBER(afvMapUrl, 0, RequiredForJson),
            SWIFT_METAMEMBER(vatsimAuthUrl, 0, RequiredForJson));
    };
} // ns

Q_DECLARE_METATYPE(swift::core::data::CGlobalSetup)

#endif // guard
