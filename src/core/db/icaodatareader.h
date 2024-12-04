// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_ICAODATAREADER_H
#define SWIFT_CORE_ICAODATAREADER_H

#include <atomic>

#include <QObject>
#include <QString>

#include "core/data/dbcaches.h"
#include "core/db/databasereader.h"
#include "core/swiftcoreexport.h"
#include "misc/aviation/aircrafticaocodelist.h"
#include "misc/aviation/airlineicaocodelist.h"
#include "misc/country.h"
#include "misc/countrylist.h"
#include "misc/datacache.h"
#include "misc/network/entityflags.h"
#include "misc/network/url.h"

class QDateTime;
class QNetworkReply;

namespace swift::core::db
{
    //! Read ICAO data from Database
    class SWIFT_CORE_EXPORT CIcaoDataReader : public CDatabaseReader
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CIcaoDataReader(QObject *owner, const CDatabaseReaderConfigList &config);

        //! Get aircraft ICAO information
        //! \threadsafe
        swift::misc::aviation::CAircraftIcaoCodeList getAircraftIcaoCodes() const;

        //! Get aircraft ICAO information count
        //! \threadsafe
        int getAircraftIcaoCodesCount() const;

        //! Get aircraft ICAO information for designator
        //! \threadsafe
        swift::misc::aviation::CAircraftIcaoCode getAircraftIcaoCodeForDesignator(const QString &designator) const;

        //! Get aircraft ICAO information for designator
        //! \threadsafe
        swift::misc::aviation::CAircraftIcaoCodeList getAircraftIcaoCodesForDesignator(const QString &designator) const;

        //! Get aircraft ICAO information for IATA code
        //! \threadsafe
        swift::misc::aviation::CAircraftIcaoCodeList getAircraftIcaoCodesForIataCode(const QString &iataCode) const;

        //! Get aircraft ICAO information for key
        //! \threadsafe
        swift::misc::aviation::CAircraftIcaoCode getAircraftIcaoCodeForDbKey(int key) const;

        //! \copydoc swift::misc::aviation::CAircraftIcaoCodeList::containsDesignator
        //! \threadsafe
        bool containsAircraftIcaoDesignator(const QString &designator) const;

        //! Get best match for incomplete aircraft ICAO code
        //! \threadsafe
        swift::misc::aviation::CAircraftIcaoCode
        smartAircraftIcaoSelector(const swift::misc::aviation::CAircraftIcaoCode &icaoPattern) const;

        //! Get countries
        //! \threadsafe
        swift::misc::CCountryList getCountries() const;

        //! Get countries count
        //! \threadsafe
        int getCountriesCount() const;

        //! Get country for ISO code
        //! \threadsafe
        swift::misc::CCountry getCountryForIsoCode(const QString &isoCode) const;

        //! Get country for ISO name
        //! \threadsafe
        swift::misc::CCountry getCountryForName(const QString &name) const;

        //! Get airline ICAO information
        //! \threadsafe
        swift::misc::aviation::CAirlineIcaoCodeList getAirlineIcaoCodes() const;

        //! Get airline ICAO information count
        //! \threadsafe
        int getAirlineIcaoCodesCount() const;

        //! \copydoc swift::misc::aviation::CAirlineIcaoCodeList::findByVDesignator
        //! \threadsafe
        swift::misc::aviation::CAirlineIcaoCodeList getAirlineIcaoCodesForDesignator(const QString &designator) const;

        //! \copydoc swift::misc::aviation::CAirlineIcaoCodeList::containsVDesignator
        //! \threadsafe
        bool containsAirlineIcaoDesignator(const QString &designator) const;

        //! \copydoc swift::misc::aviation::CAirlineIcaoCodeList::findByUniqueVDesignatorOrDefault
        //! \threadsafe
        swift::misc::aviation::CAirlineIcaoCode
        getAirlineIcaoCodeForUniqueDesignatorOrDefault(const QString &designator, bool preferOperatingAirlines) const;

        //! \copydoc swift::misc::aviation::CAirlineIcaoCodeList::findByIataCode
        //! \threadsafe
        swift::misc::aviation::CAirlineIcaoCodeList getAirlineIcaoCodesForIataCode(const QString &iataCode) const;

        //! \copydoc swift::misc::aviation::CAirlineIcaoCodeList::findByUniqueIataCodeOrDefault
        //! \threadsafe
        swift::misc::aviation::CAirlineIcaoCode
        getAirlineIcaoCodeForUniqueIataCodeOrDefault(const QString &iataCode) const;

        //! Get airline ICAO information for key
        //! \threadsafe
        swift::misc::aviation::CAirlineIcaoCode getAirlineIcaoCodeForDbKey(int key) const;

