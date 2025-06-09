// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_WEBDATASERVICES_H
#define SWIFT_CORE_WEBDATASERVICES_H

#include <atomic>

#include <QDateTime>
#include <QList>
#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>

#include "core/db/databasereader.h"
#include "core/swiftcoreexport.h"
#include "core/webreaderflags.h"
#include "misc/aviation/aircrafticaocodelist.h"
#include "misc/aviation/airlineicaocodelist.h"
#include "misc/aviation/airporticaocode.h"
#include "misc/aviation/airportlist.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/liverylist.h"
#include "misc/countrylist.h"
#include "misc/network/entityflags.h"
#include "misc/network/serverlist.h"
#include "misc/network/url.h"
#include "misc/network/userlist.h"
#include "misc/network/voicecapabilities.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/distributor.h"
#include "misc/simulation/distributorlist.h"
#include "misc/statusmessagelist.h"
#include "misc/weather/metar.h"
#include "misc/weather/metarlist.h"

namespace swift::misc
{
    class CLogCategoryList;

    namespace aviation
    {
        class CCallsign;
    }
    namespace simulation
    {
        class CSimulatedAircraft;
        class CAutoPublishData;
    } // namespace simulation
} // namespace swift::misc

namespace swift::core
{
    namespace vatsim
    {
        class CVatsimDataFileReader;
        class CVatsimMetarReader;
        class CVatsimStatusFileReader;
        class CVatsimServerFileReader;
    } // namespace vatsim

    namespace db
    {
        class CAirportDataReader;
        class CDatabaseWriter;
        class CDatabaseReader;
        class CIcaoDataReader;
        class CModelDataReader;
        class CInfoDataReader;
    } // namespace db

    /*!
     * Encapsulates reading data from web sources
     */
    class SWIFT_CORE_EXPORT CWebDataServices : public QObject
    {
        Q_OBJECT

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor, only allowed from swift::core::CApplication
        CWebDataServices(CWebReaderFlags::WebReader readerFlags,
                         const swift::core::db::CDatabaseReaderConfigList &dbReaderConfig, QObject *parent = nullptr);

        //! Destructor
        virtual ~CWebDataServices() override;

        //! Shutdown
        void gracefulShutdown();

        //! Data file reader
        vatsim::CVatsimDataFileReader *getVatsimDataFileReader() const { return m_vatsimDataFileReader; }

        //! Metar reader
        vatsim::CVatsimMetarReader *getMetarReader() const { return m_vatsimMetarReader; }

        //! DB info data reader
        swift::core::db::CInfoDataReader *getDbInfoDataReader() const { return m_dbInfoDataReader; }

        //! Shared info data reader
        swift::core::db::CInfoDataReader *getSharedInfoDataReader() const { return m_sharedInfoDataReader; }

        //! DB writer class
        db::CDatabaseWriter *getDatabaseWriter() const { return m_databaseWriter; }

        //! All DB entities for those readers used and not ignored
        swift::misc::network::CEntityFlags::Entity allDbEntitiesForUsedReaders() const;

        //! FSD servers
        //! \threadsafe
        swift::misc::network::CServerList getVatsimFsdServers() const;

        //! METAR URL (from status file)
        //! \threadsafe
        swift::misc::network::CUrl getVatsimMetarUrl() const;

        //! Data file location (from status file)
        //! \threadsafe
        swift::misc::network::CUrl getVatsimDataFileUrl() const;

        //! Users by callsign
        //! \threadsafe
        swift::misc::network::CUserList getUsersForCallsign(const swift::misc::aviation::CCallsign &callsign) const;

        //! ATC stations by callsign
        //! \threadsafe
        swift::misc::aviation::CAtcStationList
        getAtcStationsForCallsign(const swift::misc::aviation::CCallsign &callsign) const;

        //! Voice capabilities for given callsign
        //! \threadsafe
        swift::misc::network::CVoiceCapabilities
        getVoiceCapabilityForCallsign(const swift::misc::aviation::CCallsign &callsign) const;

