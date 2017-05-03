/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/db/airportdatareader.h"
#include "blackcore/db/infodatareader.h"
#include "blackcore/db/icaodatareader.h"
#include "blackcore/db/databasewriter.h"
#include "blackcore/db/icaodatareader.h"
#include "blackcore/db/modeldatareader.h"
#include "blackcore/setupreader.h"
#include "blackcore/vatsim/vatsimbookingreader.h"
#include "blackcore/vatsim/vatsimdatafilereader.h"
#include "blackcore/vatsim/vatsimmetarreader.h"
#include "blackcore/vatsim/vatsimstatusfilereader.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/restricted.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/worker.h"

#include <QDir>
#include <QFlags>
#include <QJsonDocument>
#include <QSslSocket>
#include <QThread>
#include <QTimer>
#include <QtGlobal>

using namespace BlackCore;
using namespace BlackCore::Db;
using namespace BlackCore::Data;
using namespace BlackCore::Vatsim;
using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Weather;

namespace BlackCore
{
    CWebDataServices::CWebDataServices(CWebReaderFlags::WebReader readers, const CDatabaseReaderConfigList &dbReaderConfig, BlackMisc::Restricted<CApplication>, QObject *parent) :
        QObject(parent), m_readers(readers), m_dbReaderConfig(dbReaderConfig)
    {
        if (!sApp) { return; } // shutting down

        Q_ASSERT_X(QSslSocket::supportsSsl(), Q_FUNC_INFO, "Missing SSL support");
        Q_ASSERT_X(sApp->isSetupAvailable(), Q_FUNC_INFO, "Setup not synchronized");
        this->setObjectName("CWebDataReader");

        // check if I need info objects
        const bool readFromSwiftDb = dbReaderConfig.possiblyReadsFromSwiftDb(); // DB read access
        const bool writeToSwiftDb = dbReaderConfig.possiblyWritesToSwiftDb(); // DB write access
        if (!readFromSwiftDb && readers.testFlag(CWebReaderFlags::DbInfoDataReader))
        {
            // will remove info reader because not needed
            readers &= ~CWebReaderFlags::DbInfoDataReader;
            this->m_readers = readers;
            CLogMessage(this).info("Remove info object reader because not needed");
        }

        // get entities to be read
        CEntityFlags::Entity entities = CWebReaderFlags::allEntitiesForReaders(readers);
        if (entities.testFlag(CEntityFlags::DbInfoObjectEntity))
        {
            Q_ASSERT_X(readers.testFlag(CWebReaderFlags::DbInfoDataReader), Q_FUNC_INFO, "info object but no reader");
            CLogMessage(this).info("Using info objects for swift DB entities");
        }

        this->initReaders(readers, entities); // reads info objects if required
        if (writeToSwiftDb)
        {
            this->initWriters();
        }

        // make sure this is called in event queue, so pending tasks cam be performed
        entities &= ~CEntityFlags::DbInfoObjectEntity; // triggered in init readers
        entities &= ~CEntityFlags::VatsimStatusFile;   // triggered in init readers
        entities &= ~this->m_entitiesPeriodicallyRead; // will be triggered by timers

        // trigger reading
        // but do not start all at the same time
        const CEntityFlags::Entity icaoPart = entities & CEntityFlags::AllIcaoAndCountries;
        const CEntityFlags::Entity modelPart = entities & CEntityFlags::DistributorLiveryModel;
        CEntityFlags::Entity remainingEntities = entities & ~icaoPart;
        remainingEntities &= ~modelPart;
        this->readDeferredInBackground(icaoPart, 500);
        this->readDeferredInBackground(modelPart, 1000);
        this->readDeferredInBackground(remainingEntities, 1500);
    }

    CServerList CWebDataServices::getVatsimFsdServers() const
    {
        if (m_vatsimDataFileReader) { return m_vatsimDataFileReader->getFsdServers(); }
        return CServerList();
    }

    CServerList CWebDataServices::getVatsimVoiceServers() const
    {
        if (m_vatsimDataFileReader) { return m_vatsimDataFileReader->getVoiceServers(); }
        return CServerList();
    }

    CUrlList CWebDataServices::getVatsimMetarUrls() const
    {
        if (m_vatsimStatusReader) { return m_vatsimStatusReader->getMetarFileUrls(); }
        return CUrlList();
    }

