// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_WEBDATASERVICES_H
#define BLACKCORE_WEBDATASERVICES_H

#include "blackcore/db/databasereader.h"
#include "blackcore/webreaderflags.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/url.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/voicecapabilities.h"
#include "blackmisc/weather/metarlist.h"
#include "blackmisc/weather/metar.h"
#include "blackmisc/restricted.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/countrylist.h"

#include <QDateTime>
#include <QList>
#include <QSet>
#include <QObject>
#include <QString>
#include <QStringList>
#include <atomic>

namespace BlackMisc
{
    class CLogCategoryList;

    namespace Aviation
    {
        class CCallsign;
    }
    namespace Simulation
    {
        class CSimulatedAircraft;
        class CAutoPublishData;
    }
}

namespace BlackCore
{
    class CApplication;

    namespace Vatsim
    {
        class CVatsimDataFileReader;
        class CVatsimMetarReader;
        class CVatsimStatusFileReader;
        class CVatsimServerFileReader;
    }

    namespace Db
    {
        class CAirportDataReader;
        class CDatabaseWriter;
        class CDatabaseReader;
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
        static const QStringList &getLogCategories();

        //! Constructor, only allowed from BlackCore::CApplication
        CWebDataServices(CWebReaderFlags::WebReader readerFlags, const BlackCore::Db::CDatabaseReaderConfigList &dbReaderConfig, BlackMisc::Restricted<CApplication>, QObject *parent = nullptr);

        //! Destructor
        virtual ~CWebDataServices() override;

        //! Shutdown
        void gracefulShutdown();

        //! Data file reader
        Vatsim::CVatsimDataFileReader *getVatsimDataFileReader() const { return m_vatsimDataFileReader; }

        //! Metar reader
        Vatsim::CVatsimMetarReader *getMetarReader() const { return m_vatsimMetarReader; }

        //! DB info data reader
        BlackCore::Db::CInfoDataReader *getDbInfoDataReader() const { return m_dbInfoDataReader; }

        //! Shared info data reader
        BlackCore::Db::CInfoDataReader *getSharedInfoDataReader() const { return m_sharedInfoDataReader; }

        //! DB writer class
        Db::CDatabaseWriter *getDatabaseWriter() const { return m_databaseWriter; }

        //! All DB entities for those readers used and not ignored
        BlackMisc::Network::CEntityFlags::Entity allDbEntitiesForUsedReaders() const;

        //! FSD servers
        //! \threadsafe
        BlackMisc::Network::CServerList getVatsimFsdServers() const;

        //! METAR URL (from status file)
        //! \threadsafe
        BlackMisc::Network::CUrl getVatsimMetarUrl() const;

        //! Data file location (from status file)
        //! \threadsafe
        BlackMisc::Network::CUrl getVatsimDataFileUrl() const;

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

        //! Distributor for key
        //! \threadsafe
        BlackMisc::Simulation::CDistributor getDistributorForDbKey(const QString &key) const;

        //! Use distributor object to select the best complete distributor from DB
        //! \threadsafe
        BlackMisc::Simulation::CDistributor smartDistributorSelector(const BlackMisc::Simulation::CDistributor &distributor) const;

        //! Use distributor object to select the best complete distributor from DB
        //! \threadsafe
        BlackMisc::Simulation::CDistributor smartDistributorSelector() const;

        //! Best match specified by distributor / model
        //! \threadsafe
        BlackMisc::Simulation::CDistributor smartDistributorSelector(const BlackMisc::Simulation::CDistributor &distributor, const BlackMisc::Simulation::CAircraftModel &model) const;

        //! Liveries
        //! \threadsafe
        BlackMisc::Aviation::CLiveryList getLiveries() const;

        //! Liveries count
        //! \threadsafe
        int getLiveriesCount() const;

        //! Livery for its combined code
        //! \threadsafe
        BlackMisc::Aviation::CLivery getLiveryForCombinedCode(const QString &combinedCode) const;

        //! The temp. livery if available
        //! \threadsafe
        BlackMisc::Aviation::CLivery getTempLiveryOrDefault() const;

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
        QSet<int> getModelDbKeys() const;

        //! Model strings
        //! \threadsafe
        QStringList getModelStrings(bool sort = false) const;