        //! Update with web data
        //! \threadsafe
        void updateWithVatsimDataFileData(swift::misc::simulation::CSimulatedAircraft &aircraftToBeUdpated) const;

        //! Distributors
        //! \threadsafe
        swift::misc::simulation::CDistributorList getDistributors() const;

        //! Distributors count
        //! \threadsafe
        int getDistributorsCount() const;

        //! Distributor for key
        //! \threadsafe
        swift::misc::simulation::CDistributor getDistributorForDbKey(const QString &key) const;

        //! Use distributor object to select the best complete distributor from DB
        //! \threadsafe
        swift::misc::simulation::CDistributor
        smartDistributorSelector(const swift::misc::simulation::CDistributor &distributor) const;

        //! Use distributor object to select the best complete distributor from DB
        //! \threadsafe
        swift::misc::simulation::CDistributor smartDistributorSelector() const;

        //! Best match specified by distributor / model
        //! \threadsafe
        swift::misc::simulation::CDistributor
        smartDistributorSelector(const swift::misc::simulation::CDistributor &distributor,
                                 const swift::misc::simulation::CAircraftModel &model) const;

        //! Liveries
        //! \threadsafe
        swift::misc::aviation::CLiveryList getLiveries() const;

        //! Liveries count
        //! \threadsafe
        int getLiveriesCount() const;

        //! Livery for its combined code
        //! \threadsafe
        swift::misc::aviation::CLivery getLiveryForCombinedCode(const QString &combinedCode) const;

        //! The temp. livery if available
        //! \threadsafe
        swift::misc::aviation::CLivery getTempLiveryOrDefault() const;

        //! Standard livery for airline code
        //! \threadsafe
        swift::misc::aviation::CLivery
        getStdLiveryForAirlineCode(const swift::misc::aviation::CAirlineIcaoCode &icao) const;

        //! Livery for id
        //! \threadsafe
        swift::misc::aviation::CLivery getLiveryForDbKey(int id) const;

        //! Use a livery as template and select the best complete livery from DB for it
        //! \threadsafe
        swift::misc::aviation::CLivery smartLiverySelector(const swift::misc::aviation::CLivery &livery) const;

        //! Models
        //! \threadsafe
        swift::misc::simulation::CAircraftModelList getModels() const;

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
        QStringList getModelCompleterStrings(bool sorted = true,
                                             const swift::misc::simulation::CSimulatorInfo &simulator = {
                                                 swift::misc::simulation::CSimulatorInfo::All }) const;

        //! Models for combined code and aircraft designator
        //! \threadsafe
        swift::misc::simulation::CAircraftModelList
        getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator,
                                                            const QString &combinedCode) const;

        //! Model for model string if any
        //! \threadsafe
        swift::misc::simulation::CAircraftModel getModelForModelString(const QString &modelString) const;

        //! Existing modelstring?
        //! \threadsafe
        bool containsModelString(const QString &modelString) const;

        //! Model for key if any
        //! \threadsafe
        swift::misc::simulation::CAircraftModel getModelForDbKey(int dbKey) const;

        //! Aircraft ICAO codes
        //! \threadsafe
        swift::misc::aviation::CAircraftIcaoCodeList getAircraftIcaoCodes() const;

        //! Aircraft ICAO codes count
        //! \threadsafe
        int getAircraftIcaoCodesCount() const;

        //! ICAO code for designator
        //! \threadsafe
        swift::misc::aviation::CAircraftIcaoCode getAircraftIcaoCodeForDesignator(const QString &designator) const;

        //! ICAO code for designator count
        //! \threadsafe
        int getAircraftIcaoCodesForDesignatorCount(const QString &designator) const;

        //! Aircraft ICAO designators for airline
        //! \threadsafe
        QSet<QString> getAircraftDesignatorsForAirline(const swift::misc::aviation::CAirlineIcaoCode &airline) const;

