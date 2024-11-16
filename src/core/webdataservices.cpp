// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/webdataservices.h"

#include <QDir>
#include <QJsonDocument>
#include <QPointer>
#include <QSslSocket>
#include <QThread>
#include <QTimer>
#include <QtGlobal>

#include "config/buildconfig.h"
#include "core/application.h"
#include "core/context/contextnetwork.h"
#include "core/data/globalsetup.h"
#include "core/db/airportdatareader.h"
#include "core/db/databasewriter.h"
#include "core/db/icaodatareader.h"
#include "core/db/infodatareader.h"
#include "core/db/modeldatareader.h"
#include "core/setupreader.h"
#include "core/vatsim/vatsimdatafilereader.h"
#include "core/vatsim/vatsimmetarreader.h"
#include "core/vatsim/vatsimserverfilereader.h"
#include "core/vatsim/vatsimstatusfilereader.h"
#include "misc/fileutils.h"
#include "misc/logcategories.h"
#include "misc/logmessage.h"
#include "misc/statusmessage.h"
#include "misc/threadutils.h"
#include "misc/worker.h"

using namespace swift::core;
using namespace swift::core::db;
using namespace swift::core::data;
using namespace swift::core::vatsim;
using namespace swift::core::context;
using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::db;
using namespace swift::misc::simulation;
using namespace swift::misc::network;
using namespace swift::misc::aviation;
using namespace swift::misc::weather;

namespace swift::core
{
    CWebDataServices::CWebDataServices(CWebReaderFlags::WebReader readers,
                                       const CDatabaseReaderConfigList &dbReaderConfig, QObject *parent)
        : QObject(parent), m_dbReaderConfig(dbReaderConfig)
    {
        if (!sApp) { return; } // shutting down

        Q_ASSERT_X(QSslSocket::supportsSsl(), Q_FUNC_INFO, "Missing SSL support");
        Q_ASSERT_X(sApp->isSetupAvailable(), Q_FUNC_INFO, "Setup not synchronized");
        this->setObjectName("CWebDataServices");

        // SSL INFOs
        CLogMessage(this).info(u"SSL supported: %1 Version: %2 (build version) %3 (library version)")
            << boolToYesNo(QSslSocket::supportsSsl()) << QSslSocket::sslLibraryBuildVersionString()
            << QSslSocket::sslLibraryVersionString();

        // check if I need info objects
        const bool readFromSwiftDb = dbReaderConfig.possiblyReadsFromSwiftDb(); // DB read access
        const bool writeToSwiftDb = dbReaderConfig.possiblyWritesToSwiftDb(); // DB write access
        if (!readFromSwiftDb && readers.testFlag(CWebReaderFlags::DbInfoDataReader))
        {
            // will remove info reader because not needed
            readers &= ~CWebReaderFlags::DbInfoDataReader;
            CLogMessage(this).info(u"Remove info object reader because not needed");
        }

        // get entities to be read
        CEntityFlags::Entity entities = CWebReaderFlags::allEntitiesForReaders(readers);
        if (entities.testFlag(CEntityFlags::DbInfoObjectEntity))
        {
            Q_ASSERT_X(readers.testFlag(CWebReaderFlags::DbInfoDataReader), Q_FUNC_INFO, "info object but no reader");
            CLogMessage(this).info(u"Using info objects for swift DB entities");
        }

        this->initReaders(readers, entities); // reads info objects if required
        if (writeToSwiftDb) { this->initWriters(); }

        // make sure this is called in event queue, so pending tasks cam be performed
        entities &= ~CEntityFlags::DbInfoObjectEntity; // triggered in init readers
        entities &= ~CEntityFlags::VatsimStatusFile; // triggered in init readers
        entities &= ~m_entitiesPeriodicallyRead; // will be triggered by timers

        // trigger reading
        // but do not start all at the same time
        const CEntityFlags::Entity icaoPart = entities & CEntityFlags::AllIcaoCountriesCategory;
        const CEntityFlags::Entity modelPart = entities & CEntityFlags::DistributorLiveryModel;
        CEntityFlags::Entity remainingEntities = entities & ~icaoPart;
        remainingEntities &= ~modelPart;
        this->readDeferredInBackground(icaoPart, 500);
        this->readDeferredInBackground(modelPart, 1000);
        this->readDeferredInBackground(remainingEntities, 1500);
    }

    CWebDataServices::~CWebDataServices() { this->gracefulShutdown(); }

    CServerList CWebDataServices::getVatsimFsdServers() const
    {
        if (m_vatsimServerFileReader) { return m_vatsimServerFileReader->getFsdServers(); }
        return CServerList();
    }

    CUrl CWebDataServices::getVatsimMetarUrl() const
    {
        if (m_vatsimStatusReader) { return m_vatsimStatusReader->getMetarFileUrl(); }
        return {};
    }

    CUrl CWebDataServices::getVatsimDataFileUrl() const
    {
        if (m_vatsimStatusReader) { return m_vatsimStatusReader->getDataFileUrl(); }
        return {};
    }

    CUserList CWebDataServices::getUsersForCallsign(const CCallsign &callsign) const
    {
        if (m_vatsimDataFileReader) { return m_vatsimDataFileReader->getUsersForCallsign(callsign); }
        return CUserList();
    }

    CAtcStationList CWebDataServices::getAtcStationsForCallsign(const CCallsign &callsign) const
    {
        if (m_vatsimDataFileReader) { return m_vatsimDataFileReader->getAtcStationsForCallsign(callsign); }
        return CAtcStationList();
    }

    CVoiceCapabilities CWebDataServices::getVoiceCapabilityForCallsign(const CCallsign &callsign) const
    {
        if (m_vatsimDataFileReader) { return m_vatsimDataFileReader->getVoiceCapabilityForCallsign(callsign); }
        return CVoiceCapabilities();
    }

    void CWebDataServices::updateWithVatsimDataFileData(CSimulatedAircraft &aircraftToBeUdpated) const
    {
        if (m_vatsimDataFileReader) { m_vatsimDataFileReader->updateWithVatsimDataFileData(aircraftToBeUdpated); }
    }

    CStatusMessageList CWebDataServices::asyncPublishModels(const CAircraftModelList &modelsToBePublished) const
    {
        if (m_databaseWriter) { return m_databaseWriter->asyncPublishModels(modelsToBePublished, ""); }
        return CStatusMessageList();
    }

    CStatusMessageList CWebDataServices::asyncAutoPublish(const CAutoPublishData &data) const
    {
        if (m_databaseWriter) { return m_databaseWriter->asyncAutoPublish(data); }
        return CStatusMessageList();
    }

    void CWebDataServices::triggerReadOfDbInfoObjects() { initDbInfoObjectReaderAndTriggerRead(); }

    void CWebDataServices::triggerReadOfSharedInfoObjects() { initSharedInfoObjectReaderAndTriggerRead(); }

    bool CWebDataServices::hasSuccesfullyConnectedSwiftDb() const
    {
        if (!m_icaoDataReader && !m_modelDataReader && !m_airportDataReader && !m_dbInfoDataReader) { return false; }

        // use the first one to test
        if (m_dbInfoDataReader) { return m_dbInfoDataReader->hasReceivedOkReply(); }
        if (m_icaoDataReader) { return m_icaoDataReader->hasReceivedOkReply(); }
        if (m_modelDataReader) { return m_modelDataReader->hasReceivedOkReply(); }
        if (m_airportDataReader) { return m_airportDataReader->hasReceivedOkReply(); }
        return false;
    }

    bool CWebDataServices::hasDbAircraftData() const { return this->hasDbIcaoData() && this->hasDbModelData(); }

    bool CWebDataServices::hasDbModelData() const
    {
        return (this->getModelsCount() > 0) && (this->getLiveriesCount() > 0) && (this->getDistributorsCount() > 0);
    }

    bool CWebDataServices::hasDbIcaoData() const
    {
        return (this->getAircraftIcaoCodesCount() > 0) && (this->getAirlineIcaoCodesCount() > 0) &&
               (this->getCountriesCount() > 0);
    }

    void CWebDataServices::admitDbCaches(CEntityFlags::Entity entities)
    {
        if (m_shuttingDown) { return; }

        // hint: those 2 are currently doing nothing, but this might change in the future
        // if (m_dbInfoDataReader)     { m_dbInfoDataReader->admitCaches(entities); }
        // if (m_sharedInfoDataReader) { m_sharedInfoDataReader->admitCaches(entities); }

        // hint: all the readers use own threads
        if (m_modelDataReader) { m_modelDataReader->admitCaches(entities); }
        if (m_icaoDataReader) { m_icaoDataReader->admitCaches(entities); }
        if (m_airportDataReader) { m_airportDataReader->admitCaches(entities); }
    }