    CUrlList CWebDataServices::getVatsimDataFileUrls() const
    {
        if (m_vatsimStatusReader) { return m_vatsimStatusReader->getDataFileUrls(); }
        return CUrlList();
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

    CStatusMessageList CWebDataServices::asyncPublishModels(const CAircraftModelList &models) const
    {
        if (m_databaseWriter) { return m_databaseWriter->asyncPublishModels(models);}
        return CStatusMessageList();
    }

    void CWebDataServices::triggerReadOfDbInfoObjects()
    {
        initDbInfoObjectReaderAndTriggerRead();
    }

    void CWebDataServices::triggerReadOfSharedInfoObjects()
    {
        initSharedInfoObjectReaderAndTriggerRead();
    }

    bool CWebDataServices::canConnectSwiftDb() const
    {
        if (!m_icaoDataReader && !m_modelDataReader && !m_airportDataReader && !m_dbInfoDataReader) { return false; }

        // use the first one to test
        if (m_dbInfoDataReader)
        {
            return m_dbInfoDataReader->hasReceivedOkReply();
        }
        else if (m_icaoDataReader)
        {
            return m_icaoDataReader->hasReceivedOkReply();
        }
        else if (m_modelDataReader)
        {
            return m_modelDataReader->hasReceivedOkReply();
        }
        else if (m_airportDataReader)
        {
            return m_airportDataReader->hasReceivedOkReply();
        }
        return false;
    }

    void CWebDataServices::resetSignalFlags()
    {
        m_signalledEntities.clear();
        m_signalledHeaders = false;
    }

    bool CWebDataServices::hasDbAircraftData() const
    {
        return (this->getModelsCount() > 0) && (this->getLiveriesCount() > 0) && (this->getDistributorsCount() > 0) && (this->getAircraftIcaoCodesCount() > 0);
    }

    void CWebDataServices::admitDbCaches(CEntityFlags::Entity entities)
    {
        // hint: those 2 are currently doing nothing, but this might change in the future
        if (this->m_dbInfoDataReader) { this->m_dbInfoDataReader->admitCaches(entities); }
        if (this->m_sharedInfoDataReader) { this->m_sharedInfoDataReader->admitCaches(entities); }

        // hint: all the readers use own threads
        if (this->m_modelDataReader) { this->m_modelDataReader->admitCaches(entities); }
        if (this->m_icaoDataReader) { this->m_icaoDataReader->admitCaches(entities); }
        if (this->m_airportDataReader) { this->m_airportDataReader->admitCaches(entities); }
    }

    CEntityFlags::Entity CWebDataServices::triggerRead(CEntityFlags::Entity whatToRead, const QDateTime &newerThan)
    {
        m_initialRead = true; // read started
        Q_ASSERT_X(!whatToRead.testFlag(CEntityFlags::DbInfoObjectEntity), Q_FUNC_INFO, "Info object must be read upfront");
        CEntityFlags::Entity triggeredRead = CEntityFlags::NoEntity;
        if (m_vatsimDataFileReader)
        {
            if (whatToRead.testFlag(CEntityFlags::VatsimDataFile))
            {
                m_vatsimDataFileReader->readInBackgroundThread();
                triggeredRead |= CEntityFlags::VatsimDataFile;
            }
        }

        if (m_vatsimBookingReader)
        {
            if (whatToRead.testFlag(CEntityFlags::BookingEntity))
            {
                m_vatsimBookingReader->readInBackgroundThread();
                triggeredRead |= CEntityFlags::BookingEntity;
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
            if (whatToRead.testFlag(CEntityFlags::AircraftIcaoEntity) || whatToRead.testFlag(CEntityFlags::AirlineIcaoEntity) || whatToRead.testFlag(CEntityFlags::CountryEntity))
            {
                CEntityFlags::Entity icaoEntities = whatToRead & CEntityFlags::AllIcaoAndCountries;
                m_icaoDataReader->readInBackgroundThread(icaoEntities, newerThan);
                triggeredRead |= icaoEntities;
            }
        }

        if (m_modelDataReader)
        {
            if (whatToRead.testFlag(CEntityFlags::LiveryEntity) || whatToRead.testFlag(CEntityFlags::DistributorEntity) || whatToRead.testFlag(CEntityFlags::ModelEntity))
            {
                CEntityFlags::Entity modelEntities = whatToRead & CEntityFlags::DistributorLiveryModel;
                m_modelDataReader->readInBackgroundThread(modelEntities, newerThan);
                triggeredRead |= modelEntities;
            }
        }

        return triggeredRead;
    }

    CEntityFlags::Entity CWebDataServices::triggerLoadingDirectlyFromDb(CEntityFlags::Entity whatToRead, const QDateTime &newerThan)
    {
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
                CEntityFlags::Entity icaoEntities = m_icaoDataReader->maskBySupportedEntities(whatToRead);
                m_icaoDataReader->triggerLoadingDirectlyFromDb(icaoEntities, newerThan);
                triggeredRead |= icaoEntities;
            }
        }

        if (m_modelDataReader)
        {
            if (m_modelDataReader->supportsAnyOfEntities(whatToRead))
            {
                CEntityFlags::Entity modelEntities = m_modelDataReader->maskBySupportedEntities(whatToRead);
                m_modelDataReader->triggerLoadingDirectlyFromDb(modelEntities, newerThan);
                triggeredRead |= modelEntities;
            }
        }

        if (m_airportDataReader)
        {
            if (m_airportDataReader->supportsAnyOfEntities(whatToRead))
            {
                CEntityFlags::Entity airportEntities = m_airportDataReader->maskBySupportedEntities(whatToRead);
                m_airportDataReader->triggerLoadingDirectlyFromDb(airportEntities, newerThan);
                triggeredRead |= airportEntities;
            }
        }

        return triggeredRead;
    }