        //! Get best match for airline ICAO code
        //! \threadsafe
        swift::misc::aviation::CAirlineIcaoCode smartAirlineIcaoSelector(
            const swift::misc::aviation::CAirlineIcaoCode &icaoPattern,
            const swift::misc::aviation::CCallsign &callsign = swift::misc::aviation::CCallsign()) const;

        //! Get aircraft categories
        //! \threadsafe
        swift::misc::aviation::CAircraftCategoryList getAircraftCategories() const;

        //! Get aircraft category  count
        //! \threadsafe
        int getAircraftCategoryCount() const;

        //! All data read?
        //! \threadsafe
        bool areAllDataRead() const;

        //! Write to static DB data file
        bool writeToJsonFiles(const QString &dir);

        // data read from local data
        virtual swift::misc::CStatusMessageList readFromJsonFiles(const QString &dir,
                                                                  swift::misc::network::CEntityFlags::Entity whatToRead,
                                                                  bool overrideNewerOnly) override;
        virtual bool readFromJsonFilesInBackground(const QString &dir,
                                                   swift::misc::network::CEntityFlags::Entity whatToRead,
                                                   bool overrideNewerOnly) override;

        // cache handling for base class
        virtual swift::misc::network::CEntityFlags::Entity getSupportedEntities() const override;
        virtual QDateTime getCacheTimestamp(swift::misc::network::CEntityFlags::Entity entity) const override;
        virtual int getCacheCount(swift::misc::network::CEntityFlags::Entity entity) const override;
        virtual swift::misc::network::CEntityFlags::Entity getEntitiesWithCacheCount() const override;
        virtual swift::misc::network::CEntityFlags::Entity
        getEntitiesWithCacheTimestampNewerThan(const QDateTime &threshold) const override;
        virtual void synchronizeCaches(swift::misc::network::CEntityFlags::Entity entities) override;
        virtual void admitCaches(swift::misc::network::CEntityFlags::Entity entities) override;

    protected:
        // cache handling for base class
        virtual void invalidateCaches(swift::misc::network::CEntityFlags::Entity entities) override;
        virtual bool hasChangedUrl(swift::misc::network::CEntityFlags::Entity entity,
                                   swift::misc::network::CUrl &oldUrlInfo,
                                   swift::misc::network::CUrl &newUrlInfo) const override;
        virtual swift::misc::network::CUrl getDbServiceBaseUrl() const override;

    private:
        swift::misc::CData<swift::core::data::TDbAircraftIcaoCache> m_aircraftIcaoCache {
            this, &CIcaoDataReader::aircraftIcaoCacheChanged
        };
        swift::misc::CData<swift::core::data::TDbAirlineIcaoCache> m_airlineIcaoCache {
            this, &CIcaoDataReader::airlineIcaoCacheChanged
        };
        swift::misc::CData<swift::core::data::TDbCountryCache> m_countryCache { this,
                                                                                &CIcaoDataReader::countryCacheChanged };
        swift::misc::CData<swift::core::data::TDbAircraftCategoryCache> m_categoryCache {
            this, &CIcaoDataReader::aircraftCategoryCacheChanged
        };
        std::atomic_bool m_syncedAircraftIcaoCache { false }; //!< already synchronized?
        std::atomic_bool m_syncedAirlineIcaoCache { false }; //!< already synchronized?
        std::atomic_bool m_syncedCountryCache { false }; //!< already synchronized?
        std::atomic_bool m_syncedCategories { false }; //!< already synchronized?

        //! \copydoc CDatabaseReader::read
        virtual void read(swift::misc::network::CEntityFlags::Entity entities,
                          swift::misc::db::CDbFlags::DataRetrievalModeFlag mode, const QDateTime &newerThan) override;

        //! Reader URL (we read from where?) used to detect changes of location
        swift::misc::CData<swift::core::data::TDbIcaoReaderBaseUrl> m_readerUrlCache {
            this, &CIcaoDataReader::baseUrlCacheChanged
        };

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
        void updateReaderUrl(const swift::misc::network::CUrl &url);

        //! URL
        swift::misc::network::CUrl getAircraftIcaoUrl(swift::misc::db::CDbFlags::DataRetrievalModeFlag mode) const;

        //! URL
        swift::misc::network::CUrl getAirlineIcaoUrl(swift::misc::db::CDbFlags::DataRetrievalModeFlag mode) const;

        //! URL
        swift::misc::network::CUrl getCountryUrl(swift::misc::db::CDbFlags::DataRetrievalModeFlag mode) const;

        //! URL
        swift::misc::network::CUrl getAircraftCategoryUrl(swift::misc::db::CDbFlags::DataRetrievalModeFlag mode) const;
    };
} // namespace swift::core::db

#endif // SWIFT_CORE_ICAODATAREADER_H