    void CWebDataServices::synchronizeDbCaches(CEntityFlags::Entity entities)
    {
        if (m_shuttingDown) { return; }

        // hint: those 2 are currently doing nothing, but this might change in the future
        // if (m_dbInfoDataReader)     { m_dbInfoDataReader->synchronizeCaches(entities); }
        // if (m_sharedInfoDataReader) { m_sharedInfoDataReader->synchronizeCaches(entities); }

        // hint: all the readers use own threads
        if (m_modelDataReader) { m_modelDataReader->synchronizeCaches(entities); }
        if (m_icaoDataReader) { m_icaoDataReader->synchronizeCaches(entities); }
        if (m_airportDataReader) { m_airportDataReader->synchronizeCaches(entities); }
    }

    CEntityFlags::Entity CWebDataServices::triggerRead(CEntityFlags::Entity whatToRead, const QDateTime &newerThan)
    {
        if (m_shuttingDown) { return CEntityFlags::NoEntity; }

        Q_ASSERT_X(!whatToRead.testFlag(CEntityFlags::DbInfoObjectEntity), Q_FUNC_INFO,
                   "Info object must be read upfront");
        CEntityFlags::Entity triggeredRead = CEntityFlags::NoEntity;
        if (m_vatsimDataFileReader)
        {
            if (whatToRead.testFlag(CEntityFlags::VatsimDataFile))
            {
                m_vatsimDataFileReader->readInBackgroundThread();
                triggeredRead |= CEntityFlags::VatsimDataFile;
            }
        }

        if (m_vatsimMetarReader)
        {
            if (whatToRead.testFlag(CEntityFlags::MetarEntity))
            {
                m_vatsimMetarReader->readInBackgroundThread();
                triggeredRead |= CEntityFlags::MetarEntity;
            }
        }

        if (m_airportDataReader)
        {
            if (whatToRead.testFlag(CEntityFlags::AirportEntity))
            {
                CEntityFlags::Entity airportEntities = whatToRead & CEntityFlags::AirportEntity;
                m_airportDataReader->readInBackgroundThread(airportEntities, newerThan);
                triggeredRead |= CEntityFlags::AirportEntity;
            }
        }

        if (m_icaoDataReader)
        {
            if (whatToRead.testFlag(CEntityFlags::AircraftIcaoEntity) ||
                whatToRead.testFlag(CEntityFlags::AircraftCategoryEntity) ||
                whatToRead.testFlag(CEntityFlags::AirlineIcaoEntity) ||
                whatToRead.testFlag(CEntityFlags::CountryEntity))
            {
                CEntityFlags::Entity icaoEntities = whatToRead & CEntityFlags::AllIcaoCountriesCategory;
                m_icaoDataReader->readInBackgroundThread(icaoEntities, newerThan);
                triggeredRead |= icaoEntities;
            }
        }

        if (m_modelDataReader)
        {
            if (whatToRead.testFlag(CEntityFlags::LiveryEntity) ||
                whatToRead.testFlag(CEntityFlags::DistributorEntity) || whatToRead.testFlag(CEntityFlags::ModelEntity))
            {
                CEntityFlags::Entity modelEntities = whatToRead & CEntityFlags::DistributorLiveryModel;
                m_modelDataReader->readInBackgroundThread(modelEntities, newerThan);
                triggeredRead |= modelEntities;
            }
        }

        return triggeredRead;
    }

    CEntityFlags::Entity CWebDataServices::triggerLoadingDirectlyFromDb(CEntityFlags::Entity whatToRead,
                                                                        const QDateTime &newerThan)
    {
        if (m_shuttingDown) { return CEntityFlags::NoEntity; }
        if (!sApp || sApp->isShuttingDown()) { return CEntityFlags::NoEntity; }

        CEntityFlags::Entity triggeredRead = CEntityFlags::NoEntity;
        if (m_dbInfoDataReader)
        {
            this->triggerReadOfDbInfoObjects();
            triggeredRead |= CEntityFlags::DbInfoObjectEntity;
        }

        if (m_icaoDataReader)
        {
            if (m_icaoDataReader->supportsAnyOfEntities(whatToRead))
            {
                const CEntityFlags::Entity icaoEntities = m_icaoDataReader->maskBySupportedEntities(whatToRead);
                m_icaoDataReader->triggerLoadingDirectlyFromDb(icaoEntities, newerThan);
                triggeredRead |= icaoEntities;
            }
        }

        if (m_modelDataReader)
        {
            if (m_modelDataReader->supportsAnyOfEntities(whatToRead))
            {
                const CEntityFlags::Entity modelEntities = m_modelDataReader->maskBySupportedEntities(whatToRead);
                m_modelDataReader->triggerLoadingDirectlyFromDb(modelEntities, newerThan);
                triggeredRead |= modelEntities;
            }
        }

        if (m_airportDataReader)
        {
            if (m_airportDataReader->supportsAnyOfEntities(whatToRead))
            {
                const CEntityFlags::Entity airportEntities = m_airportDataReader->maskBySupportedEntities(whatToRead);
                m_airportDataReader->triggerLoadingDirectlyFromDb(airportEntities, newerThan);
                triggeredRead |= airportEntities;
            }
        }

        return triggeredRead;
    }

    CEntityFlags::Entity CWebDataServices::triggerLoadingDirectlyFromSharedFiles(CEntityFlags::Entity whatToRead,
                                                                                 bool checkCacheTsUpfront)
    {
        if (m_shuttingDown) { return CEntityFlags::NoEntity; }
        if (!sApp || sApp->isShuttingDown()) { return CEntityFlags::NoEntity; }

        CEntityFlags::Entity triggeredRead = CEntityFlags::NoEntity;
        this->triggerReadOfSharedInfoObjects(); // trigger reload of info objects (for shared)

        if (m_icaoDataReader)
        {
            if (m_icaoDataReader->supportsAnyOfEntities(whatToRead))
            {
                CEntityFlags::Entity icaoEntities = m_icaoDataReader->maskBySupportedEntities(whatToRead);
                triggeredRead |=
                    m_icaoDataReader->triggerLoadingDirectlyFromSharedFiles(icaoEntities, checkCacheTsUpfront);
            }
        }

        if (m_modelDataReader)
        {
            if (m_modelDataReader->supportsAnyOfEntities(whatToRead))
            {
                CEntityFlags::Entity modelEntities = m_modelDataReader->maskBySupportedEntities(whatToRead);
                triggeredRead |=
                    m_modelDataReader->triggerLoadingDirectlyFromSharedFiles(modelEntities, checkCacheTsUpfront);
            }
        }

        if (m_airportDataReader)
        {
            if (m_airportDataReader->supportsAnyOfEntities(whatToRead))
            {
                CEntityFlags::Entity airportEntities = m_airportDataReader->maskBySupportedEntities(whatToRead);
                triggeredRead |=
                    m_airportDataReader->triggerLoadingDirectlyFromSharedFiles(airportEntities, checkCacheTsUpfront);
            }
        }

        return triggeredRead;
    }

