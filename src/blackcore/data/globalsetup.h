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
#include "blackcore/datacache.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"
#include <QStringList>

namespace BlackCore
{
    namespace Data
    {
        //! Settings for readers
        class BLACKCORE_EXPORT CGlobalSetup :
            public BlackMisc::CValueObject<CGlobalSetup>,
            public BlackMisc::ITimestampBased
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexDbIcaoReader = BlackMisc::CPropertyIndex::GlobalIndexCGlobalSetup,
                IndexDbModelReader,
                IndexDbHomePage,
                IndexDbLoginService,
                IndexVatsimBookings,
                IndexVatsimMetars,
                IndexVatsimData,
                IndexSwiftDbFiles,
                IndexBootstrap
            };

            //! Default constructor
            CGlobalSetup();

            //! Destructor.
            ~CGlobalSetup() {}

            //! ICAO Reader location
            const BlackMisc::Network::CUrl &dbIcaoReader() const { return m_dbIcaoReader; }

            //! Model Reader protocol
            const BlackMisc::Network::CUrl &dbModelReader() const { return m_dbModelReader; }

            //! Home page url
            BlackMisc::Network::CUrl dbHomePage() const;

            //! Login service
            BlackMisc::Network::CUrl dbLoginService() const;

            //! URL to read VATSIM bookings
            const BlackMisc::Network::CUrl &vatsimBookings() const { return m_vatsimBookings; }

            //! Type (development, productive)?
            const QString &getType() const { return m_type; }

            //! Set type
            void setType(const QString &type) { m_type = type.trimmed().toUpper(); }

            //! Same type?
            bool hasSameType(const QString &type) const;

            //! VATSIM METAR URL
            BlackMisc::Network::CUrl vatsimMetars() const;

            //! VATSIM data file URLs
            const BlackMisc::Network::CUrlList &vatsimDataFile() const { return m_vatsimDataFile; }

            //! Bootstrap URLs (where the data for the setup itself can be downloaded)
            const BlackMisc::Network::CUrlList &bootstrapUrls() const { return m_bootstrap; }

            //! Alternative locations of swift DB data files
            const BlackMisc::Network::CUrlList &swiftDbDataFileLocations() const { return m_swiftDbDataFiles; }

            //! FSD test servers
            const BlackMisc::Network::CServerList &fsdTestServers() const { return m_fsdTestServers; }

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

            //! To string
            QString convertToQString(const QString &separator, bool i18n = false) const;

            //! \copydoc CValueObject::propertyByIndex
            BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CVariant &variant, const BlackMisc::CPropertyIndex &index);

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(BlackCore::Data::CGlobalSetup)

            QString                         m_type;             //!< dev./productive?
            BlackMisc::Network::CUrl        m_dbIcaoReader;     //!< direct DB ICAO reader
            BlackMisc::Network::CUrl        m_dbModelReader;    //!< direct DB model reader
            BlackMisc::Network::CUrl        m_vatsimBookings;   //!< ATC bookings
            BlackMisc::Network::CUrl        m_vatsimMetars;     //!< METAR data
            BlackMisc::Network::CUrlList    m_vatsimDataFile;   //!< Overall VATSIM data file
            BlackMisc::Network::CUrlList    m_bootstrap;        //!< where we can obtain downloads of these data
            BlackMisc::Network::CUrlList    m_swiftDbDataFiles; //!< alternative locations of the DB files, if DB is not available
            BlackMisc::Network::CServerList m_fsdTestServers;   //!< FSD test servers
        };

        //! Trait for for global setup data
        struct GlobalSetup : public BlackCore::CDataTrait<CGlobalSetup>
        {
            //! Key in data cache
            static const char *key() { return "readers/global/bootstrap"; }

            //! Default value
            static const CGlobalSetup &defaultValue()
            {
                static const CGlobalSetup urls;
                return urls;
            }
        };

    } // ns
} // ns

Q_DECLARE_METATYPE(BlackCore::Data::CGlobalSetup)
BLACK_DECLARE_TUPLE_CONVERSION(BlackCore::Data::CGlobalSetup, (
                                   attr(o.m_timestampMSecsSinceEpoch),
                                   attr(o.m_type),
                                   attr(o.m_dbIcaoReader),
                                   attr(o.m_dbModelReader),
                                   attr(o.m_vatsimBookings),
                                   attr(o.m_vatsimMetars),
                                   attr(o.m_vatsimDataFile),
                                   attr(o.m_bootstrap),
                                   attr(o.m_swiftDbDataFiles),
                                   attr(o.m_fsdTestServers)
                               ))

#endif // guard