        //! Model completer string
        //! \threadsafe
        QStringList getModelCompleterStrings(bool sorted = true, const BlackMisc::Simulation::CSimulatorInfo &simulator = { BlackMisc::Simulation::CSimulatorInfo::All }) const;

        //! Models for combined code and aircraft designator
        //! \threadsafe
        BlackMisc::Simulation::CAircraftModelList getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode) const;

        //! Model for model string if any
        //! \threadsafe
        BlackMisc::Simulation::CAircraftModel getModelForModelString(const QString &modelString) const;

        //! Existing modelstring?
        //! \threadsafe
        bool containsModelString(const QString &modelString) const;

        //! Model for key if any
        //! \threadsafe
        BlackMisc::Simulation::CAircraftModel getModelForDbKey(int dbKey) const;

        //! Aircraft ICAO codes
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCodeList getAircraftIcaoCodes() const;

        //! Aircraft ICAO codes count
        //! \threadsafe
        int getAircraftIcaoCodesCount() const;

        //! ICAO code for designator
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDesignator(const QString &designator) const;

        //! ICAO code for designator count
        //! \threadsafe
        int getAircraftIcaoCodesForDesignatorCount(const QString &designator) const;

        //! Aircraft ICAO designators for airline
        //! \threadsafe
        QSet<QString> getAircraftDesignatorsForAirline(const BlackMisc::Aviation::CAirlineIcaoCode &airline) const;

        //! Aircraft ICAO codes for airline
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCodeList getAircraftIcaoCodesForAirline(const BlackMisc::Aviation::CAirlineIcaoCode &airline) const;

        //! Aircraft categories
        //! \threadsafe
        BlackMisc::Aviation::CAircraftCategoryList getAircraftCategories() const;

        //! Aircraft categories count
        //! \threadsafe
        int getAircraftCategoriesCount() const;

        //! Contains the given designator?
        //! \threadsafe
        bool containsAircraftIcaoDesignator(const QString &designator) const;

        //! ICAO codes for designator
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCodeList getAircraftIcaoCodesForDesignator(const QString &designator) const;

        //! ICAO code for id
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDbKey(int id) const;

        //! Use an ICAO object to select the best complete ICAO object from DB for it
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCode smartAircraftIcaoSelector(const BlackMisc::Aviation::CAircraftIcaoCode &icao) const;

        //! Airline ICAO codes
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodes() const;

        //! Contains the given designator?
        //! \threadsafe
        bool containsAirlineIcaoDesignator(const QString &designator) const;

        //! ICAO code if unique, otherwise default
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForUniqueDesignatorOrDefault(const QString &designator, bool preferOperatingAirlines) const;

        //! ICAO code if unique, otherwise default
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForUniqueIataCodeOrDefault(const QString &iataCode) const;

        //! Airline names
        //! \threadsafe
        QStringList getAirlineNames() const;

        //! Airline telephony designators
        //! \threadsafe
        QStringList getTelephonyDesignators() const;

        //! Airline ICAO codes count
        //! \threadsafe
        int getAirlineIcaoCodesCount() const;

        //! Airline ICAO codes for designator
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodesForDesignator(const QString &designator) const;

        //! Airline ICAO codes for designator count
        //! \threadsafe
        int getAirlineIcaoCodesForDesignatorCount(const QString &designator) const;

        //! ICAO code for id
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForDbKey(int id) const;

        //! Smart airline selector
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode smartAirlineIcaoSelector(const BlackMisc::Aviation::CAirlineIcaoCode &code, const BlackMisc::Aviation::CCallsign &callsign = BlackMisc::Aviation::CCallsign()) const;

        //! ICAO code for callsign (e.g. DLH123 -> DLH)
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode findBestMatchByCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

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

        //! Get airports
        //! \threadsafe
        BlackMisc::Aviation::CAirportList getAirports() const;

        //! Get airports count
        //! \threadsafe
        int getAirportsCount() const;

        //! Get airport for ICAO designator
        //! \threadsafe
        BlackMisc::Aviation::CAirport getAirportForIcaoDesignator(const QString &icao) const;

        //! Get airport for name of location
        //! \threadsafe
        BlackMisc::Aviation::CAirport getAirportForNameOrLocation(const QString &nameOrLocation) const;