        //! Aircraft ICAO codes for airline
        //! \threadsafe
        swift::misc::aviation::CAircraftIcaoCodeList
        getAircraftIcaoCodesForAirline(const swift::misc::aviation::CAirlineIcaoCode &airline) const;

        //! Aircraft categories
        //! \threadsafe
        swift::misc::aviation::CAircraftCategoryList getAircraftCategories() const;

        //! Aircraft categories count
        //! \threadsafe
        int getAircraftCategoriesCount() const;

        //! Contains the given designator?
        //! \threadsafe
        bool containsAircraftIcaoDesignator(const QString &designator) const;

        //! ICAO codes for designator
        //! \threadsafe
        swift::misc::aviation::CAircraftIcaoCodeList getAircraftIcaoCodesForDesignator(const QString &designator) const;

        //! ICAO code for id
        //! \threadsafe
        swift::misc::aviation::CAircraftIcaoCode getAircraftIcaoCodeForDbKey(int id) const;

        //! Use an ICAO object to select the best complete ICAO object from DB for it
        //! \threadsafe
        swift::misc::aviation::CAircraftIcaoCode
        smartAircraftIcaoSelector(const swift::misc::aviation::CAircraftIcaoCode &icao) const;

        //! Airline ICAO codes
        //! \threadsafe
        swift::misc::aviation::CAirlineIcaoCodeList getAirlineIcaoCodes() const;

        //! Contains the given designator?
        //! \threadsafe
        bool containsAirlineIcaoDesignator(const QString &designator) const;

        //! ICAO code if unique, otherwise default
        //! \threadsafe
        swift::misc::aviation::CAirlineIcaoCode
        getAirlineIcaoCodeForUniqueDesignatorOrDefault(const QString &designator, bool preferOperatingAirlines) const;

        //! ICAO code if unique, otherwise default
        //! \threadsafe
        swift::misc::aviation::CAirlineIcaoCode
        getAirlineIcaoCodeForUniqueIataCodeOrDefault(const QString &iataCode) const;

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
        swift::misc::aviation::CAirlineIcaoCodeList getAirlineIcaoCodesForDesignator(const QString &designator) const;

        //! Airline ICAO codes for designator count
        //! \threadsafe
        int getAirlineIcaoCodesForDesignatorCount(const QString &designator) const;

        //! ICAO code for id
        //! \threadsafe
        swift::misc::aviation::CAirlineIcaoCode getAirlineIcaoCodeForDbKey(int id) const;

        //! Smart airline selector
        //! \threadsafe
        swift::misc::aviation::CAirlineIcaoCode smartAirlineIcaoSelector(
            const swift::misc::aviation::CAirlineIcaoCode &code,
            const swift::misc::aviation::CCallsign &callsign = swift::misc::aviation::CCallsign()) const;

        //! ICAO code for callsign (e.g. DLH123 -> DLH)
        //! \threadsafe
        swift::misc::aviation::CAirlineIcaoCode
        findBestMatchByCallsign(const swift::misc::aviation::CCallsign &callsign) const;

        //! Countries
        //! \threadsafe
        swift::misc::CCountryList getCountries() const;

        //! Country count
        //! \threadsafe
        int getCountriesCount() const;

        //! Country by ISO code (GB, US...)
        //! \threadsafe
        swift::misc::CCountry getCountryForIsoCode(const QString &iso) const;

        //! Country by name (France, China ..)
        //! \threadsafe
        swift::misc::CCountry getCountryForName(const QString &name) const;

        //! Get airports
        //! \threadsafe
        swift::misc::aviation::CAirportList getAirports() const;

        //! Get airports count
        //! \threadsafe
        int getAirportsCount() const;

        //! Get airport for ICAO designator
        //! \threadsafe
        swift::misc::aviation::CAirport getAirportForIcaoDesignator(const QString &icao) const;

        //! Get airport for name of location
        //! \threadsafe
        swift::misc::aviation::CAirport getAirportForNameOrLocation(const QString &nameOrLocation) const;