    QDateTime CWebDataServices::getCacheTimestamp(CEntityFlags::Entity entity) const
    {
        Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "Need single entity");
        if (!CEntityFlags::anySwiftDbEntity(entity)) { return QDateTime(); }
        const CDatabaseReader *dr = this->getDbReader(entity);
        if (!dr) { return QDateTime(); }
        return dr->getCacheTimestamp(entity);
    }

    QDateTime CWebDataServices::getLatestDbEntityTimestamp(CEntityFlags::Entity entity) const
    {
        Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "Need single entity");
        if (!CEntityFlags::anySwiftDbEntity(entity)) { return QDateTime(); }
        const CInfoDataReader *ir = this->getDbInfoDataReader();
        if (!ir) { return QDateTime(); }
        return ir->getLatestEntityTimestampFromDbInfoObjects(entity);
    }

    QDateTime CWebDataServices::getLatestSharedInfoObjectTimestamp(CEntityFlags::Entity entity) const
    {
        const CDatabaseReader *reader = this->getDbReader(entity);
        if (!reader) { return QDateTime(); }
        Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "need single entity");
        return reader->getLatestEntityTimestampFromSharedInfoObjects(entity);
    }

    QDateTime CWebDataServices::getLatestDbEntityCacheTimestamp() const
    {
        QDateTime latest;
        const CEntityFlags::EntitySet set = CEntityFlags::asSingleEntities(CEntityFlags::AllDbEntitiesNoInfoObjects);
        for (CEntityFlags::Entity e : set)
        {
            const QDateTime ts = this->getCacheTimestamp(e);
            if (!ts.isValid()) { continue; }
            if (!latest.isValid() || latest < ts) { latest = ts; }
        }
        return latest;
    }

    CEntityFlags::Entity CWebDataServices::getEntitiesWithNewerSharedFile(CEntityFlags::Entity entities) const
    {
        Q_ASSERT_X(m_sharedInfoDataReader, Q_FUNC_INFO, "Shared info reader was not initialized");
        if (m_sharedInfoDataReader->getInfoObjectCount() < 1) { return CEntityFlags::NoEntity; }
        return m_sharedInfoDataReader->getEntitesWithNewerSharedInfoObject(entities);
    }

    CEntityFlags::Entity CWebDataServices::getEmptyEntities(CEntityFlags::Entity entities) const
    {
        entities &= CEntityFlags::AllDbEntities; // handled by this reader
        CEntityFlags::Entity currentEntity = CEntityFlags::iterateDbEntities(entities);
        CEntityFlags::Entity emptyEntities = CEntityFlags::NoEntity;
        while (currentEntity != CEntityFlags::NoEntity)
        {
            const int c = this->getCacheCount(currentEntity);
            if (c < 1) { emptyEntities |= currentEntity; }
            currentEntity = CEntityFlags::iterateDbEntities(entities);
        }
        return emptyEntities;
    }

    CEntityFlags::Entity
    CWebDataServices::getSynchronizedEntitiesWithNewerSharedFileOrEmpty(bool syncData, CEntityFlags::Entity entities)
    {
        CEntityFlags::Entity loadEntities = this->getEntitiesWithNewerSharedFile(entities);
        const CEntityFlags::Entity checkForEmptyEntities =
            CEntityFlags::entityFlagToEntity(CEntityFlags::AllDbEntitiesNoInfoObjects) & ~loadEntities;

        // it can happen the timestamps are not newer, but the data are empty
        // - can happen if caches are copied and the TS does not represent the DB timestamp
        // - cache files have been deleted
        // - sync all DB entities
        //   - fast if there are no data
        //   - no impact if already synced
        //   - slow if newer synced before and all has to be done now
        if (syncData) { this->synchronizeDbCaches(checkForEmptyEntities); }

        // we have no newer timestamps, but incomplete data
        loadEntities |= this->getEmptyEntities();
        return loadEntities;
    }

    int CWebDataServices::getCacheCount(CEntityFlags::Entity entity) const
    {
        Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "Need single entity");
        if (CEntityFlags::anySwiftDbEntity(entity))
        {
            const CDatabaseReader *dr = this->getDbReader(entity);
            if (!dr) { return -1; }
            return dr->getCacheCount(entity);
        }
        else
        {
            // non DB/shared entities would go here
            return -1;
        }
    }

    int CWebDataServices::getDbInfoObjectCount(CEntityFlags::Entity entity) const
    {
        if (!m_dbInfoDataReader) { return -1; }
        return this->getInfoObjectCount(entity, m_dbInfoDataReader);
    }

    int CWebDataServices::getDbInfoObjectsCount(CEntityFlags::Entity entities, bool stopIfNotFound) const
    {
        if (!m_dbInfoDataReader) { return -1; }
        int total = 0;
        CEntityFlags::EntitySet set = CEntityFlags::asSingleEntities(entities);
        for (CEntityFlags::Entity single : set)
        {
            const int c = this->getDbInfoObjectCount(single);
            if (c < 0 && stopIfNotFound) { return -1; }
            if (c > 0) { total += c; }
        }
        return total;
    }

    int CWebDataServices::getSharedInfoObjectCount(CEntityFlags::Entity entity) const
    {
        if (!m_sharedInfoDataReader) return -1;
        return this->getInfoObjectCount(entity, m_sharedInfoDataReader);
    }

    QString CWebDataServices::getDbReadersLog(const QString &separator) const
    {
        QStringList report;
        if (m_dbInfoDataReader)
        {
            report << m_dbInfoDataReader->getName() + ": " + m_dbInfoDataReader->getReadLog().getSummary();
        }
        if (m_sharedInfoDataReader)
        {
            report << m_sharedInfoDataReader->getName() + ": " + m_sharedInfoDataReader->getReadLog().getSummary();
        }

        if (m_airportDataReader)
        {
            report << m_airportDataReader->getName() + ": " + m_airportDataReader->getReadLog().getSummary();
        }
        if (m_icaoDataReader)
        {
            report << m_icaoDataReader->getName() + ": " + m_icaoDataReader->getReadLog().getSummary();
        }
        if (m_modelDataReader)
        {
            report << m_modelDataReader->getName() + ": " + m_modelDataReader->getReadLog().getSummary();
        }
        if (m_databaseWriter)
        {
            report << m_databaseWriter->getName() + ": " + m_databaseWriter->getWriteLog().getSummary();
        }
        return report.join(separator);
    }

    QString CWebDataServices::getReadersLog(const QString &separator) const
    {
        const QString db = this->getDbReadersLog(separator);
        QStringList report;
        if (m_vatsimMetarReader)
        {
            report << m_vatsimMetarReader->getName() + ": " + m_vatsimMetarReader->getReadLog().getSummary();
        }
        if (m_vatsimStatusReader)
        {
            report << m_vatsimStatusReader->getName() + ": " + m_vatsimStatusReader->getReadLog().getSummary();
        }
        if (report.isEmpty()) { return db; }
        return report.join(separator) + separator + db;
    }

    CDistributorList CWebDataServices::getDistributors() const
    {
        if (m_modelDataReader) { return m_modelDataReader->getDistributors(); }
        return CDistributorList();
    }

    int CWebDataServices::getDistributorsCount() const
    {
        if (m_modelDataReader) { return m_modelDataReader->getDistributorsCount(); }
        return 0;
    }

    CDistributor CWebDataServices::getDistributorForDbKey(const QString &key) const
    {
        if (m_modelDataReader) { return m_modelDataReader->getDistributorForDbKey(key); }
        return CDistributor();
    }

    CDistributor CWebDataServices::smartDistributorSelector(const CDistributor &distributor) const
    {
        if (m_modelDataReader) { return m_modelDataReader->smartDistributorSelector(distributor); }
        return CDistributor();
    }

    CDistributor CWebDataServices::smartDistributorSelector(const CDistributor &distributor,
                                                            const CAircraftModel &model) const
    {
        if (m_modelDataReader) { return m_modelDataReader->smartDistributorSelector(distributor, model); }
        return CDistributor();
    }

    CLiveryList CWebDataServices::getLiveries() const
    {
        if (m_modelDataReader) { return m_modelDataReader->getLiveries(); }
        return CLiveryList();
    }

    int CWebDataServices::getLiveriesCount() const
    {
        if (m_modelDataReader) { return m_modelDataReader->getLiveriesCount(); }
        return 0;
    }

    CLivery CWebDataServices::getLiveryForCombinedCode(const QString &combinedCode) const
    {
        if (m_modelDataReader) { return m_modelDataReader->getLiveryForCombinedCode(combinedCode); }
        return CLivery();
    }

    CLivery CWebDataServices::getTempLiveryOrDefault() const
    {
        if (m_modelDataReader) { return m_modelDataReader->getLiveryForCombinedCode(CLivery::tempLiveryCode()); }
        return CLivery();
    }

    CLivery CWebDataServices::getStdLiveryForAirlineCode(const CAirlineIcaoCode &icao) const
    {
        if (m_modelDataReader) { return m_modelDataReader->getStdLiveryForAirlineVDesignator(icao); }
        return CLivery();
    }

    CLivery CWebDataServices::getLiveryForDbKey(int id) const
    {
        if (m_modelDataReader) { return m_modelDataReader->getLiveryForDbKey(id); }
        return CLivery();
    }

    CLivery CWebDataServices::smartLiverySelector(const CLivery &livery) const
    {
        if (m_modelDataReader) { return m_modelDataReader->smartLiverySelector(livery); }
        return livery;
    }

    CAircraftModelList CWebDataServices::getModels() const
    {
        if (m_modelDataReader) { return m_modelDataReader->getModels(); }
        return CAircraftModelList();
    }

    int CWebDataServices::getModelsCount() const
    {
        if (m_modelDataReader) { return m_modelDataReader->getModelsCount(); }
        return 0;
    }

    QSet<int> CWebDataServices::getModelDbKeys() const
    {
        if (m_modelDataReader) { return m_modelDataReader->getModelDbKeys(); }
        return QSet<int>();
    }

    QStringList CWebDataServices::getModelStrings(bool sort) const
    {
        if (m_modelDataReader) { return m_modelDataReader->getModelStringList(sort); }
        return QStringList();
    }

    QStringList CWebDataServices::getModelCompleterStrings(bool sorted, const CSimulatorInfo &simulator) const
    {
        if (m_modelDataReader) { return m_modelDataReader->getModels().toCompleterStrings(sorted, simulator); }
        return QStringList();
    }

    CAircraftModelList
    CWebDataServices::getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator,
                                                                          const QString &combinedCode) const
    {
        if (m_modelDataReader)
        {
            return m_modelDataReader->getModelsForAircraftDesignatorAndLiveryCombinedCode(aircraftDesignator,
                                                                                          combinedCode);
        }
        return CAircraftModelList();
    }

    CAircraftModel CWebDataServices::getModelForModelString(const QString &modelString) const
    {
        if (m_modelDataReader) { return m_modelDataReader->getModelForModelString(modelString); }
        return CAircraftModel();
    }

    bool CWebDataServices::containsModelString(const QString &modelString) const
    {
        if (m_modelDataReader) { return m_modelDataReader->containsModelString(modelString); }
        return false;
    }

    CAircraftModel CWebDataServices::getModelForDbKey(int dbKey) const
    {
        if (m_modelDataReader) { return m_modelDataReader->getModelForDbKey(dbKey); }
        return CAircraftModel();
    }

    CAircraftIcaoCodeList CWebDataServices::getAircraftIcaoCodes() const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAircraftIcaoCodes(); }
        return CAircraftIcaoCodeList();
    }

    int CWebDataServices::getAircraftIcaoCodesCount() const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAircraftIcaoCodesCount(); }
        return 0;
    }

    CAircraftIcaoCode CWebDataServices::getAircraftIcaoCodeForDesignator(const QString &designator) const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAircraftIcaoCodeForDesignator(designator); }
        return CAircraftIcaoCode();
    }

    int CWebDataServices::getAircraftIcaoCodesForDesignatorCount(const QString &designator) const
    {
        return this->getAircraftIcaoCodesForDesignator(designator).size();
    }

    QSet<QString> CWebDataServices::getAircraftDesignatorsForAirline(const CAirlineIcaoCode &airline) const
    {
        if (!airline.hasValidDesignator()) { return QSet<QString>(); }
        if (m_modelDataReader) { return m_modelDataReader->getAircraftDesignatorsForAirline(airline); }
        return QSet<QString>();
    }

    CAircraftIcaoCodeList CWebDataServices::getAircraftIcaoCodesForAirline(const CAirlineIcaoCode &airline) const
    {
        if (!airline.hasValidDesignator()) { return CAircraftIcaoCodeList(); }
        if (m_modelDataReader) { return m_modelDataReader->getAicraftIcaoCodesForAirline(airline); }
        return CAircraftIcaoCodeList();
    }

    CAircraftCategoryList CWebDataServices::getAircraftCategories() const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAircraftCategories(); }
        return CAircraftCategoryList();
    }

    int CWebDataServices::getAircraftCategoriesCount() const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAircraftCategoryCount(); }
        return 0;
    }

    bool CWebDataServices::containsAircraftIcaoDesignator(const QString &designator) const
    {
        if (designator.isEmpty()) { return false; }
        if (m_icaoDataReader) { return m_icaoDataReader->containsAircraftIcaoDesignator(designator); }
        return false;
    }

    CAircraftIcaoCodeList CWebDataServices::getAircraftIcaoCodesForDesignator(const QString &designator) const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAircraftIcaoCodesForDesignator(designator); }
        return CAircraftIcaoCodeList();
    }

    CAircraftIcaoCode CWebDataServices::getAircraftIcaoCodeForDbKey(int key) const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAircraftIcaoCodeForDbKey(key); }
        return CAircraftIcaoCode();
    }

    CAircraftIcaoCode CWebDataServices::smartAircraftIcaoSelector(const CAircraftIcaoCode &icao) const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->smartAircraftIcaoSelector(icao); }
        return icao;
    }

    CAirlineIcaoCodeList CWebDataServices::getAirlineIcaoCodes() const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAirlineIcaoCodes(); }
        return CAirlineIcaoCodeList();
    }

    bool CWebDataServices::containsAirlineIcaoDesignator(const QString &designator) const
    {
        if (designator.isEmpty()) { return false; }
        if (m_icaoDataReader) { return m_icaoDataReader->containsAirlineIcaoDesignator(designator); }
        return false;
    }

    CAirlineIcaoCode
    CWebDataServices::getAirlineIcaoCodeForUniqueDesignatorOrDefault(const QString &designator,
                                                                     bool preferOperatingAirlines) const
    {
        if (designator.isEmpty()) { return CAirlineIcaoCode(); }
        if (m_icaoDataReader)
        {
            return m_icaoDataReader->getAirlineIcaoCodeForUniqueDesignatorOrDefault(designator,
                                                                                    preferOperatingAirlines);
        }
        return CAirlineIcaoCode();
    }

    CAirlineIcaoCode CWebDataServices::getAirlineIcaoCodeForUniqueIataCodeOrDefault(const QString &iataCode) const
    {
        if (iataCode.isEmpty()) { return CAirlineIcaoCode(); }
        if (m_icaoDataReader) { return m_icaoDataReader->getAirlineIcaoCodeForUniqueIataCodeOrDefault(iataCode); }
        return CAirlineIcaoCode();
    }

    int CWebDataServices::getAirlineIcaoCodesCount() const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAirlineIcaoCodesCount(); }
        return 0;
    }

    QStringList CWebDataServices::getAirlineNames() const
    {
        QStringList names;
        if (!m_icaoDataReader) { return names; }
        for (const CAirlineIcaoCode &code : this->getAirlineIcaoCodes())
        {
            if (code.hasName()) { names.push_back(code.getName()); }
        }
        return names;
    }

    QStringList CWebDataServices::getTelephonyDesignators() const
    {
        QStringList designators;
        if (!m_icaoDataReader) { return designators; }
        for (const CAirlineIcaoCode &code : this->getAirlineIcaoCodes())
        {
            if (code.hasTelephonyDesignator()) { designators.push_back(code.getTelephonyDesignator()); }
        }
        return designators;
    }

    CAirlineIcaoCode CWebDataServices::smartAirlineIcaoSelector(const CAirlineIcaoCode &icaoPattern,
                                                                const CCallsign &callsign) const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->smartAirlineIcaoSelector(icaoPattern, callsign); }
        return CAirlineIcaoCode();
    }

    CAirlineIcaoCode CWebDataServices::findBestMatchByCallsign(const CCallsign &callsign) const
    {
        if (callsign.isEmpty()) { return CAirlineIcaoCode(); }
        const CAirlineIcaoCodeList icaos(this->getAirlineIcaoCodes());
        return icaos.findBestMatchByCallsign(callsign);
    }

    CAirlineIcaoCode CWebDataServices::getAirlineIcaoCodeForDbKey(int key) const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAirlineIcaoCodeForDbKey(key); }
        return CAirlineIcaoCode();
    }

    CCountryList CWebDataServices::getCountries() const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getCountries(); }
        return CCountryList();
    }

    int CWebDataServices::getCountriesCount() const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getCountriesCount(); }
        return 0;
    }

    CCountry CWebDataServices::getCountryForName(const QString &name) const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getCountryForName(name); }
        return CCountry();
    }

    CAirportList CWebDataServices::getAirports() const
    {
        if (m_airportDataReader) { return m_airportDataReader->getAirports(); }
        return CAirportList();
    }

    int CWebDataServices::getAirportsCount() const
    {
        if (m_airportDataReader) { return m_airportDataReader->getAirportsCount(); }
        return 0;
    }

    CAirport CWebDataServices::getAirportForIcaoDesignator(const QString &icao) const
    {
        if (m_airportDataReader) { return m_airportDataReader->getAirportForIcaoDesignator(icao); }
        return CAirport();
    }

    CAirport CWebDataServices::getAirportForNameOrLocation(const QString &nameOrLocation) const
    {
        if (m_airportDataReader) { return m_airportDataReader->getAirportForNameOrLocation(nameOrLocation); }
        return CAirport();
    }

    CCountry CWebDataServices::getCountryForIsoCode(const QString &iso) const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getCountryForIsoCode(iso); }
        return CCountry();
    }

    CMetarList CWebDataServices::getMetars() const
    {
        if (m_vatsimMetarReader) { return m_vatsimMetarReader->getMetars(); }
        return {};
    }

    CMetar CWebDataServices::getMetarForAirport(const CAirportIcaoCode &icao) const
    {
        if (m_vatsimMetarReader) { return m_vatsimMetarReader->getMetarForAirport(icao); }
        return CMetar();
    }

    CStatusMessageList CWebDataServices::validateForPublishing(const CAircraftModelList &modelsToBePublished,
                                                               bool ignoreEqual, CAircraftModelList &validModels,
                                                               CAircraftModelList &invalidModels) const
    {
        CStatusMessageList msgs(
            modelsToBePublished.validateForPublishing(validModels, invalidModels)); // technical validation

        // check against existing distributors
        const CDistributorList distributors(this->getDistributors());
        if (!distributors.isEmpty())
        {
            // only further check the valid ones
            CAircraftModelList newValidModels;
            const CStatusMessageList msgsDistributors(
                validModels.validateDistributors(distributors, newValidModels, invalidModels));
            validModels = newValidModels;
            msgs.push_back(msgsDistributors);
        }

        // check if model is changed
        // in case of not ignoreEqual we just check create the messages
        {
            CAircraftModelList newValidModels;
            for (const CAircraftModel &publishModel : validModels)
            {
                CStatusMessageList equalMessages;
                const bool changed = !this->isDbModelEqualForPublishing(publishModel, &equalMessages);
                if (changed)
                {
                    // all good
                    newValidModels.push_back(publishModel);
                    continue;
                }
                if (ignoreEqual) { equalMessages.warningToError(); }
                msgs.push_back(
                    CStatusMessage(this, ignoreEqual ? CStatusMessage::SeverityError : CStatusMessage::SeverityWarning,
                                   u"Model: '%1', there is no change")
                    << publishModel.getModelString());
                if (ignoreEqual) { invalidModels.push_back(publishModel); }
                else { newValidModels.push_back(publishModel); }
            }
            validModels = newValidModels;
        }
        return msgs;
    }

    bool CWebDataServices::isDbModelEqualForPublishing(const CAircraftModel &modelToBeChecked,
                                                       CStatusMessageList *details) const
    {
        const CAircraftModel compareDbModel = modelToBeChecked.isLoadedFromDb() ?
                                                  this->getModelForDbKey(modelToBeChecked.getDbKey()) :
                                                  this->getModelForModelString(modelToBeChecked.getModelString());
        return modelToBeChecked.isEqualForPublishing(compareDbModel, details);
    }

    CAirlineIcaoCodeList CWebDataServices::getAirlineIcaoCodesForDesignator(const QString &designator) const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAirlineIcaoCodesForDesignator(designator); }
        return CAirlineIcaoCodeList();
    }

    int CWebDataServices::getAirlineIcaoCodesForDesignatorCount(const QString &designator) const
    {
        return this->getAirlineIcaoCodesForDesignator(designator).size();
    }

    void CWebDataServices::gracefulShutdown()
    {
        if (m_shuttingDown) { return; }
        m_shuttingDown = true;
        this->disconnect(); // all signals
        if (m_vatsimMetarReader)
        {
            m_vatsimMetarReader->quitAndWait();
            m_vatsimMetarReader = nullptr;
        }
        if (m_vatsimDataFileReader)
        {
            m_vatsimDataFileReader->quitAndWait();
            m_vatsimDataFileReader = nullptr;
        }
        if (m_vatsimStatusReader)
        {
            m_vatsimStatusReader->quitAndWait();
            m_vatsimStatusReader = nullptr;
        }
        if (m_vatsimServerFileReader)
        {
            m_vatsimServerFileReader->quitAndWait();
            m_vatsimServerFileReader = nullptr;
        }
        if (m_modelDataReader)
        {
            m_modelDataReader->quitAndWait();
            m_modelDataReader = nullptr;
        }
        if (m_airportDataReader)
        {
            m_airportDataReader->quitAndWait();
            m_airportDataReader = nullptr;
        }
        if (m_icaoDataReader)
        {
            m_icaoDataReader->quitAndWait();
            m_icaoDataReader = nullptr;
        }
        if (m_dbInfoDataReader)
        {
            m_dbInfoDataReader->quitAndWait();
            m_dbInfoDataReader = nullptr;
        }

        // DB writer is no threaded reader, it has a special role
        if (m_databaseWriter)
        {
            m_databaseWriter->gracefulShutdown();
            m_databaseWriter = nullptr;
        }
    }

    CEntityFlags::Entity CWebDataServices::allDbEntitiesForUsedReaders() const
    {
        // obtain entities from real readers (means when reader is really used)
        CEntityFlags::Entity entities = CEntityFlags::NoEntity;
        if (m_icaoDataReader) { entities |= CWebReaderFlags::allEntitiesForReaders(CWebReaderFlags::IcaoDataReader); }
        if (m_modelDataReader) { entities |= CWebReaderFlags::allEntitiesForReaders(CWebReaderFlags::ModelReader); }
        if (m_airportDataReader) { entities |= CWebReaderFlags::allEntitiesForReaders(CWebReaderFlags::AirportReader); }

        // when we have a config, we ignore the ones not from cache or DB
        if (!m_dbReaderConfig.isEmpty())
        {
            CEntityFlags::Entity configuredEntities = m_dbReaderConfig.getEntitesCachedOrReadFromDB();
            entities &= configuredEntities;
        }

        entities &= CEntityFlags::AllDbEntities; // make sure to only use DB data
        return entities;
    }

    const QStringList &CWebDataServices::getLogCategories()
    {
        static const QStringList cats { "swift.datareader", CLogCategories::webservice() };
        return cats;
    }

    void CWebDataServices::initReaders(CWebReaderFlags::WebReader readersNeeded, CEntityFlags::Entity entities)
    {
        Q_ASSERT_X(CThreadUtils::thisIsMainThread(), Q_FUNC_INFO, "shall run in main application thread");

        //
        // ---- "metadata" reader, 1 will trigger read directly during init
        //
        CDatabaseReaderConfigList dbReaderConfig(m_dbReaderConfig);
        const CEntityFlags::Entity dbEntities = entities & CEntityFlags::AllDbEntitiesNoInfoObjects;
        const bool anyDbEntities = CEntityFlags::anySwiftDbEntity(dbEntities); // contains any DB entities
        const bool needsSharedInfoObjects = dbReaderConfig.needsSharedInfoObjects(dbEntities);
        const bool needsDbInfoObjects = dbReaderConfig.possiblyReadsFromSwiftDb();
        bool c = false; // for signal connect

        // 1a. If any DB data, read the info objects upfront
        if (needsDbInfoObjects)
        {
            if (anyDbEntities && readersNeeded.testFlag(CWebReaderFlags::WebReaderFlag::DbInfoDataReader))
            {
                // info data reader has a special role, it will not be triggered in triggerRead()
                this->initDbInfoObjectReaderAndTriggerRead();
            }
        }

        // 1b. Read info objects if needed
        if (needsSharedInfoObjects) { this->initSharedInfoObjectReaderAndTriggerRead(); }

        // 2. Status and server file, updating the VATSIM related caches
        // Read as soon as initReaders is done
        if (readersNeeded.testFlag(CWebReaderFlags::VatsimStatusReader) ||
            readersNeeded.testFlag(CWebReaderFlags::VatsimDataReader) ||
            readersNeeded.testFlag(CWebReaderFlags::VatsimMetarReader))
        {
            m_vatsimStatusReader = new CVatsimStatusFileReader(this);
            c = connect(m_vatsimStatusReader, &CVatsimStatusFileReader::statusFileRead, this,
                        &CWebDataServices::vatsimStatusFileRead, Qt::QueuedConnection);
            CLogMessage(this).info(u"Trigger read of VATSIM status file");
            m_vatsimStatusReader->start(QThread::LowPriority);

            // run single shot in main loop, so readInBackgroundThread is not called before initReaders completes
            const QPointer<CWebDataServices> myself(this);
            QTimer::singleShot(0, this, [=]() {
                if (!myself || m_shuttingDown) { return; }
                if (!sApp || sApp->isShuttingDown()) { return; }
                m_vatsimStatusReader->readInBackgroundThread();
            });

            startVatsimServerFileReader();
        }

        // ---- "normal data", triggerRead will start read, not starting directly

        // 3. VATSIM data file
        if (readersNeeded.testFlag(CWebReaderFlags::WebReaderFlag::VatsimDataReader))
        {
            m_vatsimDataFileReader = new CVatsimDataFileReader(this);
            c = connect(m_vatsimDataFileReader, &CVatsimDataFileReader::dataFileRead, this,
                        &CWebDataServices::vatsimDataFileRead, Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "VATSIM data reader signals");
            c = connect(m_vatsimDataFileReader, &CVatsimDataFileReader::dataRead, this, &CWebDataServices::dataRead,
                        Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed VATSIM data file");
            m_entitiesPeriodicallyRead |= CEntityFlags::VatsimDataFile;
            m_vatsimDataFileReader->start(QThread::LowPriority);
            m_vatsimDataFileReader->startReader();
        }

        // 4. VATSIM METAR data
        if (readersNeeded.testFlag(CWebReaderFlags::WebReaderFlag::VatsimMetarReader))
        {
            m_vatsimMetarReader = new CVatsimMetarReader(this);
            c = connect(m_vatsimMetarReader, &CVatsimMetarReader::metarsRead, this, &CWebDataServices::receivedMetars,
                        Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "VATSIM METAR reader signals");
            c = connect(m_vatsimMetarReader, &CVatsimMetarReader::dataRead, this, &CWebDataServices::dataRead,
                        Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed VATSIM METAR");
            m_entitiesPeriodicallyRead |= CEntityFlags::MetarEntity;
            m_vatsimMetarReader->start(QThread::LowPriority);
            m_vatsimMetarReader->startReader();
        }

        // 5. ICAO data reader
        if (readersNeeded.testFlag(CWebReaderFlags::WebReaderFlag::IcaoDataReader))
        {
            m_icaoDataReader = new CIcaoDataReader(this, dbReaderConfig);
            c = connect(m_icaoDataReader, &CIcaoDataReader::dataRead, this, &CWebDataServices::readFromSwiftReader,
                        Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect ICAO reader signals");
            c = connect(m_icaoDataReader, &CIcaoDataReader::dataRead, this, &CWebDataServices::dataRead,
                        Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect ICAO reader signals");
            c = connect(m_icaoDataReader, &CIcaoDataReader::swiftDbDataRead, this, &CWebDataServices::swiftDbDataRead,
                        Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect Model reader signals");
            c = connect(m_icaoDataReader, &CIcaoDataReader::entityDownloadProgress, this,
                        &CWebDataServices::entityDownloadProgress, Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect Model reader signals");
            m_icaoDataReader->start(QThread::LowPriority);
        }

        // 6. Model reader
        if (readersNeeded.testFlag(CWebReaderFlags::WebReaderFlag::ModelReader))
        {
            m_modelDataReader = new CModelDataReader(this, dbReaderConfig);
            c = connect(m_modelDataReader, &CModelDataReader::dataRead, this, &CWebDataServices::readFromSwiftReader,
                        Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect Model reader signals");
            c = connect(m_modelDataReader, &CModelDataReader::dataRead, this, &CWebDataServices::dataRead,
                        Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect Model reader signals");
            c = connect(m_modelDataReader, &CModelDataReader::swiftDbDataRead, this, &CWebDataServices::swiftDbDataRead,
                        Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect Model reader signals");
            c = connect(m_modelDataReader, &CModelDataReader::entityDownloadProgress, this,
                        &CWebDataServices::entityDownloadProgress, Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect Model reader signals");
            m_modelDataReader->start(QThread::LowPriority);
        }

        // 7. Airport reader
        if (readersNeeded.testFlag(CWebReaderFlags::WebReaderFlag::AirportReader))
        {
            m_airportDataReader = new CAirportDataReader(this, dbReaderConfig);
            c = connect(m_airportDataReader, &CAirportDataReader::dataRead, this,
                        &CWebDataServices::readFromSwiftReader, Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect Model reader signals");
            c = connect(m_airportDataReader, &CAirportDataReader::dataRead, this, &CWebDataServices::dataRead,
                        Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect Model reader signals");
            c = connect(m_airportDataReader, &CAirportDataReader::swiftDbDataRead, this,
                        &CWebDataServices::swiftDbDataRead, Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect Model reader signals");
            c = connect(m_airportDataReader, &CAirportDataReader::entityDownloadProgress, this,
                        &CWebDataServices::entityDownloadProgress, Qt::QueuedConnection);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect Model reader signals");
            m_airportDataReader->start(QThread::LowPriority);
        }
        Q_UNUSED(c) // signal connect flag

        const QDateTime threshold =
            QDateTime::currentDateTimeUtc().addDays(-365); // country and airports are "semi static"
        const CEntityFlags::Entity cachedDbEntities =
            this->getDbEntitiesWithCachedData(); // those caches are already read
        const CEntityFlags::Entity validTsDbEntities =
            this->getDbEntitiesWithTimestampNewerThan(threshold); // those caches are not read, but have a timestamp
        const bool needsSharedInfoObjectsWithoutCache =
            dbReaderConfig.needsSharedInfoObjectsIfCachesEmpty(dbEntities, cachedDbEntities | validTsDbEntities);
        if (m_sharedInfoDataReader && !needsSharedInfoObjectsWithoutCache)
        {
            // demote error message
            // Rational: we cannot read shared info objects, but we have and use cached objects
            m_sharedInfoDataReader->setSeverityNoWorkingUrl(CStatusMessage::SeverityWarning);
        }
    }

    void CWebDataServices::startVatsimServerFileReader()
    {
        m_vatsimServerFileReader = new CVatsimServerFileReader(this);
        connect(m_vatsimServerFileReader, &CVatsimServerFileReader::dataFileRead, this,
                &CWebDataServices::vatsimServerFileRead, Qt::QueuedConnection);
        CLogMessage(this).info(u"Trigger read of VATSIM server file");
        m_vatsimServerFileReader->start(QThread::LowPriority);

        // run single shot in main loop, so readInBackgroundThread is not called before initReaders completes
        const QPointer<CWebDataServices> myself(this);
        QTimer::singleShot(0, this, [=]() {
            if (!myself || m_shuttingDown) { return; }
            if (!sApp || sApp->isShuttingDown()) { return; }
            m_vatsimServerFileReader->readInBackgroundThread();
        });
    }

    void CWebDataServices::initDbInfoObjectReaderAndTriggerRead()
    {
        // run in correct thread
        if (m_shuttingDown) { return; }
        if (!CThreadUtils::isInThisThread(this))
        {
            const QPointer<CWebDataServices> myself(this);
            QTimer::singleShot(0, this, [=] {
                if (!myself || m_shuttingDown) { return; }
                if (!sApp || sApp->isShuttingDown()) { return; }
                this->initDbInfoObjectReaderAndTriggerRead();
            });
            return;
        }

        if (!m_dbInfoDataReader)
        {
            m_dbInfoDataReader = new CInfoDataReader(this, m_dbReaderConfig, CDbFlags::DbReading);
            m_dbInfoDataReader->setObjectName(m_dbInfoDataReader->objectName() + " (DB)");
            bool c =
                connect(m_dbInfoDataReader, &CInfoDataReader::dataRead, this, &CWebDataServices::readFromSwiftReader);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Info reader connect failed");

            // relay signal
            c = connect(m_dbInfoDataReader, &CInfoDataReader::dataRead, this, &CWebDataServices::dataRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Info reader connect failed");
            c = connect(m_dbInfoDataReader, &CInfoDataReader::databaseReaderMessages, this,
                        &CWebDataServices::databaseReaderMessages);
            Q_UNUSED(c)

            // start in own thread
            m_dbInfoDataReader->start(QThread::LowPriority);

            const QPointer<CWebDataServices> myself(this);
            QTimer::singleShot(25, m_dbInfoDataReader, [=]() {
                if (!myself || m_shuttingDown) { return; }
                if (!sApp || sApp->isShuttingDown()) { return; }
                m_dbInfoDataReader->readInfoData(); // trigger read of info objects
            });
        }
    }

    void CWebDataServices::initSharedInfoObjectReaderAndTriggerRead()
    {
        // run in correct thread
        if (m_shuttingDown) { return; }
        if (!CThreadUtils::isInThisThread(this))
        {
            const QPointer<CWebDataServices> myself(this);
            QTimer::singleShot(0, this, [=] {
                if (!myself || m_shuttingDown) { return; }
                this->initSharedInfoObjectReaderAndTriggerRead();
            });
            return;
        }

        if (!m_sharedInfoDataReader)
        {
            m_sharedInfoDataReader = new CInfoDataReader(this, m_dbReaderConfig, CDbFlags::Shared);
            m_sharedInfoDataReader->setObjectName(m_sharedInfoDataReader->objectName() + " (shared)");
            bool c = connect(m_sharedInfoDataReader, &CInfoDataReader::dataRead, this,
                             &CWebDataServices::readFromSwiftReader);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Info reader connect failed");

            // relay signal
            c = connect(m_sharedInfoDataReader, &CInfoDataReader::dataRead, this, &CWebDataServices::dataRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Info reader connect failed");
            Q_UNUSED(c)

            // start in own thread
            m_sharedInfoDataReader->start(QThread::LowPriority);
        }

        // and trigger read
        const QPointer<CWebDataServices> myself(this);
        QTimer::singleShot(25, m_sharedInfoDataReader, [=]() {
            if (!myself || m_shuttingDown) { return; }
            m_sharedInfoDataReader->readInfoData();
        });
    }

    CDatabaseReader *CWebDataServices::getDbReader(CEntityFlags::Entity entity) const
    {
        Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "Need single entity");
        Q_ASSERT_X(CEntityFlags::anySwiftDbEntity(entity), Q_FUNC_INFO, "No swift DB entity");

        const CWebReaderFlags::WebReader wr = CWebReaderFlags::entitiesToReaders(entity);
        switch (wr)
        {
        case CWebReaderFlags::IcaoDataReader: return m_icaoDataReader;
        case CWebReaderFlags::ModelReader: return m_modelDataReader;
        case CWebReaderFlags::AirportReader: return m_airportDataReader;
        default: break;
        }
        return nullptr;
    }

    void CWebDataServices::initWriters()
    {
        m_databaseWriter = new CDatabaseWriter(sApp->getGlobalSetup().getDbRootDirectoryUrl(), this);
    }

    bool CWebDataServices::signalEntitiesAlreadyRead(CEntityFlags::Entity entities)
    {
        if (m_signalledEntities.contains(entities)) { return false; }
        m_signalledEntities.insert(entities);
        return true;
    }

    int CWebDataServices::getInfoObjectCount(CEntityFlags::Entity entity, CInfoDataReader *reader) const
    {
        Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "Need single entity");
        Q_ASSERT_X(reader, Q_FUNC_INFO, "Need reader");
        if (CEntityFlags::anySwiftDbEntity(entity))
        {
            const CDbInfo info = reader->getInfoObjects().findFirstByEntityOrDefault(entity);
            return info.getEntries();
        }
        else
        {
            // non DB entities would go here
            return -1;
        }
    }

    CEntityFlags::Entity CWebDataServices::getDbEntitiesWithCachedData() const
    {
        CEntityFlags::Entity entities = CEntityFlags::NoEntity;
        if (m_airportDataReader) { entities |= m_airportDataReader->getEntitiesWithCacheCount(); }
        if (m_icaoDataReader) { entities |= m_icaoDataReader->getEntitiesWithCacheCount(); }
        if (m_modelDataReader) { entities |= m_modelDataReader->getEntitiesWithCacheCount(); }
        return entities;
    }

    CEntityFlags::Entity CWebDataServices::getDbEntitiesWithTimestampNewerThan(const QDateTime &threshold) const
    {
        CEntityFlags::Entity entities = CEntityFlags::NoEntity;
        if (m_airportDataReader) { entities |= m_airportDataReader->getEntitiesWithCacheTimestampNewerThan(threshold); }
        if (m_icaoDataReader) { entities |= m_icaoDataReader->getEntitiesWithCacheTimestampNewerThan(threshold); }
        if (m_modelDataReader) { entities |= m_modelDataReader->getEntitiesWithCacheTimestampNewerThan(threshold); }
        return entities;
    }

    void CWebDataServices::receivedMetars(const CMetarList &metars)
    {
        CLogMessage(this).info(u"Read %1 METARs") << metars.size();
    }

    void CWebDataServices::vatsimDataFileRead(int kB) { CLogMessage(this).info(u"Read VATSIM data file, %1 kB") << kB; }

    void CWebDataServices::vatsimStatusFileRead(int bytes)
    {
        CLogMessage(this).info(u"Read VATSIM status file, %1 bytes") << bytes;
    }

    void CWebDataServices::vatsimServerFileRead(int bytes)
    {
        CLogMessage(this).info(u"Read VATSIM server file, %1 bytes") << bytes;
    }

    void CWebDataServices::readFromSwiftReader(CEntityFlags::Entity entities, CEntityFlags::ReadState state, int number,
                                               const QUrl &url)
    {
        if (state == CEntityFlags::ReadStarted) { return; } // just started

        const QString from = url.isEmpty() ? QStringLiteral("") : QStringLiteral(" from '%1'").arg(url.toString());
        const QString entStr = CEntityFlags::entitiesToString(entities);

        if (CEntityFlags::isWarningOrAbove(state))
        {
            const CStatusMessage::StatusSeverity severity = CEntityFlags::flagToSeverity(state);
            if (severity == CStatusMessage::SeverityWarning)
            {
                CLogMessage(this).warning(u"Read data '%1' entries: %2 state: %3%4")
                    << entStr << number << CEntityFlags::stateToString(state) << from;
            }
            else
            {
                CLogMessage(this).error(u"Read data '%1' entries: %2 state: %3%4")
                    << entStr << number << CEntityFlags::stateToString(state) << from;
            }
        }
        else
        {
            CLogMessage(this).info(u"Read data '%1' entries: %2 state: %3%4")
                << entStr << number << CEntityFlags::stateToString(state) << from;
        }

        m_swiftDbEntitiesRead |= entities;
        const int allUsedEntities = static_cast<int>(this->allDbEntitiesForUsedReaders());
        if (((static_cast<int>(m_swiftDbEntitiesRead)) & allUsedEntities) == allUsedEntities)
        {
            emit this->swiftDbAllDataRead();
        }

        // individual signals
        if (CEntityFlags::isFinishedReadState(state))
        {
            // emit one time only
            if (entities.testFlag(CEntityFlags::AirportEntity) &&
                signalEntitiesAlreadyRead(CEntityFlags::AirportEntity))
            {
                emit swiftDbAirportsRead();
            }
            if (entities.testFlag(CEntityFlags::AirlineIcaoEntity) &&
                signalEntitiesAlreadyRead(CEntityFlags::AirlineIcaoEntity))
            {
                emit swiftDbAirlineIcaoRead();
            }
            if (entities.testFlag(CEntityFlags::AircraftIcaoEntity) &&
                signalEntitiesAlreadyRead(CEntityFlags::AircraftIcaoEntity))
            {
                emit swiftDbAircraftIcaoRead();
            }
            if (entities.testFlag(CEntityFlags::ModelEntity) && signalEntitiesAlreadyRead(CEntityFlags::ModelEntity))
            {
                emit swiftDbModelsRead();
            }
            if (entities.testFlag(CEntityFlags::SharedInfoObjectEntity)) { emit sharedInfoObjectsRead(); }

            if (m_swiftDbEntitiesRead.testFlag(CEntityFlags::AllIcaoEntities) &&
                signalEntitiesAlreadyRead(CEntityFlags::AllIcaoEntities))
            {
                emit this->swiftDbAllIcaoEntitiesRead();
            }
            if (m_swiftDbEntitiesRead.testFlag(CEntityFlags::ModelMatchingEntities) &&
                signalEntitiesAlreadyRead(CEntityFlags::ModelMatchingEntities))
            {
                emit this->swiftDbModelMatchingEntitiesRead();
            }
        }
    }

    void CWebDataServices::readDeferredInBackground(CEntityFlags::Entity entities, int delayMs)
    {
        if (m_shuttingDown) { return; }
        if (entities == CEntityFlags::NoEntity) { return; }
        const QPointer<CWebDataServices> myself(this);
        QTimer::singleShot(delayMs, [=]() // clazy:exclude=connect-3arg-lambda
                           {
                               if (!myself || m_shuttingDown) { return; }
                               this->readInBackground(entities); // deferred
                           });
    }

    void CWebDataServices::readInBackground(CEntityFlags::Entity entities)
    {
        if (m_shuttingDown) { return; }

        if (CEntityFlags::anySwiftDbEntity(entities))
        {
            // with info objects wait until info objects are loaded
            Q_ASSERT_X(!entities.testFlag(CEntityFlags::DbInfoObjectEntity), Q_FUNC_INFO,
                       "Info object must be read upfront, do not pass as entity here");
            const bool waitForDbInfoReader = m_dbInfoDataReader && !m_dbInfoDataReader->areAllInfoObjectsRead() &&
                                             !m_dbInfoDataReader->isMarkedAsFailed();
            if (waitForDbInfoReader)
            {
                // do not read yet, will call this function again after some time
                // see CWebDataServices::waitForInfoObjects
                if (!this->waitForDbInfoObjectsThenRead(entities)) { return; }
            }

            const bool waitForSharedInfoFile =
                m_dbReaderConfig.needsSharedInfoFile(entities) && !m_sharedInfoDataReader->areAllInfoObjectsRead();
            if (waitForSharedInfoFile)
            {
                // do not read yet, will call this function again after some time
                // CWebDataServices::waitForInfoObjects
                if (!this->waitForSharedInfoObjectsThenRead(entities)) { return; }
            }
        }

        // read entities
        this->triggerRead(entities);
    }

    bool CWebDataServices::waitForDbInfoObjectsThenRead(CEntityFlags::Entity entities)
    {
        if (m_shuttingDown) { return false; }

        Q_ASSERT_X(m_dbInfoDataReader, Q_FUNC_INFO, "need reader");

        // in a dev build all symbols are loaded which sometimes causes unnecessary timeout
        if (m_dbInfoDataReader->areAllInfoObjectsRead()) { return true; }
        const int timeOutMs = 30 * 1000;
        if (!m_dbInfoObjectTimeout.isValid())
        {
            m_dbInfoObjectTimeout = QDateTime::currentDateTimeUtc().addMSecs(timeOutMs);
            CLogMessage(this).info(u"Set DbInfoObjects timeout %1ms to %2")
                << timeOutMs << m_dbInfoObjectTimeout.toString("dd.MM.yyyy hh:mm:ss");
        }
        const bool read = this->waitForInfoObjectsThenRead(entities, "DB", m_dbInfoDataReader, m_dbInfoObjectTimeout);
        return read;
    }

    bool CWebDataServices::waitForSharedInfoObjectsThenRead(CEntityFlags::Entity entities)
    {
        if (m_shuttingDown) { return false; }

        Q_ASSERT_X(m_sharedInfoDataReader, Q_FUNC_INFO, "need reader");
        if (m_sharedInfoDataReader->areAllInfoObjectsRead()) { return true; }
        const int timeOutMs = 30 * 1000;
        if (!m_sharedInfoObjectsTimeout.isValid())
        {
            m_sharedInfoObjectsTimeout = QDateTime::currentDateTimeUtc().addMSecs(timeOutMs);
            CLogMessage(this).info(u"Set SharedInfoObjects timeout %1ms to %2")
                << timeOutMs << m_sharedInfoObjectsTimeout.toString("dd.MM.yyyy hh:mm:ss");
        }
        const bool read =
            this->waitForInfoObjectsThenRead(entities, "shared", m_sharedInfoDataReader, m_sharedInfoObjectsTimeout);
        return read;
    }

    bool CWebDataServices::waitForInfoObjectsThenRead(CEntityFlags::Entity entities, const QString &info,
                                                      CInfoDataReader *infoReader, QDateTime &timeOut)
    {
        if (m_shuttingDown) { return false; }

        // this will called for each entity readers, i.e. model reader, ICAO reader ...
        Q_ASSERT_X(infoReader, Q_FUNC_INFO, "Need info data reader");
        const int waitForInfoObjectsMs = 1000; // ms

        if (infoReader->areAllInfoObjectsRead())
        {
            // we have all data and carry on
            CLogMessage(this).info(u"Info objects (%1) triggered for '%2' loaded from '%3'")
                << info << CEntityFlags::entitiesToString(entities) << infoReader->getInfoObjectsUrl().toQString();
            timeOut = QDateTime(); // reset to null
            return true; // no need to wait any longer
        }

        // try to read if not timed out
        if (timeOut.isValid() && QDateTime::currentDateTimeUtc() > timeOut)
        {
            const QString timeOutString = timeOut.toString();
            const CStatusMessage m =
                CLogMessage(this).warning(u"Could not read '%1' info objects for '%2' from '%3', time out '%4'. "
                                          u"Marking reader '%5' as failed and continue.")
                << info << CEntityFlags::entitiesToString(entities) << infoReader->getInfoObjectsUrl().toQString()
                << timeOutString << infoReader->getName();
            emit this->databaseReaderMessages(m);

            // continue here and read data without info objects
            infoReader->setMarkedAsFailed(true);
            // no timeout reset here
            return true; // carry on, regardless of situation
        }

        if (infoReader->hasReceivedFirstReply())
        {
            // we have received a response, but not all data yet
            if (infoReader->hasReceivedOkReply())
            {
                // ok, this means we are parsing
                this->readDeferredInBackground(entities, waitForInfoObjectsMs);
                const CStatusMessage m = CLogMessage(this).info(u"Parsing objects (%1) for '%2' from '%3'")
                                         << info << CEntityFlags::entitiesToString(entities)
                                         << infoReader->getInfoObjectsUrl().toQString();
                emit this->databaseReaderMessages(m);
                return false; // wait
            }
            else
            {
                // we have a response, but a failure, means server is alive, but responded with error
                // such an error (access, ...) normally will not go away
                const CStatusMessage m =
                    CLogMessage(this).error(u"Info objects (%1) loading for '%2' failed from '%3', '%4'")
                    << info << CEntityFlags::entitiesToString(entities) << infoReader->getInfoObjectsUrl().toQString()
                    << infoReader->getStatusMessage();
                infoReader->setMarkedAsFailed(true);
                emit this->databaseReaderMessages(m);
                return true; // carry on, regardless of situation
            }
        }
        else
        {
            // wait for 1st reply
            // we call read again in some time
            this->readDeferredInBackground(entities, waitForInfoObjectsMs);
            return false; // wait
        }
    }

    bool CWebDataServices::writeDbDataToDisk(const QString &dir)
    {
        if (dir.isEmpty()) { return false; }
        const QDir directory(dir);
        if (!directory.exists())
        {
            const bool s = directory.mkpath(dir);
            if (!s) { return false; }
        }
        QList<QPair<QString, QString>> fileContents;

        if (this->getModelsCount() > 0)
        {
            const QString json(QJsonDocument(this->getModels().toJson()).toJson());
            fileContents.push_back({ "models.json", json });
        }

        if (this->getLiveriesCount() > 0)
        {
            const QString json(QJsonDocument(this->getLiveries().toJson()).toJson());
            fileContents.push_back({ "liveries.json", json });
        }

        if (this->getAirportsCount() > 0)
        {
            const QString json(QJsonDocument(this->getAirports().toJson()).toJson());
            fileContents.push_back({ "airports.json", json });
        }

        for (const auto &pair : fileContents)
        {
            CWorker::fromTask(this, Q_FUNC_INFO, [pair, directory] {
                CFileUtils::writeStringToFile(CFileUtils::appendFilePaths(directory.absolutePath(), pair.first),
                                              pair.second);
            });
        }
        return true;
    }

    bool CWebDataServices::readDbDataFromDisk(const QString &dir, bool inBackground, bool overrideNewerOnly)
    {
        if (dir.isEmpty()) { return false; }
        const QDir directory(dir);
        if (!directory.exists()) { return false; }

        bool s1 = !m_icaoDataReader;
        if (m_icaoDataReader)
        {
            // force update to background reading if reader is already in another thread
            bool ib = inBackground || !CThreadUtils::isInThisThread(m_icaoDataReader);
            if (ib)
            {
                CLogMessage(this).info(u"Reading from disk in background: %1")
                    << m_icaoDataReader->getSupportedEntitiesAsString();
                s1 = m_icaoDataReader->readFromJsonFilesInBackground(dir, m_icaoDataReader->getSupportedEntities(),
                                                                     overrideNewerOnly);
            }
            else
            {
                const CStatusMessageList msgs = m_icaoDataReader->readFromJsonFiles(
                    dir, m_icaoDataReader->getSupportedEntities(), overrideNewerOnly);
                CLogMessage::preformatted(msgs);
                s1 = msgs.isSuccess();
            }
        }

        bool s2 = !m_modelDataReader;
        if (m_modelDataReader)
        {
            // force update to background reading if reader is already in another thread
            bool ib = inBackground || !CThreadUtils::isInThisThread(m_modelDataReader);
            if (ib)
            {
                CLogMessage(this).info(u"Reading from disk in background: %1")
                    << m_modelDataReader->getSupportedEntitiesAsString();
                s2 = m_modelDataReader->readFromJsonFilesInBackground(dir, m_modelDataReader->getSupportedEntities(),
                                                                      overrideNewerOnly);
            }
            else
            {
                const CStatusMessageList msgs = m_modelDataReader->readFromJsonFiles(
                    dir, m_modelDataReader->getSupportedEntities(), overrideNewerOnly);
                CLogMessage::preformatted(msgs);
                s2 = msgs.isSuccess();
            }
        }

        bool s3 = !m_airportDataReader;
        if (m_airportDataReader)
        {
            // force update to background reading if reader is already in another thread
            bool ib = inBackground || !CThreadUtils::isInThisThread(m_airportDataReader);
            if (ib)
            {
                CLogMessage(this).info(u"Reading from disk in background: %1")
                    << m_airportDataReader->getSupportedEntitiesAsString();
                s3 = m_airportDataReader->readFromJsonFilesInBackground(
                    dir, m_airportDataReader->getSupportedEntities(), overrideNewerOnly);
            }
            else
            {
                const CStatusMessageList msgs = m_airportDataReader->readFromJsonFiles(
                    dir, m_airportDataReader->getSupportedEntities(), overrideNewerOnly);
                CLogMessage::preformatted(msgs);
                s3 = msgs.isSuccess();
            }
        }

        return s1 && s2 && s3;
    }

    CStatusMessageList CWebDataServices::initDbCachesFromLocalResourceFiles(bool inBackground)
    {
        CStatusMessageList msgs;
        msgs.push_back(m_icaoDataReader ? m_icaoDataReader->initFromLocalResourceFiles(inBackground) :
                                          CStatusMessage(this).info(u"No ICAO reader"));
        msgs.push_back(m_modelDataReader ? m_modelDataReader->initFromLocalResourceFiles(inBackground) :
                                           CStatusMessage(this).info(u"No model reader"));
        msgs.push_back(m_airportDataReader ? m_airportDataReader->initFromLocalResourceFiles(inBackground) :
                                             CStatusMessage(this).info(u"No airport reader"));
        return msgs;
    }

    //! \cond PRIVATE
    CStatusMessageList CWebDataServices::initDbCachesFromLocalResourceFiles(CEntityFlags::Entity entities,
                                                                            bool inBackground)
    {
        CStatusMessageList msgs;
        msgs.push_back(m_icaoDataReader && m_icaoDataReader->supportsAnyOfEntities(entities) ?
                           m_icaoDataReader->initFromLocalResourceFiles(entities, inBackground) :
                           CStatusMessage(this).info(u"No ICAO reader or not supporting entities"));
        msgs.push_back(m_modelDataReader && m_modelDataReader->supportsAnyOfEntities(entities) ?
                           m_modelDataReader->initFromLocalResourceFiles(entities, inBackground) :
                           CStatusMessage(this).info(u"No model reader or not supporting entities"));
        msgs.push_back(m_airportDataReader && m_airportDataReader->supportsAnyOfEntities(entities) ?
                           m_airportDataReader->initFromLocalResourceFiles(entities, inBackground) :
                           CStatusMessage(this).info(u"No airport reader or not supporting entities"));
        return msgs;
    }
    //! \endcond
} // namespace swift::core
