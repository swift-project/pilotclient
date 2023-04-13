/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_ICAODATAREADER_H
#define BLACKCORE_ICAODATAREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/db/databasereader.h"
#include "blackcore/data/dbcaches.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/url.h"
#include "blackmisc/country.h"
#include "blackmisc/countrylist.h"
#include "blackmisc/datacache.h"

#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <atomic>

class QDateTime;
class QNetworkReply;

namespace BlackCore::Db
{
    //! Read ICAO data from Database
    class BLACKCORE_EXPORT CIcaoDataReader : public CDatabaseReader
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CIcaoDataReader(QObject *owner, const CDatabaseReaderConfigList &config);

        //! Get aircraft ICAO information
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCodeList getAircraftIcaoCodes() const;

        //! Get aircraft ICAO information count
        //! \threadsafe
        int getAircraftIcaoCodesCount() const;

        //! Get aircraft ICAO information for designator
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDesignator(const QString &designator) const;

        //! Get aircraft ICAO information for designator
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCodeList getAircraftIcaoCodesForDesignator(const QString &designator) const;

        //! Get aircraft ICAO information for IATA code
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCodeList getAircraftIcaoCodesForIataCode(const QString &iataCode) const;

        //! Get aircraft ICAO information for key
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDbKey(int key) const;

        //! \copydoc BlackMisc::Aviation::CAircraftIcaoCodeList::containsDesignator
        //! \threadsafe
        bool containsAircraftIcaoDesignator(const QString &designator) const;

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

        //! Get airline ICAO information
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodes() const;

        //! Get airline ICAO information count
        //! \threadsafe
        int getAirlineIcaoCodesCount() const;

        //! \copydoc BlackMisc::Aviation::CAirlineIcaoCodeList::findByVDesignator
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodesForDesignator(const QString &designator) const;

        //! \copydoc BlackMisc::Aviation::CAirlineIcaoCodeList::containsVDesignator
        //! \threadsafe
        bool containsAirlineIcaoDesignator(const QString &designator) const;

        //! \copydoc BlackMisc::Aviation::CAirlineIcaoCodeList::findByUniqueVDesignatorOrDefault
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForUniqueDesignatorOrDefault(const QString &designator, bool preferOperatingAirlines) const;

        //! \copydoc BlackMisc::Aviation::CAirlineIcaoCodeList::findByIataCode
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodesForIataCode(const QString &iataCode) const;

        //! \copydoc BlackMisc::Aviation::CAirlineIcaoCodeList::findByUniqueIataCodeOrDefault
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForUniqueIataCodeOrDefault(const QString &iataCode) const;

        //! Get airline ICAO information for key
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForDbKey(int key) const;

        //! Get best match for airline ICAO code
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode smartAirlineIcaoSelector(const BlackMisc::Aviation::CAirlineIcaoCode &icaoPattern, const BlackMisc::Aviation::CCallsign &callsign = BlackMisc::Aviation::CCallsign()) const;

        //! Get aircraft categories
        //! \threadsafe
        BlackMisc::Aviation::CAircraftCategoryList getAircraftCategories() const;

        //! Get aircraft category  count
        //! \threadsafe
        int getAircraftCategoryCount() const;

        //! All data read?
        //! \threadsafe
        bool areAllDataRead() const;

        //! Write to static DB data file
        bool writeToJsonFiles(const QString &dir);

        // data read from local data
        virtual BlackMisc::CStatusMessageList readFromJsonFiles(const QString &dir, BlackMisc::Network::CEntityFlags::Entity whatToRead, bool overrideNewerOnly) override;
        virtual bool readFromJsonFilesInBackground(const QString &dir, BlackMisc::Network::CEntityFlags::Entity whatToRead, bool overrideNewerOnly) override;