        //! Get METARs
        //! \threadsafe
        swift::misc::weather::CMetarList getMetars() const;

        //! Get METAR for airport
        //! \threadsafe
        swift::misc::weather::CMetar getMetarForAirport(const swift::misc::aviation::CAirportIcaoCode &icao) const;

        //! Validate for publishing
        //! \remark More detailed check than swift::misc::simulation::CAircraftModelList::validateForPublishing
        swift::misc::CStatusMessageList
        validateForPublishing(const swift::misc::simulation::CAircraftModelList &modelsToBePublished, bool ignoreEqual,
                              swift::misc::simulation::CAircraftModelList &validModels,
                              swift::misc::simulation::CAircraftModelList &invalidModels) const;

        //! \copydoc swift::misc::simulation::CAircraftModel::isEqualForPublishing
        bool isDbModelEqualForPublishing(const swift::misc::simulation::CAircraftModel &modelToBeChecked,
                                         swift::misc::CStatusMessageList *details = nullptr) const;

        //! Publish models to database
        swift::misc::CStatusMessageList
        asyncPublishModels(const swift::misc::simulation::CAircraftModelList &modelsToBePublished) const;

        //! Auto publish to database
        swift::misc::CStatusMessageList asyncAutoPublish(const swift::misc::simulation::CAutoPublishData &data) const;

        //! Trigger read of DB info objects
        void triggerReadOfDbInfoObjects();

        //! Trigger read of shared info objects
        void triggerReadOfSharedInfoObjects();

        //! Trigger read of new data
        //! \note requires info objects loaded upfront and uses the full cache logic
        swift::misc::network::CEntityFlags::Entity triggerRead(swift::misc::network::CEntityFlags::Entity whatToRead,
                                                               const QDateTime &newerThan = QDateTime());

        //! Trigger reload from DB, loads the DB data and bypasses the caches checks and info objects
        swift::misc::network::CEntityFlags::Entity
        triggerLoadingDirectlyFromDb(swift::misc::network::CEntityFlags::Entity whatToRead,
                                     const QDateTime &newerThan = QDateTime());

        //! Trigger reload from shared files, loads the data and bypasses caches
        swift::misc::network::CEntityFlags::Entity
        triggerLoadingDirectlyFromSharedFiles(swift::misc::network::CEntityFlags::Entity whatToRead,
                                              bool checkCacheTsUpfront);

        //! Corresponding cache timestamp if applicable
        //! \threadsafe
        QDateTime getCacheTimestamp(swift::misc::network::CEntityFlags::Entity entity) const;

        //! Latest DB object timestamp, or null if there is no such timestamp
        //! \threadsafe
        QDateTime getLatestDbEntityCacheTimestamp() const;

        //! Corresponding DB timestamp if applicable
        //! \remark from Db::CInfoDataReader
        //! \threadsafe
        QDateTime getLatestDbEntityTimestamp(swift::misc::network::CEntityFlags::Entity entity) const;

        //! Shared info object timestamp
        //! \remark from Db::CInfoDataReader
        //! \threadsafe
        QDateTime getLatestSharedInfoObjectTimestamp(swift::misc::network::CEntityFlags::Entity entity) const;

        //! Entities with newer shared file (from DB Info object)
        //! \threadsafe
        swift::misc::network::CEntityFlags::Entity
        getEntitiesWithNewerSharedFile(swift::misc::network::CEntityFlags::Entity entities) const;

        //! Empty entities in cache
        //! \remark can also be empty because cache is not yet synchronized
        //! \threadsafe
        swift::misc::network::CEntityFlags::Entity
        getEmptyEntities(swift::misc::network::CEntityFlags::Entity entities =
                             swift::misc::network::CEntityFlags::AllDbEntities) const;