        //! Get METARs
        //! \threadsafe
        BlackMisc::Weather::CMetarList getMetars() const;

        //! Get METAR for airport
        //! \threadsafe
        BlackMisc::Weather::CMetar getMetarForAirport(const BlackMisc::Aviation::CAirportIcaoCode &icao) const;

        //! Validate for publishing
        //! \remark More detailed check than BlackMisc::Simulation::CAircraftModelList::validateForPublishing
        BlackMisc::CStatusMessageList validateForPublishing(
            const BlackMisc::Simulation::CAircraftModelList &modelsToBePublished,
            bool ignoreEqual,
            BlackMisc::Simulation::CAircraftModelList &validModels,
            BlackMisc::Simulation::CAircraftModelList &invalidModels) const;

        //! \copydoc BlackMisc::Simulation::CAircraftModel::isEqualForPublishing
        bool isDbModelEqualForPublishing(const BlackMisc::Simulation::CAircraftModel &modelToBeChecked, BlackMisc::CStatusMessageList *details = nullptr) const;

        //! Publish models to database
        BlackMisc::CStatusMessageList asyncPublishModels(const BlackMisc::Simulation::CAircraftModelList &modelsToBePublished) const;

        //! Auto publish to database
        BlackMisc::CStatusMessageList asyncAutoPublish(const BlackMisc::Simulation::CAutoPublishData &data) const;

        //! Trigger read of DB info objects
        void triggerReadOfDbInfoObjects();

        //! Trigger read of shared info objects
        void triggerReadOfSharedInfoObjects();

        //! Trigger read of new data
        //! \note requires info objects loaded upfront and uses the full cache logic
        BlackMisc::Network::CEntityFlags::Entity triggerRead(BlackMisc::Network::CEntityFlags::Entity whatToRead, const QDateTime &newerThan = QDateTime());

        //! Trigger reload from DB, loads the DB data and bypasses the caches checks and info objects
        BlackMisc::Network::CEntityFlags::Entity triggerLoadingDirectlyFromDb(BlackMisc::Network::CEntityFlags::Entity whatToRead, const QDateTime &newerThan = QDateTime());

        //! Trigger reload from shared files, loads the data and bypasses caches
        BlackMisc::Network::CEntityFlags::Entity triggerLoadingDirectlyFromSharedFiles(BlackMisc::Network::CEntityFlags::Entity whatToRead, bool checkCacheTsUpfront);

        //! Corresponding cache timestamp if applicable
        //! \threadsafe
        QDateTime getCacheTimestamp(BlackMisc::Network::CEntityFlags::Entity entity) const;

        //! Latest DB object timestamp, or null if there is no such timestamp
        //! \threadsafe
        QDateTime getLatestDbEntityCacheTimestamp() const;

        //! Corresponding DB timestamp if applicable
        //! \remark from Db::CInfoDataReader
        //! \threadsafe
        QDateTime getLatestDbEntityTimestamp(BlackMisc::Network::CEntityFlags::Entity entity) const;

        //! Shared info object timestamp
        //! \remark from Db::CInfoDataReader
        //! \threadsafe
        QDateTime getLatestSharedInfoObjectTimestamp(BlackMisc::Network::CEntityFlags::Entity entity) const;

        //! Entities with newer shared file (from DB Info object)
        //! \threadsafe
        BlackMisc::Network::CEntityFlags::Entity getEntitiesWithNewerSharedFile(BlackMisc::Network::CEntityFlags::Entity entities) const;

        //! Empty entities in cache
        //! \remark can also be empty because cache is not yet synchronized
        //! \threadsafe
        BlackMisc::Network::CEntityFlags::Entity getEmptyEntities(BlackMisc::Network::CEntityFlags::Entity entities = BlackMisc::Network::CEntityFlags::AllDbEntities) const;

        //! Synchronized entities either empty or with newer shared file
        //! \remark will synchronize entities
        //! \threadsafe
        BlackMisc::Network::CEntityFlags::Entity getSynchronizedEntitiesWithNewerSharedFileOrEmpty(bool syncData = true, BlackMisc::Network::CEntityFlags::Entity entities = BlackMisc::Network::CEntityFlags::AllDbEntities);