    CEntityFlags::Entity CWebDataServices::triggerLoadingDirectlyFromSharedFiles(CEntityFlags::Entity whatToRead, bool checkCacheTsUpfront)
    {
        CEntityFlags::Entity triggeredRead = CEntityFlags::NoEntity;
        this->triggerReadOfSharedInfoObjects(); // trigger reload of info objects (for shared)

        if (m_icaoDataReader)
        {
            if (m_icaoDataReader->supportsAnyOfEntities(whatToRead))
            {
                CEntityFlags::Entity icaoEntities = m_icaoDataReader->maskBySupportedEntities(whatToRead);
                triggeredRead |= m_icaoDataReader->triggerLoadingDirectlyFromSharedFiles(icaoEntities, checkCacheTsUpfront);
            }
        }

        if (m_modelDataReader)
        {
            if (m_modelDataReader->supportsAnyOfEntities(whatToRead))
            {
                CEntityFlags::Entity modelEntities = m_modelDataReader->maskBySupportedEntities(whatToRead);
                triggeredRead |= m_modelDataReader->triggerLoadingDirectlyFromSharedFiles(modelEntities, checkCacheTsUpfront);
            }
        }

        if (m_airportDataReader)
        {
            if (m_airportDataReader->supportsAnyOfEntities(whatToRead))
            {
                CEntityFlags::Entity airportEntities = m_airportDataReader->maskBySupportedEntities(whatToRead);
                triggeredRead |= m_airportDataReader->triggerLoadingDirectlyFromSharedFiles(airportEntities, checkCacheTsUpfront);
            }
        }

        return triggeredRead;
    }

