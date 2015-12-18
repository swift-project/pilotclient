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
#include "blackcore/databasereader.h"
#include "blackcore/data/globalsetup.h"
#include "blackmisc/countrylist.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/network/entityflags.h"

#include <QObject>
#include <QTimer>
#include <QNetworkReply>
#include <QReadWriteLock>

namespace BlackCore
{
    //! Read ICAO data from Database
    class BLACKCORE_EXPORT CIcaoDataReader : public CDatabaseReader
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CIcaoDataReader(QObject *owner);

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
        BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForDesignator(const QString &designator) const;

        //! Get airline ICAO information for key
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForDbKey(int key) const;

        //! Get best match for incomplete airline ICAO code
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode smartAirlineIcaoSelector(const BlackMisc::Aviation::CAirlineIcaoCode &icaoPattern) const;

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

    signals:
        //! Combined read signal
        void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

    private slots:
        //! Aircraft have been read
        void ps_parseAircraftIcaoData(QNetworkReply *nwReply);

        //! Airlines have been read
        void ps_parseAirlineIcaoData(QNetworkReply *nwReply);

        //! Airlines have been read
        void ps_parseCountryData(QNetworkReply *nwReply);

        //! Read / re-read data file
        void ps_read(BlackMisc::Network::CEntityFlags::Entity entities);

    private:
        QNetworkAccessManager *m_networkManagerAircraft  = nullptr;
        QNetworkAccessManager *m_networkManagerAirlines  = nullptr;
        QNetworkAccessManager *m_networkManagerCountries = nullptr;
        BlackMisc::Aviation::CAircraftIcaoCodeList m_aircraftIcaos;
        BlackMisc::Aviation::CAirlineIcaoCodeList  m_airlineIcaos;
        BlackMisc::CCountryList                    m_countries;

        mutable QReadWriteLock m_lockAirline;
        mutable QReadWriteLock m_lockAircraft;
        mutable QReadWriteLock m_lockCountry;

        BlackMisc::CData<BlackCore::Data::GlobalSetup> m_setup {this}; //!< setup cache

        //! Base URL
        BlackMisc::Network::CUrl getBaseUrl() const;

        //! URL
        BlackMisc::Network::CUrl getAircraftIcaoUrl() const;

        //! URL
        BlackMisc::Network::CUrl getAirlineIcaoUrl() const;

        //! URL
        BlackMisc::Network::CUrl getCountryUrl() const;
    };
}

#endif // guard