        //! Cache count for entity
        //! \threadsafe
        int getCacheCount(BlackMisc::Network::CEntityFlags::Entity entity) const;

        //! Count for entity from DB entity objects
        //! \threadsafe
        int getDbInfoObjectCount(BlackMisc::Network::CEntityFlags::Entity entity) const;

        //! Count for 1-n entities from DB entity objects
        //! \threadsafe
        int getDbInfoObjectsCount(BlackMisc::Network::CEntityFlags::Entity entities, bool stopIfNotFound = true) const;

        //! Count for entity from shared entity objects
        //! \threadsafe
        int getSharedInfoObjectCount(BlackMisc::Network::CEntityFlags::Entity entity) const;

        //! For all available DB readers the log info is generated
        QString getDbReadersLog(const QString &separator = "\n") const;

        //! For all available readers the log info is generated
        QString getReadersLog(const QString &separator = "\n") const;

        //! Has already successfully connect swift DB?
        bool hasSuccesfullyConnectedSwiftDb() const;

        //! Are all DB data for an aircraft entity available?
        bool hasDbAircraftData() const;

        //! Are DB model data available?
        bool hasDbModelData() const;

        //! Are all DB ICAO data available?
        bool hasDbIcaoData() const;

        //! Admit all DB caches specified
        void admitDbCaches(BlackMisc::Network::CEntityFlags::Entity entities);

        //! Synchronize all DB caches specified
        //! \remark can be slow if all data are forced to be loaded
        void synchronizeDbCaches(BlackMisc::Network::CEntityFlags::Entity entities);

        //! Write data to disk (mainly for testing scenarios)
        bool writeDbDataToDisk(const QString &dir);

        //! Load DB data from disk (mainly for initial data load and testing scenarios)
        //! \remark if the DB readers are alred in aother thread reads in background
        //! \sa CWebDataServices::initDbCachesFromLocalResourceFiles for reading from local resource file
        bool readDbDataFromDisk(const QString &dir, bool inBackground, bool overrideNewerOnly);

        //! Init caches from local DB files
        //! \remark the shared files coming with the installer
        BlackMisc::CStatusMessageList initDbCachesFromLocalResourceFiles(bool inBackground);

        //! Init caches from local DB files per given entities
        //! \remark the shared files coming with the installer
        BlackMisc::CStatusMessageList initDbCachesFromLocalResourceFiles(BlackMisc::Network::CEntityFlags::Entity entities, bool inBackground);

