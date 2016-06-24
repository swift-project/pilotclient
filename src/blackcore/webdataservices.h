/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_WEB_DATASERVICES_H
#define BLACKCORE_WEB_DATASERVICES_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/webreaderflags.h"
#include "blackcore/db/databasereader.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/country.h"
#include "blackmisc/countrylist.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/network/voicecapabilities.h"
#include "blackmisc/restricted.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/weather/metar.h"
#include "blackmisc/weather/metarset.h"

#include <QDateTime>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>


namespace BlackMisc
{
    class CLogCategoryList;
    template <typename T> class Restricted;

    namespace Aviation { class CCallsign; }
    namespace Simulation { class CSimulatedAircraft; }
}

namespace BlackCore
{
    class CApplication;

    namespace Vatsim
    {
        class CVatsimBookingReader;
        class CVatsimDataFileReader;
        class CVatsimMetarReader;
        class CVatsimStatusFileReader;
    }

    namespace Db
    {
        class CDatabaseWriter;
        class CIcaoDataReader;
        class CModelDataReader;
        class CInfoDataReader;
    }

    /*!
     * Encapsulates reading data from web sources
     */
    class BLACKCORE_EXPORT CWebDataServices :
        public QObject
    {
        Q_OBJECT

    public:
        //! Log categories
        static const BlackMisc::CLogCategoryList &getLogCategories();

        //! Constructor, only allowed from BlackCore::CApplication
        CWebDataServices(CWebReaderFlags::WebReader readerFlags, const BlackCore::Db::CDatabaseReaderConfigList &dbReaderConfig, BlackMisc::Restricted<CApplication>, QObject *parent = nullptr);

        //! Shutdown
        void gracefulShutdown();

        //! Read ATC bookings (used to re-read)
        void readAtcBookingsInBackground() const;

        //! Booking reader
        Vatsim::CVatsimBookingReader *getBookingReader() const { return m_vatsimBookingReader; }

        //! Data file reader
        Vatsim::CVatsimDataFileReader *getDataFileReader() const { return m_vatsimDataFileReader; }

        //! Metar reader
        Vatsim::CVatsimMetarReader *getMetarReader() const { return m_vatsimMetarReader; }

        //! Info data reader
        Db::CInfoDataReader *getInfoDataReader() const { return m_infoDataReader; }

        //! DB writer class
        Db::CDatabaseWriter *getDatabaseWriter() const { return m_databaseWriter; }

        //! Reader flags
        CWebReaderFlags::WebReader getReaderFlags() const { return m_readers; }

        //! FSD servers
        //! \threadsafe
        BlackMisc::Network::CServerList getVatsimFsdServers() const;

        //! Voice servers
        //! \threadsafe
        BlackMisc::Network::CServerList getVatsimVoiceServers() const;

        //! METAR URLs (from status file)
        //! \threadsafe
        BlackMisc::Network::CUrlList getVatsimMetarUrls() const;

        //! Data file locations (from status file)
        //! \threadsafe
        BlackMisc::Network::CUrlList getVatsimDataFileUrls() const;

        //! Users by callsign
        //! \threadsafe
        BlackMisc::Network::CUserList getUsersForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! ATC stations by callsign
        //! \threadsafe
        BlackMisc::Aviation::CAtcStationList getAtcStationsForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! Voice capabilities for given callsign
        //! \threadsafe
        BlackMisc::Network::CVoiceCapabilities getVoiceCapabilityForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! Update with web data
        //! \threadsafe
        void updateWithVatsimDataFileData(BlackMisc::Simulation::CSimulatedAircraft &aircraftToBeUdpated) const;

        //! Distributors
        //! \threadsafe
        BlackMisc::Simulation::CDistributorList getDistributors() const;

        //! Distributors count
        //! \threadsafe
        int getDistributorsCount() const;

        //! Use distributor object to select the best complete distributor from DB
        //! \threadsafe
        BlackMisc::Simulation::CDistributor smartDistributorSelector(const BlackMisc::Simulation::CDistributor &distributor) const;

        //! Liveries
        //! \threadsafe
        BlackMisc::Aviation::CLiveryList getLiveries() const;

        //! Liveries count
        //! \threadsafe
        int getLiveriesCount() const;

        //! Livery for its combined code
        //! \threadsafe
        BlackMisc::Aviation::CLivery getLiveryForCombinedCode(const QString &combinedCode) const;

        //! Standard livery for airline code
        //! \threadsafe
        BlackMisc::Aviation::CLivery getStdLiveryForAirlineCode(const BlackMisc::Aviation::CAirlineIcaoCode &icao) const;

        //! Livery for id
        //! \threadsafe
        BlackMisc::Aviation::CLivery getLiveryForDbKey(int id) const;

        //! Use a livery as template and select the best complete livery from DB for it
        //! \threadsafe
        BlackMisc::Aviation::CLivery smartLiverySelector(const BlackMisc::Aviation::CLivery &livery) const;

        //! Models
        //! \threadsafe
        BlackMisc::Simulation::CAircraftModelList getModels() const;

        //! Models count
        //! \threadsafe
        int getModelsCount() const;

        //! Model keys
        //! \threadsafe
        QList<int> getModelDbKeys() const;

        //! Model strings
        //! \threadsafe
        QStringList getModelStrings() const;

        //! Models for combined code and aircraft designator
        //! \threadsafe
        BlackMisc::Simulation::CAircraftModelList getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode) const;