    QDateTime CWebDataServices::getCacheTimestamp(CEntityFlags::Entity entity) const
    {
        Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "Need single entity");
        if (CEntityFlags::anySwiftDbEntity(entity))
        {
            const CDatabaseReader *dr = this->getDbReader(entity);
            if (!dr) { return QDateTime(); }
            return dr->getCacheTimestamp(entity);
        }
        else
        {
            // non DB entities would go here
            return QDateTime();
        }
    }

    QDateTime CWebDataServices::getLatestDbEntityTimestamp(CEntityFlags::Entity entity) const
    {
        Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "Need single entity");
        if (CEntityFlags::anySwiftDbEntity(entity))
        {
            CInfoDataReader *ir = this->getDbInfoDataReader();
            if (!ir) { return QDateTime(); }
            return ir->getLatestEntityTimestampFromDbInfoObjects(entity);
        }
        else
        {
            return QDateTime();
        }
    }

    QDateTime CWebDataServices::getLatestSharedInfoObjectTimestamp(CEntityFlags::Entity entity) const
    {
        const CDatabaseReader *reader = this->getDbReader(entity);
        if (reader)
        {
            Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "need single entity");
            return reader->getLatestEntityTimestampFromSharedInfoObjects(entity);
        }
        else
        {
            return QDateTime();
        }
    }

    CEntityFlags::Entity CWebDataServices::getEntitiesWithNewerSharedFile(CEntityFlags::Entity entities) const
    {
        Q_ASSERT_X(this->m_sharedInfoDataReader, Q_FUNC_INFO, "Shared info reader was not initialized");
        if (this->m_sharedInfoDataReader->getInfoObjectCount() < 1) { return CEntityFlags::NoEntity; }
        return this->m_sharedInfoDataReader->getEntitesWithNewerSharedInfoObject(entities);
    }

    int CWebDataServices::getCacheCount(CEntityFlags::Entity entity) const
    {
        Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "Need single entity");
        if (CEntityFlags::anySwiftDbEntity(entity))
        {
            CDatabaseReader *dr = this->getDbReader(entity);
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
        if (!this->m_dbInfoDataReader) return -1;
        return this->getInfoObjectCount(entity, this->m_dbInfoDataReader);
    }

    int CWebDataServices::getSharedInfoObjectCount(CEntityFlags::Entity entity) const
    {
        if (!this->m_sharedInfoDataReader) return -1;
        return this->getInfoObjectCount(entity, this->m_sharedInfoDataReader);
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

    CDistributor CWebDataServices::smartDistributorSelector(const CDistributor &distributor) const
    {
        if (m_modelDataReader) { return m_modelDataReader->smartDistributorSelector(distributor); }
        return CDistributor();
    }

    CDistributor CWebDataServices::smartDistributorSelector(const CDistributor &distributor, const CAircraftModel &model) const
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

    QStringList CWebDataServices::getModelStrings() const
    {
        if (m_modelDataReader) { return m_modelDataReader->getModelStringList(); }
        return QStringList();
    }

    QStringList CWebDataServices::getModelCompleterStrings(bool sorted) const
    {
        if (m_modelDataReader) { return m_modelDataReader->getModels().toCompleterStrings(sorted); }
        return QStringList();
    }

    CAircraftModelList CWebDataServices::getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode) const
    {
        if (m_modelDataReader) { return m_modelDataReader->getModelsForAircraftDesignatorAndLiveryCombinedCode(aircraftDesignator, combinedCode); }
        return CAircraftModelList();
    }

    CAircraftModel CWebDataServices::getModelForModelString(const QString &modelString) const
    {
        if (m_modelDataReader) { return m_modelDataReader->getModelForModelString(modelString); }
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

    int CWebDataServices::getAirlineIcaoCodesCount() const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAirlineIcaoCodesCount(); }
        return 0;
    }

    CAirlineIcaoCode CWebDataServices::smartAirlineIcaoSelector(const CAirlineIcaoCode &icaoPattern, const CCallsign &callsign) const
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

    int CWebDataServices::getMetarsCount() const
    {
        if (m_vatsimMetarReader) { return m_vatsimMetarReader->getMetarsCount(); }
        return 0;
    }

    CStatusMessageList CWebDataServices::validateForPublishing(const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels) const
    {
        CStatusMessageList msgs(models.validateForPublishing(validModels, invalidModels));

        // check against existing distributors
        const CDistributorList distributors(this->getDistributors());
        if (!distributors.isEmpty())
        {
            // only further check the valid ones
            CAircraftModelList newValidModels;
            CStatusMessageList msgsDistributos(validModels.validateDistributors(distributors, newValidModels, invalidModels));
            validModels = newValidModels;
            msgs.push_back(msgsDistributos);
        }
        return msgs;
    }

    CAirlineIcaoCodeList CWebDataServices::getAirlineIcaoCodesForDesignator(const QString &designator) const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAirlineIcaoCodesForDesignator(designator); }
        return CAirlineIcaoCodeList();
    }

    void CWebDataServices::gracefulShutdown()
    {
        this->disconnect(); // all signals
        if (this->m_vatsimMetarReader)    { this->m_vatsimMetarReader->gracefulShutdown(); }
        if (this->m_vatsimBookingReader)  { this->m_vatsimBookingReader->gracefulShutdown(); }
        if (this->m_vatsimDataFileReader) { this->m_vatsimDataFileReader->gracefulShutdown(); }
        if (this->m_vatsimStatusReader)   { this->m_vatsimStatusReader->gracefulShutdown(); }
        if (this->m_modelDataReader)      { this->m_modelDataReader->gracefulShutdown(); }
        if (this->m_airportDataReader)    { this->m_airportDataReader->gracefulShutdown(); }
        if (this->m_icaoDataReader)       { this->m_icaoDataReader->gracefulShutdown(); }
        if (this->m_dbInfoDataReader)     { this->m_dbInfoDataReader->gracefulShutdown(); }
        if (this->m_databaseWriter)       { this->m_databaseWriter->gracefulShutdown(); }
    }

    CUrl CWebDataServices::getDbReaderCurrentSharedDbDataUrl() const
    {
        return CDatabaseReader::getCurrentSharedDbDataUrl();
    }

    CEntityFlags::Entity CWebDataServices::allDbEntitiesForUsedReaders() const
    {
        // obtain entities from real readers (means when reader is really used)
        CEntityFlags::Entity entities = CEntityFlags::NoEntity;
        if (this->m_icaoDataReader)
        {
            entities |=  CWebReaderFlags::allEntitiesForReaders(CWebReaderFlags::IcaoDataReader);
        }
        if (this->m_modelDataReader)
        {
            entities |=  CWebReaderFlags::allEntitiesForReaders(CWebReaderFlags::ModelReader);
        }
        if (this->m_airportDataReader)
        {
            entities |=  CWebReaderFlags::allEntitiesForReaders(CWebReaderFlags::AirportReader);
        }

        // when we have a config, we ignore the ones not from cache or DB
        if (!this->m_dbReaderConfig.isEmpty())
        {
            CEntityFlags::Entity configuredEntities = this->m_dbReaderConfig.getEntitesCachedOrReadFromDB();
            entities &= configuredEntities;
        }

        entities &= CEntityFlags::AllDbEntities; // make sure to only use DB data
        return entities;
    }

    const CLogCategoryList &CWebDataServices::getLogCategories()
    {
        static const BlackMisc::CLogCategoryList cats { CLogCategory("swift.datareader"), CLogCategory::webservice() };
        return cats;
    }

    void CWebDataServices::initReaders(CWebReaderFlags::WebReader flags, CEntityFlags::Entity entities)
    {
        //
        // ---- "metadata" reader, 1/2 will trigger read directly during init
        //

        CDatabaseReaderConfigList dbReaderConfig(this->m_dbReaderConfig);
        const bool anyDbEntities = CEntityFlags::anySwiftDbEntity(entities);
        const bool needsSharedInfoObjects = dbReaderConfig.needsSharedInfoObjects(entities);
        const bool needsDbInfoObjects = dbReaderConfig.possiblyReadsFromSwiftDb();
        bool c = false; // for signal connect

        // 1a. If any DB data, read the info objects upfront
        if (needsDbInfoObjects)
        {
            const bool databaseUp = CInfoDataReader::canPingSwiftServer();
            if (!databaseUp) { dbReaderConfig.markAsDbDown(); }

            if (anyDbEntities && flags.testFlag(CWebReaderFlags::WebReaderFlag::DbInfoDataReader))
            {
                // info data reader has a special role, it will not be triggered in triggerRead()
                if (databaseUp)
                {
                    this->initDbInfoObjectReaderAndTriggerRead();
                }
                else
                {
                    CLogMessage(this).warning("DB unreachable, skipping read from info data reader");
                }
            }
        }

        // 1b. Read info objects if needed
        if (needsSharedInfoObjects)
        {
            this->initSharedInfoObjectReaderAndTriggerRead();
        }

        // 2. Status file, updating the VATSIM related caches
        if (flags.testFlag(CWebReaderFlags::VatsimStatusReader) || flags.testFlag(CWebReaderFlags::VatsimDataReader) || flags.testFlag(CWebReaderFlags::VatsimMetarReader))
        {
            this->m_vatsimStatusReader = new CVatsimStatusFileReader(this);
            this->m_vatsimStatusReader->start(QThread::LowPriority);
            // no timer updates from timer here
            QTimer::singleShot(100, this->m_vatsimStatusReader, &CVatsimStatusFileReader::readInBackgroundThread);
        }

        // ---- "normal data", triggerRead will start read, not starting directly

        // 3. VATSIM bookings
        if (flags.testFlag(CWebReaderFlags::WebReaderFlag::VatsimBookingReader))
        {
            this->m_vatsimBookingReader = new CVatsimBookingReader(this);
            c = connect(this->m_vatsimBookingReader, &CVatsimBookingReader::atcBookingsRead, this, &CWebDataServices::ps_receivedBookings);
            Q_ASSERT_X(c, Q_FUNC_INFO, "VATSIM booking reader signals");
            c = connect(this->m_vatsimBookingReader, &CVatsimBookingReader::dataRead, this, &CWebDataServices::dataRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed bookings");
            this->m_entitiesPeriodicallyRead |= CEntityFlags::BookingEntity;
            this->m_vatsimBookingReader->start(QThread::LowPriority);
            this->m_vatsimBookingReader->startReader();
        }

        // 4. VATSIM data file
        if (flags.testFlag(CWebReaderFlags::WebReaderFlag::VatsimDataReader))
        {
            this->m_vatsimDataFileReader = new CVatsimDataFileReader(this);
            c = connect(this->m_vatsimDataFileReader, &CVatsimDataFileReader::dataFileRead, this, &CWebDataServices::ps_vatsimDataFileRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "VATSIM data reader signals");
            c = connect(this->m_vatsimDataFileReader, &CVatsimDataFileReader::dataRead, this, &CWebDataServices::dataRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed VATSIM data file");
            this->m_entitiesPeriodicallyRead |= CEntityFlags::VatsimDataFile;
            this->m_vatsimDataFileReader->start(QThread::LowPriority);
            this->m_vatsimDataFileReader->startReader();
        }

        // 5. VATSIM METAR data
        if (flags.testFlag(CWebReaderFlags::WebReaderFlag::VatsimMetarReader))
        {
            this->m_vatsimMetarReader = new CVatsimMetarReader(this);
            c = connect(this->m_vatsimMetarReader, &CVatsimMetarReader::metarsRead, this, &CWebDataServices::ps_receivedMetars);
            Q_ASSERT_X(c, Q_FUNC_INFO, "VATSIM METAR reader signals");
            c = connect(this->m_vatsimMetarReader, &CVatsimMetarReader::dataRead, this, &CWebDataServices::dataRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed VATSIM METAR");
            this->m_entitiesPeriodicallyRead |= CEntityFlags::MetarEntity;
            this->m_vatsimMetarReader->start(QThread::LowPriority);
            this->m_vatsimMetarReader->startReader();
        }

        // 6. ICAO data reader
        if (flags.testFlag(CWebReaderFlags::WebReaderFlag::IcaoDataReader))
        {
            this->m_icaoDataReader = new CIcaoDataReader(this, dbReaderConfig);
            c = connect(this->m_icaoDataReader, &CIcaoDataReader::dataRead, this, &CWebDataServices::ps_readFromSwiftReader);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect ICAO reader signals");
            c = connect(this->m_icaoDataReader, &CIcaoDataReader::dataRead, this, &CWebDataServices::dataRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect ICAO reader signals");
            this->m_icaoDataReader->start(QThread::LowPriority);
        }

        // 7. Model reader
        if (flags.testFlag(CWebReaderFlags::WebReaderFlag::ModelReader))
        {
            this->m_modelDataReader = new CModelDataReader(this, dbReaderConfig);
            c = connect(this->m_modelDataReader, &CModelDataReader::dataRead, this, &CWebDataServices::ps_readFromSwiftReader);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect Model reader signals");
            c = connect(this->m_modelDataReader, &CModelDataReader::dataRead, this, &CWebDataServices::dataRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect Model reader signals");
            this->m_modelDataReader->start(QThread::LowPriority);
        }

        // 8. Airport list reader
        if (flags.testFlag(CWebReaderFlags::WebReaderFlag::AirportReader))
        {
            this->m_airportDataReader = new CAirportDataReader(this, dbReaderConfig);
            c = connect(this->m_airportDataReader, &CAirportDataReader::dataRead, this, &CWebDataServices::ps_readFromSwiftReader);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect Model reader signals");
            c = connect(this->m_airportDataReader, &CAirportDataReader::dataRead, this, &CWebDataServices::dataRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect Model reader signals");
            this->m_airportDataReader->start(QThread::LowPriority);
        }

        Q_UNUSED(c); // signal connect flag
    }

    void CWebDataServices::initDbInfoObjectReaderAndTriggerRead()
    {
        if (!this->m_dbInfoDataReader)
        {
            this->m_dbInfoDataReader = new CInfoDataReader(this, m_dbReaderConfig, CDbFlags::DbReading);
            bool c = connect(this->m_dbInfoDataReader, &CInfoDataReader::dataRead, this, &CWebDataServices::ps_readFromSwiftReader);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Info reader connect failed");

            // relay signal
            c = connect(this->m_dbInfoDataReader, &CInfoDataReader::dataRead, this, &CWebDataServices::dataRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Info reader connect failed");
            Q_UNUSED(c);

            // start in own thread
            this->m_dbInfoDataReader->start(QThread::LowPriority);
        }

        // and trigger read
        QTimer::singleShot(0, [this]() { this->m_dbInfoDataReader->read(); });
    }

    void CWebDataServices::initSharedInfoObjectReaderAndTriggerRead()
    {
        if (!this->m_sharedInfoDataReader)
        {
            this->m_sharedInfoDataReader = new CInfoDataReader(this, m_dbReaderConfig, CDbFlags::Shared);
            bool c = connect(this->m_sharedInfoDataReader, &CInfoDataReader::dataRead, this, &CWebDataServices::ps_readFromSwiftReader);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Info reader connect failed");

            // relay signal
            c = connect(this->m_sharedInfoDataReader, &CInfoDataReader::dataRead, this, &CWebDataServices::dataRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Info reader connect failed");
            Q_UNUSED(c);

            // start in own thread
            this->m_sharedInfoDataReader->start(QThread::LowPriority);
        }

        // and trigger read
        QTimer::singleShot(0, [this]() { this->m_sharedInfoDataReader->read(); });
    }

    CDatabaseReader *CWebDataServices::getDbReader(CEntityFlags::Entity entity) const
    {
        Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "Need single entity");
        Q_ASSERT_X(CEntityFlags::anySwiftDbEntity(entity), Q_FUNC_INFO, "No swift DB entity");

        const CWebReaderFlags::WebReader wr = CWebReaderFlags::entityToReader(entity);
        switch (wr)
        {
        case CWebReaderFlags::IcaoDataReader: return this->m_icaoDataReader;
        case CWebReaderFlags::ModelReader: return this->m_modelDataReader;
        case CWebReaderFlags::AirportReader: return this->m_airportDataReader;
        default:
            break;
        }
        return nullptr;
    }

    void CWebDataServices::initWriters()
    {
        this->m_databaseWriter = new CDatabaseWriter(
            sApp->getGlobalSetup().getDbRootDirectoryUrl(),
            this);
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

    void CWebDataServices::ps_receivedBookings(const CAtcStationList &stations)
    {
        CLogMessage(this).info("Read %1 ATC bookings from network") << stations.size();
    }

    void CWebDataServices::ps_receivedMetars(const CMetarList &metars)
    {
        CLogMessage(this).info("Read %1 METARs") << metars.size();
    }

    void CWebDataServices::ps_vatsimDataFileRead(int lines)
    {
        CLogMessage(this).info("Read VATSIM data file, %1 lines") << lines;
    }

    void CWebDataServices::ps_readFromSwiftReader(CEntityFlags::Entity entities, CEntityFlags::ReadState state, int number)
    {
        static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::webservice()}));

        if (state == CEntityFlags::StartRead) { return; } // just started
        if (CEntityFlags::isWarningOrAbove(state))
        {
            const CStatusMessage::StatusSeverity severity = CEntityFlags::flagToSeverity(state);
            if (severity == CStatusMessage::SeverityWarning)
            {
                CLogMessage(cats).warning("Read data '%1' entries: %2 state: %3") << CEntityFlags::flagToString(entities) << number << CEntityFlags::flagToString(state);
            }
            else
            {
                CLogMessage(cats).error("Read data '%1' entries: %2 state: %3") << CEntityFlags::flagToString(entities) << number << CEntityFlags::flagToString(state);
            }
        }
        else
        {
            CLogMessage(cats).info("Read data '%1' entries: %2 state: %3") << CEntityFlags::flagToString(entities) << number << CEntityFlags::flagToString(state);
        }

        this->m_swiftDbEntitiesRead |= entities;
        const int allUsedEntities = static_cast<int>(this->allDbEntitiesForUsedReaders());
        if (((static_cast<int>(this->m_swiftDbEntitiesRead)) & allUsedEntities) == allUsedEntities)
        {
            emit allSwiftDbDataRead();
        }

        // individual signals
        if (state == CEntityFlags::ReadFinished || state == CEntityFlags::ReadFinishedRestricted)
        {
            if (entities.testFlag(CEntityFlags::AirportEntity) && signalEntitiesAlreadyRead(CEntityFlags::AirportEntity)) { emit swiftDbAirportsRead(); }
            if (entities.testFlag(CEntityFlags::AirlineIcaoEntity) && signalEntitiesAlreadyRead(CEntityFlags::AirlineIcaoEntity)) { emit swiftDbAirlineIcaoRead(); }
            if (entities.testFlag(CEntityFlags::AircraftIcaoEntity) && signalEntitiesAlreadyRead(CEntityFlags::AircraftIcaoEntity)) { emit swiftDbAircraftIcaoRead(); }
            if (entities.testFlag(CEntityFlags::ModelEntity) && signalEntitiesAlreadyRead(CEntityFlags::ModelEntity)) { emit swiftDbModelsRead(); }
            if (entities.testFlag(CEntityFlags::SharedInfoObjectEntity)) { emit sharedInfoObjectsRead(); }

            if (m_swiftDbEntitiesRead.testFlag(CEntityFlags::AllIcaoEntities) && signalEntitiesAlreadyRead(CEntityFlags::AllIcaoEntities))
            {
                emit swiftDbAllIcaoEntities();
            }
            if (m_swiftDbEntitiesRead.testFlag(CEntityFlags::ModelMatchingEntities) && signalEntitiesAlreadyRead(CEntityFlags::ModelMatchingEntities))
            {
                emit swiftDbModelMatchingEntities();
            }
        }
    }

    void CWebDataServices::readDeferredInBackground(CEntityFlags::Entity entities, int delayMs)
    {
        if (entities == CEntityFlags::NoEntity) { return; }
        QTimer::singleShot(delayMs, [ = ]()
        {
            this->readInBackground(entities);
        });
    }

    void CWebDataServices::readInBackground(CEntityFlags::Entity entities)
    {
        this->m_initialRead = true; // read started
        if (CEntityFlags::anySwiftDbEntity(entities))
        {
            // with info objects wait until info objects are loaded
            Q_ASSERT_X(!entities.testFlag(CEntityFlags::DbInfoObjectEntity), Q_FUNC_INFO, "Info object must be read upfront, do not pass as entity here");
            const bool waitForDbInfoReader = this->m_dbInfoDataReader && !this->m_dbInfoDataReader->areAllDataRead() && !this->m_dbInfoDataReader->isMarkedAsFailed();
            if (waitForDbInfoReader)
            {
                if (!this->waitForDbInfoObjects(entities)) { return; } // will call this function again after some time
            }

            const bool waitForSharedInfoFile = m_dbReaderConfig.needsSharedInfoFileLoaded(entities);
            if (waitForSharedInfoFile)
            {
                if (!this->waitForSharedInfoObjects(entities)) { return; } // will call this function again after some time
            }
        }

        // read entities
        this->triggerRead(entities);
    }

    bool CWebDataServices::waitForDbInfoObjects(CEntityFlags::Entity entities)
    {
        return this->waitForInfoObjects(entities, "DB", this->m_dbInfoDataReader, this->m_dbInfoObjectTrials);
    }

    bool CWebDataServices::waitForSharedInfoObjects(CEntityFlags::Entity entities)
    {
        return this->waitForInfoObjects(entities, "shared", this->m_sharedInfoDataReader, this->m_sharedInfoObjectsTrials);
    }

    bool CWebDataServices::waitForInfoObjects(CEntityFlags::Entity entities, const QString &info, CInfoDataReader *reader, int &trials)
    {
        Q_ASSERT_X(reader, Q_FUNC_INFO, "Need info data reader");

        const int waitForInfoObjectsMs = 1000; // ms
        const int maxWaitCycles = 10;

        // try to read
        if (trials > maxWaitCycles)
        {
            CLogMessage(this).warning("Cannot read %1 info objects for '%2' from '%3' trial %4") << info << CEntityFlags::flagToString(entities) << reader->getInfoObjectsUrl().toQString() << trials;

            // continue here and read data without info objects
            reader->setMarkedAsFailed(true);
            return true; // carry on, regardless of situation
        }
        else if (reader->hasReceivedFirstReply())
        {
            if (reader->areAllDataRead())
            {
                // we have all data and carry on
                CLogMessage(this).info("Info objects (%1) for '%2' loaded (trial %3) from '%4'") << info << CEntityFlags::flagToString(entities) << trials << reader->getInfoObjectsUrl().toQString();
                return true; // no need to wait any longer
            }
            else
            {
                // we have received a response, but not all data yet
                if (reader->hasReceivedOkReply())
                {
                    // ok, this means we are parsing
                    trials++;
                    this->readDeferredInBackground(entities, waitForInfoObjectsMs);
                    return false; // wait
                }
                else
                {
                    // we have a response, but a failure, means server is alive, but responded with error
                    // such an error (access, ...) normally will not go away
                    CLogMessage(this).error("Info objects (%1) loading for '%2' failed from '%3', '%4'") << info << CEntityFlags::flagToString(entities) << reader->getInfoObjectsUrl().toQString() << reader->getStatusMessage();
                    reader->setMarkedAsFailed(true);
                    return true; // carry on, regardless of situation
                }
            }
        }
        else
        {
            // wait for 1st reply
            trials++;
            this->readDeferredInBackground(entities, waitForInfoObjectsMs);
            return false; // wait
        }
    }

    bool CWebDataServices::writeDbDataToDisk(const QString &dir) const
    {
        if (dir.isEmpty()) { return false; }
        QDir directory(dir);
        if (!directory.exists())
        {
            bool s = directory.mkpath(dir);
            if (!s) { return false; }
        }

        if (this->getModelsCount() > 0)
        {
            QString json(QJsonDocument(this->getModels().toJson()).toJson());
            bool s = CFileUtils::writeStringToFileInBackground(json, CFileUtils::appendFilePaths(directory.absolutePath(), "models.json"));
            if (!s) { return false; }
        }

        if (this->getLiveriesCount() > 0)
        {
            QString json(QJsonDocument(this->getLiveries().toJson()).toJson());
            bool s = CFileUtils::writeStringToFileInBackground(json, CFileUtils::appendFilePaths(directory.absolutePath(), "liveries.json"));
            if (!s) { return false; }
        }

        if (this->getAirportsCount() > 0)
        {
            QString json(QJsonDocument(this->getAirports().toJson()).toJson());
            bool s = CFileUtils::writeStringToFileInBackground(json, CFileUtils::appendFilePaths(directory.absolutePath(), "airports.json"));
            if (!s) { return false; }
        }

        return true;
    }

    bool CWebDataServices::readDbDataFromDisk(const QString &dir, bool inBackground)
    {
        if (dir.isEmpty()) { return false; }
        const QDir directory(dir);
        if (!directory.exists()) { return false; }

        bool s = false;
        if (this->m_icaoDataReader)
        {
            if (inBackground) { return this->m_icaoDataReader->readFromJsonFilesInBackground(dir); }
            const CStatusMessageList msgs = this->m_icaoDataReader->readFromJsonFiles(dir);
            if (msgs.isFailure()) { CLogMessage::preformatted(msgs); }
            s = msgs.isSuccess();
        }
        if (s && this->m_modelDataReader)
        {
            if (inBackground) { return this->m_modelDataReader->readFromJsonFilesInBackground(dir); }
            const CStatusMessageList msgs = this->m_modelDataReader->readFromJsonFiles(dir);
            if (msgs.isFailure()) { CLogMessage::preformatted(msgs); }
            s = msgs.isSuccess();
        }
        if (s && this->m_airportDataReader)
        {
            if (inBackground) { return this->m_airportDataReader->readFromJsonFilesInBackground(dir); }
            const CStatusMessageList msgs = this->m_airportDataReader->readFromJsonFiles(dir);
            if (msgs.isFailure()) { CLogMessage::preformatted(msgs); }
            s = msgs.isSuccess();
        }
        return s;
    }
} // ns