        //! Synchronized entities either empty or with newer shared file
        //! \remark will synchronize entities
        //! \threadsafe
        swift::misc::network::CEntityFlags::Entity getSynchronizedEntitiesWithNewerSharedFileOrEmpty(
            bool syncData = true,
            swift::misc::network::CEntityFlags::Entity entities = swift::misc::network::CEntityFlags::AllDbEntities);

        //! Cache count for entity
        //! \threadsafe
        int getCacheCount(swift::misc::network::CEntityFlags::Entity entity) const;

        //! Count for entity from DB entity objects
        //! \threadsafe
        int getDbInfoObjectCount(swift::misc::network::CEntityFlags::Entity entity) const;

        //! Count for 1-n entities from DB entity objects
        //! \threadsafe
        int getDbInfoObjectsCount(swift::misc::network::CEntityFlags::Entity entities,
                                  bool stopIfNotFound = true) const;

        //! Count for entity from shared entity objects
        //! \threadsafe
        int getSharedInfoObjectCount(swift::misc::network::CEntityFlags::Entity entity) const;

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
        void admitDbCaches(swift::misc::network::CEntityFlags::Entity entities);

        //! Synchronize all DB caches specified
        //! \remark can be slow if all data are forced to be loaded
        void synchronizeDbCaches(swift::misc::network::CEntityFlags::Entity entities);

        //! Write data to disk (mainly for testing scenarios)
        bool writeDbDataToDisk(const QString &dir);

        //! Load DB data from disk (mainly for initial data load and testing scenarios)
        //! \remark if the DB readers are alred in aother thread reads in background
        //! \sa CWebDataServices::initDbCachesFromLocalResourceFiles for reading from local resource file
        bool readDbDataFromDisk(const QString &dir, bool inBackground, bool overrideNewerOnly);

        //! Init caches from local DB files
        //! \remark the shared files coming with the installer
        swift::misc::CStatusMessageList initDbCachesFromLocalResourceFiles(bool inBackground);

        //! Init caches from local DB files per given entities
        //! \remark the shared files coming with the installer
        swift::misc::CStatusMessageList
        initDbCachesFromLocalResourceFiles(swift::misc::network::CEntityFlags::Entity entities, bool inBackground);

    signals:
        //! Combined read signal
        void dataRead(swift::misc::network::CEntityFlags::Entity entity,
                      swift::misc::network::CEntityFlags::ReadState state, int number, const QUrl &url);

        //! Download progress for an entity
        void entityDownloadProgress(swift::misc::network::CEntityFlags::Entity entity, int logId, int progress,
                                    qint64 current, qint64 max, const QUrl &url);

        //! Database reader messages
        //! \remark used with splash screen
        void databaseReaderMessages(const swift::misc::CStatusMessageList &messages);

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
        void readDeferredInBackground(swift::misc::network::CEntityFlags::Entity entities, int delayMs);

        //! First read (allows to immediately read in background)
        //! \remark ensures info objects (if and only if needed) are read upfront
        void readInBackground(
            swift::misc::network::CEntityFlags::Entity entities = swift::misc::network::CEntityFlags::AllEntities);

    private:
        //! Received METAR data
        void receivedMetars(const swift::misc::weather::CMetarList &metars);

        //! VATSIM data file has been read
        void vatsimDataFileRead(int kB);

        //! VATSIM status file has been read
        void vatsimStatusFileRead(int bytes);

        //! VATSIM server file has been read
        void vatsimServerFileRead(int bytes);

        //! Initialize and start VATSIM status file reader
        void startVatsimStatusFileReader();

        //! Initialize and start VATSIM server file reader
        void startVatsimServerFileReader();

        //! Initialize and start VATSIM data file reader
        void startVatsimDataFileReader();

        //! Initialize and start VATSIM data file reader
        void startVatsimMetarFileReader();

        //! Initialize and start ICAO data reader
        void startIcaoDataReader();

        //! Initialize and start model data reader
        void startModelDataReader();

        //! Initialize and start airport data reader
        void startAirportDataReader();