    signals:
        //! Combined read signal
        void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number, const QUrl &url);

        //! Download progress for an entity
        void entityDownloadProgress(BlackMisc::Network::CEntityFlags::Entity entity, int logId, int progress, qint64 current, qint64 max, const QUrl &url);

        //! Database reader messages
        //! \remark used with splash screen
        void databaseReaderMessages(const BlackMisc::CStatusMessageList &messages);

        //! DB data read
        void swiftDbDataRead(bool success);

        // simplified signals follow
        // 1) simple signature
        // 2) fired directly after read, no need to wait for other entities

        //! \name Simplified read signals
        //! @{

        //! All swift DB data have been read
        void swiftDbAllDataRead();

        //! Shared info objects read
        void sharedInfoObjectsRead();

        //! All models read
        void swiftDbModelsRead();

        //! Aircraft ICAO data read
        void swiftDbAircraftIcaoRead();

        //! Airline ICAO data read
        void swiftDbAirlineIcaoRead();

        //! Airports read
        void swiftDbAirportsRead();

        //! All ICAO entities
        void swiftDbAllIcaoEntitiesRead();

        //! All entities needed for model matching
        void swiftDbModelMatchingEntitiesRead();
        //! @}

    public slots:
        //! Call CWebDataServices::readInBackground by single shot
        void readDeferredInBackground(BlackMisc::Network::CEntityFlags::Entity entities, int delayMs);

        //! First read (allows to immediately read in background)
        //! \remark ensures info objects (if and only if needed) are read upfront
        void readInBackground(BlackMisc::Network::CEntityFlags::Entity entities = BlackMisc::Network::CEntityFlags::AllEntities);

    private:
        //! Received METAR data
        void receivedMetars(const BlackMisc::Weather::CMetarList &metars);

        //! VATSIM data file has been read
        void vatsimDataFileRead(int kB);

        //! VATSIM status file has been read
        void vatsimStatusFileRead(int lines);

        //! VATSIM server file has been read
        void vatsimServerFileRead(int bytes);

        //! Initialize and start VATSIM server file reader
        void startVatsimServerFileReader();

        //! Read finished from reader
        void readFromSwiftReader(BlackMisc::Network::CEntityFlags::Entity entities, BlackMisc::Network::CEntityFlags::ReadState state, int number, const QUrl &url);

        //! Init the readers
        void initReaders(CWebReaderFlags::WebReader readersNeeded, BlackMisc::Network::CEntityFlags::Entity entities);

        //! Init the info objects reader (DB web service)
        void initDbInfoObjectReaderAndTriggerRead();

        //! Init the info objects reader (shared dbinfo.json)
        void initSharedInfoObjectReaderAndTriggerRead();

        //! DB reader for given entity
        Db::CDatabaseReader *getDbReader(BlackMisc::Network::CEntityFlags::Entity entity) const;

        //! Init the writers
        void initWriters();

        //! Remember this entity/those enties already have been signaled
        bool signalEntitiesAlreadyRead(BlackMisc::Network::CEntityFlags::Entity entities);

        //! Info object count from shared/DB info objects
        int getInfoObjectCount(BlackMisc::Network::CEntityFlags::Entity entity, BlackCore::Db::CInfoDataReader *reader) const;

        //! Entities from DB with cached data (i.e. count > 0)
        //! \remark requires the caches already read
        BlackMisc::Network::CEntityFlags::Entity getDbEntitiesWithCachedData() const;

        //! Entities from DB with cache timestamp newer than given threshold
        //! \remark unlike getDbEntitiesWithCachedData() this does not need the caches already being read
        BlackMisc::Network::CEntityFlags::Entity getDbEntitiesWithTimestampNewerThan(const QDateTime &threshold) const;

        //! Wait for DB info objects to be read
        bool waitForDbInfoObjectsThenRead(BlackMisc::Network::CEntityFlags::Entity entities);

        //! Wait for shared info objects to be read
        //! \return true means info objects available
        bool waitForSharedInfoObjectsThenRead(BlackMisc::Network::CEntityFlags::Entity entities);

        //! Wait for info objects to be read
        //! \return true means info objects available
        bool waitForInfoObjectsThenRead(BlackMisc::Network::CEntityFlags::Entity entities, const QString &info, BlackCore::Db::CInfoDataReader *infoReader, QDateTime &timeOut);

        BlackMisc::Network::CEntityFlags::Entity m_entitiesPeriodicallyRead = BlackMisc::Network::CEntityFlags::NoEntity; //!< entities permanently updated by timers
        BlackMisc::Network::CEntityFlags::Entity m_swiftDbEntitiesRead = BlackMisc::Network::CEntityFlags::NoEntity; //!< entities read
        BlackCore::Db::CDatabaseReaderConfigList m_dbReaderConfig; //!< how to read DB data
        std::atomic_bool m_shuttingDown { false }; //!< shutting down?
        QDateTime m_dbInfoObjectTimeout; //!< started reading DB info objects
        QDateTime m_sharedInfoObjectsTimeout; //!< started reading shared info objects
        QSet<BlackMisc::Network::CEntityFlags::Entity> m_signalledEntities; //!< remember signalled entites

        // for reading XML and VATSIM data files
        Vatsim::CVatsimStatusFileReader *m_vatsimStatusReader = nullptr;
        Vatsim::CVatsimDataFileReader *m_vatsimDataFileReader = nullptr;
        Vatsim::CVatsimMetarReader *m_vatsimMetarReader = nullptr;
        Vatsim::CVatsimServerFileReader *m_vatsimServerFileReader = nullptr;
        Db::CIcaoDataReader *m_icaoDataReader = nullptr;
        Db::CModelDataReader *m_modelDataReader = nullptr;
        Db::CAirportDataReader *m_airportDataReader = nullptr;
        Db::CInfoDataReader *m_dbInfoDataReader = nullptr;
        Db::CInfoDataReader *m_sharedInfoDataReader = nullptr;

        // writing objects directly into DB
        Db::CDatabaseWriter *m_databaseWriter = nullptr;
    };
} // namespace

#endif