        // cache handling for base class
        virtual BlackMisc::Network::CEntityFlags::Entity getSupportedEntities() const override;
        virtual QDateTime getCacheTimestamp(BlackMisc::Network::CEntityFlags::Entity entity) const override;
        virtual int getCacheCount(BlackMisc::Network::CEntityFlags::Entity entity) const override;
        virtual BlackMisc::Network::CEntityFlags::Entity getEntitiesWithCacheCount() const override;
        virtual BlackMisc::Network::CEntityFlags::Entity getEntitiesWithCacheTimestampNewerThan(const QDateTime &threshold) const override;
        virtual void synchronizeCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;
        virtual void admitCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;

    protected:
        // cache handling for base class
        virtual void invalidateCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;
        virtual bool hasChangedUrl(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CUrl &oldUrlInfo, BlackMisc::Network::CUrl &newUrlInfo) const override;
        virtual BlackMisc::Network::CUrl getDbServiceBaseUrl() const override;

    private:
        BlackMisc::CData<BlackCore::Data::TDbAircraftIcaoCache> m_aircraftIcaoCache { this, &CIcaoDataReader::aircraftIcaoCacheChanged };
        BlackMisc::CData<BlackCore::Data::TDbAirlineIcaoCache> m_airlineIcaoCache { this, &CIcaoDataReader::airlineIcaoCacheChanged };
        BlackMisc::CData<BlackCore::Data::TDbCountryCache> m_countryCache { this, &CIcaoDataReader::countryCacheChanged };
        BlackMisc::CData<BlackCore::Data::TDbAircraftCategoryCache> m_categoryCache { this, &CIcaoDataReader::aircraftCategoryCacheChanged };
        std::atomic_bool m_syncedAircraftIcaoCache { false }; //!< already synchronized?
        std::atomic_bool m_syncedAirlineIcaoCache { false }; //!< already synchronized?
        std::atomic_bool m_syncedCountryCache { false }; //!< already synchronized?
        std::atomic_bool m_syncedCategories { false }; //!< already synchronized?

        //! \copydoc CDatabaseReader::read
        virtual void read(BlackMisc::Network::CEntityFlags::Entity entities,
                          BlackMisc::Db::CDbFlags::DataRetrievalModeFlag mode, const QDateTime &newerThan) override;

        //! Reader URL (we read from where?) used to detect changes of location
        BlackMisc::CData<BlackCore::Data::TDbIcaoReaderBaseUrl> m_readerUrlCache { this, &CIcaoDataReader::baseUrlCacheChanged };

        //! Aircraft have been read
        void parseAircraftIcaoData(QNetworkReply *nwReply);

        //! Airlines have been read
        void parseAirlineIcaoData(QNetworkReply *nwReply);

        //! Countries have been read
        void parseCountryData(QNetworkReply *nwReply);

        //! Categories have been read
        void parseAircraftCategoryData(QNetworkReply *nwReplyPtr);

        //! Cache has changed elsewhere
        void aircraftIcaoCacheChanged();

        //! Cache has changed elsewhere
        void airlineIcaoCacheChanged();

        //! Cache has changed elsewhere
        void countryCacheChanged();

        //! Cache has changed elsewhere
        void aircraftCategoryCacheChanged();

        //! Cache has changed elsewhere
        void baseUrlCacheChanged();

        //! Update reader URL
        void updateReaderUrl(const BlackMisc::Network::CUrl &url);

        //! URL
        BlackMisc::Network::CUrl getAircraftIcaoUrl(BlackMisc::Db::CDbFlags::DataRetrievalModeFlag mode) const;

        //! URL
        BlackMisc::Network::CUrl getAirlineIcaoUrl(BlackMisc::Db::CDbFlags::DataRetrievalModeFlag mode) const;

        //! URL
        BlackMisc::Network::CUrl getCountryUrl(BlackMisc::Db::CDbFlags::DataRetrievalModeFlag mode) const;

        //! URL
        BlackMisc::Network::CUrl getAircraftCategoryUrl(BlackMisc::Db::CDbFlags::DataRetrievalModeFlag mode) const;
    };
} // ns

#endif // guard