        //! Read finished from reader
        void readFromSwiftReader(swift::misc::network::CEntityFlags::Entity entities,
                                 swift::misc::network::CEntityFlags::ReadState state, int number, const QUrl &url);

        //! Init the readers
        void initReaders(CWebReaderFlags::WebReader readersNeeded, swift::misc::network::CEntityFlags::Entity entities);

        //! Init the info objects reader (DB web service)
        void initDbInfoObjectReaderAndTriggerRead();

        //! Init the info objects reader (shared dbinfo.json)
        void initSharedInfoObjectReaderAndTriggerRead();

        //! DB reader for given entity
        db::CDatabaseReader *getDbReader(swift::misc::network::CEntityFlags::Entity entity) const;

        //! Init the writers
        void initWriters();

        //! Remember this entity/those enties already have been signaled
        bool signalEntitiesAlreadyRead(swift::misc::network::CEntityFlags::Entity entities);

        //! Info object count from shared/DB info objects
        int getInfoObjectCount(swift::misc::network::CEntityFlags::Entity entity,
                               swift::core::db::CInfoDataReader *reader) const;

        //! Entities from DB with cached data (i.e. count > 0)
        //! \remark requires the caches already read
        swift::misc::network::CEntityFlags::Entity getDbEntitiesWithCachedData() const;

        //! Entities from DB with cache timestamp newer than given threshold
        //! \remark unlike getDbEntitiesWithCachedData() this does not need the caches already being read
        swift::misc::network::CEntityFlags::Entity
        getDbEntitiesWithTimestampNewerThan(const QDateTime &threshold) const;

        //! Wait for DB info objects to be read
        bool waitForDbInfoObjectsThenRead(swift::misc::network::CEntityFlags::Entity entities);

        //! Wait for shared info objects to be read
        //! \return true means info objects available
        bool waitForSharedInfoObjectsThenRead(swift::misc::network::CEntityFlags::Entity entities);

        //! Wait for info objects to be read
        //! \return true means info objects available
        bool waitForInfoObjectsThenRead(swift::misc::network::CEntityFlags::Entity entities, const QString &info,
                                        swift::core::db::CInfoDataReader *infoReader, QDateTime &timeOut);

        swift::misc::network::CEntityFlags::Entity m_entitiesPeriodicallyRead =
            swift::misc::network::CEntityFlags::NoEntity; //!< entities permanently updated by timers
        swift::misc::network::CEntityFlags::Entity m_swiftDbEntitiesRead =
            swift::misc::network::CEntityFlags::NoEntity; //!< entities read
        swift::core::db::CDatabaseReaderConfigList m_dbReaderConfig; //!< how to read DB data
        std::atomic_bool m_shuttingDown { false }; //!< shutting down?
        QDateTime m_dbInfoObjectTimeout; //!< started reading DB info objects
        QDateTime m_sharedInfoObjectsTimeout; //!< started reading shared info objects
        QSet<swift::misc::network::CEntityFlags::Entity> m_signalledEntities; //!< remember signalled entites

        // for reading XML and VATSIM data files
        vatsim::CVatsimStatusFileReader *m_vatsimStatusReader = nullptr;
        vatsim::CVatsimDataFileReader *m_vatsimDataFileReader = nullptr;
        vatsim::CVatsimMetarReader *m_vatsimMetarReader = nullptr;
        vatsim::CVatsimServerFileReader *m_vatsimServerFileReader = nullptr;
        db::CIcaoDataReader *m_icaoDataReader = nullptr;
        db::CModelDataReader *m_modelDataReader = nullptr;
        db::CAirportDataReader *m_airportDataReader = nullptr;
        db::CInfoDataReader *m_dbInfoDataReader = nullptr;
        db::CInfoDataReader *m_sharedInfoDataReader = nullptr;

        // writing objects directly into DB
        db::CDatabaseWriter *m_databaseWriter = nullptr;
    };
} // namespace swift::core

#endif // SWIFT_CORE_WEBDATASERVICES_H
