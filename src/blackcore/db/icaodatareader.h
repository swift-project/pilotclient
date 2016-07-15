/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_ICAODATAREADER_H
#define BLACKCORE_ICAODATAREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/db/databasereader.h"
#include "blackcore/data/dbcaches.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/url.h"
#include "blackmisc/country.h"
#include "blackmisc/countrylist.h"
#include "blackmisc/datacache.h"

#include <QObject>
#include <QReadWriteLock>
#include <QString>

class QDateTime;
class QNetworkReply;

namespace BlackCore
{
    namespace Db
    {
        //! Read ICAO data from Database
        class BLACKCORE_EXPORT CIcaoDataReader : public CDatabaseReader
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CIcaoDataReader(QObject *owner, const CDatabaseReaderConfigList &confg);

            //! Get aircraft ICAO information
            //! \threadsafe
            BlackMisc::Aviation::CAircraftIcaoCodeList getAircraftIcaoCodes() const;

            //! Get aircraft ICAO information for designator
            //! \threadsafe
            BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDesignator(const QString &designator) const;

            //! Get aircraft ICAO information for key
            //! \threadsafe
            BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDbKey(int key) const;

            //! Get airline ICAO information
            //! \threadsafe
            BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodes() const;

            //! Get best match for incomplete aircraft ICAO code
            //! \threadsafe
            BlackMisc::Aviation::CAircraftIcaoCode smartAircraftIcaoSelector(const BlackMisc::Aviation::CAircraftIcaoCode &icaoPattern) const;

            //! Get countries
            //! \threadsafe
            BlackMisc::CCountryList getCountries() const;

            //! Get countries count
            //! \threadsafe
            int getCountriesCount() const;

            //! Get country for ISO code
            //! \threadsafe
            BlackMisc::CCountry getCountryForIsoCode(const QString &isoCode) const;

            //! Get country for ISO name
            //! \threadsafe
            BlackMisc::CCountry getCountryForName(const QString &name) const;

            //! Get airline ICAO information for designator
            //! \threadsafe
            BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodesForDesignator(const QString &designator) const;

            //! Get airline ICAO information for key
            //! \threadsafe
            BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForDbKey(int key) const;

            //! Get best match for incomplete airline ICAO code
            //! \threadsafe
            BlackMisc::Aviation::CAirlineIcaoCode smartAirlineIcaoSelector(const BlackMisc::Aviation::CAirlineIcaoCode &icaoPattern, const BlackMisc::Aviation::CCallsign &callsign = BlackMisc::Aviation::CCallsign()) const;

            //! Get aircraft ICAO information count
            //! \threadsafe
            int getAircraftIcaoCodesCount() const;

            //! Get airline ICAO information count
            //! \threadsafe
            int getAirlineIcaoCodesCount() const;

            //! All data read?
            //! \threadsafe
            bool areAllDataRead() const;

            //! Read from static DB data file
            bool readFromJsonFiles(const QString &dir, BlackMisc::Network::CEntityFlags::Entity whatToRead = BlackMisc::Network::CEntityFlags::AllIcaoAndCountries);

            //! Read from static DB data file
            bool readFromJsonFilesInBackground(const QString &dir, BlackMisc::Network::CEntityFlags::Entity whatToRead = BlackMisc::Network::CEntityFlags::AllIcaoAndCountries);

            //! Write to static DB data file
            bool writeToJsonFiles(const QString &dir) const;

            // cache handling for base class
            virtual QDateTime getCacheTimestamp(BlackMisc::Network::CEntityFlags::Entity entity) const override;
            virtual int getCacheCount(BlackMisc::Network::CEntityFlags::Entity entity) const override;
            virtual void syncronizeCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;

        protected:
            // cache handling for base class
            virtual void invalidateCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;
            virtual bool hasChangedUrl(BlackMisc::Network::CEntityFlags::Entity entity) const override;

        private slots:
            //! Aircraft have been read
            void ps_parseAircraftIcaoData(QNetworkReply *nwReply);

            //! Airlines have been read
            void ps_parseAirlineIcaoData(QNetworkReply *nwReply);

            //! Airlines have been read
            void ps_parseCountryData(QNetworkReply *nwReply);

            //! Read / re-read data file
            void ps_read(BlackMisc::Network::CEntityFlags::Entity entities, const QDateTime &newerThan);

            void ps_aircraftIcaoCacheChanged();
            void ps_airlineIcaoCacheChanged();
            void ps_countryCacheChanged();
            void ps_baseUrlCacheChanged();

        private:
            BlackMisc::CData<BlackCore::Data::DbAircraftIcaoCache> m_aircraftIcaoCache {this, &CIcaoDataReader::ps_aircraftIcaoCacheChanged };
            BlackMisc::CData<BlackCore::Data::DbAirlineIcaoCache>  m_airlineIcaoCache  {this, &CIcaoDataReader::ps_airlineIcaoCacheChanged };
            BlackMisc::CData<BlackCore::Data::DbCountryCache>      m_countryCache      {this, &CIcaoDataReader::ps_countryCacheChanged };

            //! Reader URL (we read from where?) used to detect changes of location
            BlackMisc::CData<BlackCore::Data::DbIcaoReaderBaseUrl> m_readerUrlCache {this, &CIcaoDataReader::ps_baseUrlCacheChanged };

            //! Update reader URL
            void updateReaderUrl(const BlackMisc::Network::CUrl &url);

            //! Base URL
            //! \threadsafe
            static const BlackMisc::Network::CUrl &getBaseUrl();

            //! URL
            BlackMisc::Network::CUrl getAircraftIcaoUrl(bool shared = false) const;

            //! URL
            BlackMisc::Network::CUrl getAirlineIcaoUrl(bool shared = false) const;

            //! URL
            BlackMisc::Network::CUrl getCountryUrl(bool shared = false) const;
        };
    } // ns
} // ns

#endif // guard