        //! Model for key if any
        //! \threadsafe
        BlackMisc::Simulation::CAircraftModel getModelForModelString(const QString &modelKey) const;

        //! Aircraft ICAO codes
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCodeList getAircraftIcaoCodes() const;

        //! Aircraft ICAO codes count
        //! \threadsafe
        int getAircraftIcaoCodesCount() const;

        //! ICAO code for designator
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDesignator(const QString &designator) const;

        //! ICAO code for id
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDbKey(int id) const;

        //! Use an ICAO object to select the best complete ICAO object from DB for it
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCode smartAircraftIcaoSelector(const BlackMisc::Aviation::CAircraftIcaoCode &icao) const;

        //! Airline ICAO codes
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodes() const;

        //! Airline ICAO codes count
        //! \threadsafe
        int getAirlineIcaoCodesCount() const;

        //! ICAO codes for designator
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodesForDesignator(const QString &designator) const;

        //! ICAO code for id
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForDbKey(int id) const;

        //! Smart airline selector
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode smartAirlineIcaoSelector(const BlackMisc::Aviation::CAirlineIcaoCode &code) const;

        //! Countries
        //! \threadsafe
        BlackMisc::CCountryList getCountries() const;

        //! Country count
        //! \threadsafe
        int getCountriesCount() const;

        //! Country by ISO code (GB, US...)
        //! \threadsafe
        BlackMisc::CCountry getCountryForIsoCode(const QString &iso) const;

        //! Country by name (France, China ..)
        //! \threadsafe
        BlackMisc::CCountry getCountryForName(const QString &name) const;

        //! Get METARs
        //! \threadsafe
        BlackMisc::Weather::CMetarSet getMetars() const;

        //! Get METAR for airport
        //! \threadsafe
        BlackMisc::Weather::CMetar getMetarForAirport(const BlackMisc::Aviation::CAirportIcaoCode &icao) const;

        //! Get METARs count
        //! \threadsafe
        int getMetarsCount() const;

        //! Publish models to database
        BlackMisc::CStatusMessageList asyncPublishModels(const BlackMisc::Simulation::CAircraftModelList &models) const;

        //! Trigger read of new data
        BlackMisc::Network::CEntityFlags::Entity triggerRead(BlackMisc::Network::CEntityFlags::Entity whatToRead, const QDateTime &dateTime = QDateTime());

        //! Can connect to swift DB?
        bool canConnectSwiftDb() const;

        //! Write data to disk (mainly for testing scenarios)
        bool writeDbDataToDisk(const QString &dir) const;

        //! Load DB data from disk (mainly for testing scenarios)
        bool readDbDataFromDisk(const QString &dir, bool inBackground);

    signals:
        //! Combined read signal
        void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

    public slots:
        //! First read (allows to immediately read in background)
        void readInBackground(BlackMisc::Network::CEntityFlags::Entity entities = BlackMisc::Network::CEntityFlags::AllEntities, int delayMs = 0);

    private slots:
        //! ATC bookings received
        void ps_receivedBookings(const BlackMisc::Aviation::CAtcStationList &bookedStations);

        //! Received METAR data
        void ps_receivedMetars(const BlackMisc::Weather::CMetarSet &metars);

        //! Data file has been read
        void ps_dataFileRead(int lines);

        //! Read finished from reader
        void ps_readFromSwiftDb(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

        //! Setup changed
        void ps_setupChanged();

    private:
        //! Init the readers
        void initReaders(CWebReaderFlags::WebReader flags);

        //! Init the writers
        void initWriters();

        //! Call CWebDataServices::readInBackground by single shot
        void singleShotReadInBackground(BlackMisc::Network::CEntityFlags::Entity entities, int delayMs);

        CWebReaderFlags::WebReader               m_readers = CWebReaderFlags::WebReaderFlag::None;  //!< which readers are available
        BlackCore::Db::CDatabaseReaderConfigList m_dbReaderConfig;                                  //!< how to read DB data
        bool                                     m_initialRead = false;                             //!< Initial read started
        int                                      m_infoObjectTrials = 0;                            //!< Tried to read info objects

        // for reading XML and VATSIM data files
        Vatsim::CVatsimStatusFileReader *m_vatsimStatusReader   = nullptr;
        Vatsim::CVatsimBookingReader    *m_vatsimBookingReader  = nullptr;
        Vatsim::CVatsimDataFileReader   *m_vatsimDataFileReader = nullptr;
        Vatsim::CVatsimMetarReader      *m_vatsimMetarReader    = nullptr;
        Db::CIcaoDataReader             *m_icaoDataReader       = nullptr;
        Db::CModelDataReader            *m_modelDataReader      = nullptr;
        Db::CInfoDataReader             *m_infoDataReader       = nullptr;

        // writing objects directly into DB
        Db::CDatabaseWriter     *m_databaseWriter       = nullptr;
    };
} // namespace

#endif
