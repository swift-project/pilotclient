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

        Q_ASSERT_X(QSslSocket::supportsSsl(), Q_FUNC_INFO, "missing SSL support");
        Q_ASSERT_X(sApp->isSetupAvailable(), Q_FUNC_INFO, "Setup not syncronized");
        this->setObjectName("CWebDataReader");

        // check if I need info objects
        const bool readFromSwiftDb = dbReaderConfig.possiblyReadsFromSwiftDb(); // only cached?
        if (!readFromSwiftDb && readers.testFlag(CWebReaderFlags::InfoDataReader))
        {
            // will remove info reader becaue not needed
            readers &= ~CWebReaderFlags::InfoDataReader;
            this->m_readers = readers;
            CLogMessage(this).info("Remove info object reader because not needed");
        }

        // get entities to be read
        CEntityFlags::Entity entities = CWebReaderFlags::allEntitiesForReaders(readers);
        if (entities.testFlag(CEntityFlags::InfoObjectEntity))
        {
            Q_ASSERT_X(readers.testFlag(CWebReaderFlags::InfoDataReader), Q_FUNC_INFO, "info object but no reader");
            CLogMessage(this).info("Using info objects for swift DB objects");
        }

        this->initReaders(readers); // reads info object if required
        this->initWriters();

        // make sure this is called in event queue, so pending tasks cam be performed
        // important so info objects can be read
        entities &= ~CEntityFlags::InfoObjectEntity;
        this->singleShotReadInBackground(entities, 1000);
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

    bool CWebDataServices::canConnectSwiftDb() const
    {
        if (!m_icaoDataReader && !m_modelDataReader && !m_infoDataReader) { return false; }

        // use the first one to test
        if (m_infoDataReader)
        {
            return m_infoDataReader->canConnect();
        }
        else if (m_icaoDataReader)
        {
            return m_icaoDataReader->canConnect();
        }
        else if (m_modelDataReader)
        {
            return m_modelDataReader->canConnect();
        }
        return false;
    }

    CEntityFlags::Entity CWebDataServices::triggerRead(CEntityFlags::Entity whatToRead, const QDateTime &newerThan)
    {
        m_initialRead = true; // read started
        Q_ASSERT_X(!whatToRead.testFlag(CEntityFlags::InfoObjectEntity), Q_FUNC_INFO, "Info object must be read upfront");
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

    CLivery CWebDataServices::getStdLiveryForAirlineCode(const CAirlineIcaoCode &icao) const
    {
        if (m_modelDataReader) { return m_modelDataReader->getStdLiveryForAirlineCode(icao); }
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

    QList<int> CWebDataServices::getModelDbKeys() const
    {
        if (m_modelDataReader) { return m_modelDataReader->getModelDbKeys(); }
        return QList<int>();
    }

    QStringList CWebDataServices::getModelStrings() const
    {
        if (m_modelDataReader) { return m_modelDataReader->getModelStrings(); }
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

    CAirlineIcaoCode CWebDataServices::smartAirlineIcaoSelector(const CAirlineIcaoCode &icaoPattern) const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->smartAirlineIcaoSelector(icaoPattern); }
        return CAirlineIcaoCode();
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

    CCountry CWebDataServices::getCountryForIsoCode(const QString &iso) const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getCountryForIsoCode(iso); }
        return CCountry();
    }

    CMetarSet CWebDataServices::getMetars() const
    {
        if (m_vatsimMetarReader) { return m_vatsimMetarReader->getMetars(); }
        return CMetarSet();
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

    CAirlineIcaoCodeList CWebDataServices::getAirlineIcaoCodesForDesignator(const QString &designator) const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAirlineIcaoCodesForDesignator(designator); }
        return CAirlineIcaoCodeList();
    }

    void CWebDataServices::gracefulShutdown()
    {
        this->disconnect(); // all signals
        if (this->m_vatsimStatusReader)   { this->m_vatsimStatusReader->gracefulShutdown(); }
        if (this->m_vatsimBookingReader)  { this->m_vatsimBookingReader->gracefulShutdown(); }
        if (this->m_vatsimDataFileReader) { this->m_vatsimDataFileReader->gracefulShutdown(); }
        if (this->m_vatsimStatusReader)   { this->m_vatsimStatusReader->gracefulShutdown(); }
        if (this->m_vatsimMetarReader)    { this->m_vatsimMetarReader->gracefulShutdown(); }
        if (this->m_modelDataReader)      { this->m_modelDataReader->gracefulShutdown(); }
        if (this->m_icaoDataReader)       { this->m_icaoDataReader->gracefulShutdown(); }
        if (this->m_infoDataReader)       { this->m_infoDataReader->gracefulShutdown(); }
        if (this->m_databaseWriter)       { this->m_databaseWriter->gracefulShutdown(); }
    }

    const CLogCategoryList &CWebDataServices::getLogCategories()
    {
        static const BlackMisc::CLogCategoryList cats { CLogCategory("swift.datareader"), CLogCategory::webservice() };
        return cats;
    }

    void CWebDataServices::initReaders(CWebReaderFlags::WebReader flags)
    {
        // ---- "metadata" reader, 1/2 will trigger read directly during init

        // 1. If any DB data, read the info upfront
        const bool anyDbData = flags.testFlag(CWebReaderFlags::WebReaderFlag::IcaoDataReader) || flags.testFlag(CWebReaderFlags::WebReaderFlag::ModelReader);
        const CDatabaseReaderConfigList dbReaderConfig(this->m_dbReaderConfig);
        bool c = false; // signal connect
        Q_UNUSED(c);

        if (anyDbData && flags.testFlag(CWebReaderFlags::WebReaderFlag::InfoDataReader))
        {
            this->m_infoDataReader = new CInfoDataReader(this, dbReaderConfig);
            c = connect(this->m_infoDataReader, &CInfoDataReader::dataRead, this, &CWebDataServices::ps_readFromSwiftDb);
            Q_ASSERT_X(c, Q_FUNC_INFO, "ICAO info object signals");
            c = connect(this->m_infoDataReader, &CInfoDataReader::dataRead, this, &CWebDataServices::dataRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed info data");

            // info data reader has a special role, it will not be triggered in triggerRead()
            this->m_infoDataReader->start(QThread::LowPriority);
            // directly call read
            QTimer::singleShot(0, [this]() { this->m_infoDataReader->read(CEntityFlags::InfoObjectEntity, QDateTime()); });
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
            this->m_vatsimBookingReader->start(QThread::LowPriority);
            this->m_vatsimBookingReader->setIntervalFromSettingsAndStart();
        }

        // 4. VATSIM data file
        if (flags.testFlag(CWebReaderFlags::WebReaderFlag::VatsimDataReader))
        {
            this->m_vatsimDataFileReader = new CVatsimDataFileReader(this);
            c = connect(this->m_vatsimDataFileReader, &CVatsimDataFileReader::dataFileRead, this, &CWebDataServices::ps_dataFileRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "VATSIM data reader signals");
            c = connect(this->m_vatsimDataFileReader, &CVatsimDataFileReader::dataRead, this, &CWebDataServices::dataRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed VATSIM data file");
            this->m_vatsimDataFileReader->start(QThread::LowPriority);
            this->m_vatsimDataFileReader->setIntervalFromSettingsAndStart();
        }

        // 5. VATSIM metar data
        if (flags.testFlag(CWebReaderFlags::WebReaderFlag::VatsimMetarReader))
        {
            this->m_vatsimMetarReader = new CVatsimMetarReader(this);
            c = connect(this->m_vatsimMetarReader, &CVatsimMetarReader::metarsRead, this, &CWebDataServices::ps_receivedMetars);
            Q_ASSERT_X(c, Q_FUNC_INFO, "VATSIM METAR reader signals");
            c = connect(this->m_vatsimMetarReader, &CVatsimMetarReader::dataRead, this, &CWebDataServices::dataRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed VATSIM METAR");
            this->m_vatsimMetarReader->start(QThread::LowPriority);
            this->m_vatsimMetarReader->setIntervalFromSettingsAndStart();
        }

        // 6. ICAO data reader
        if (flags.testFlag(CWebReaderFlags::WebReaderFlag::IcaoDataReader))
        {
            this->m_icaoDataReader = new CIcaoDataReader(this, dbReaderConfig);
            c = connect(this->m_icaoDataReader, &CIcaoDataReader::dataRead, this, &CWebDataServices::ps_readFromSwiftDb);
            Q_ASSERT_X(c, Q_FUNC_INFO, "ICAO reader signals");
            c = connect(this->m_icaoDataReader, &CIcaoDataReader::dataRead, this, &CWebDataServices::dataRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect ICAO reader signals");
            this->m_icaoDataReader->start(QThread::LowPriority);
        }

        // 7. Model reader
        if (flags.testFlag(CWebReaderFlags::WebReaderFlag::ModelReader))
        {
            this->m_modelDataReader = new CModelDataReader(this, dbReaderConfig);
            c = connect(this->m_modelDataReader, &CModelDataReader::dataRead, this, &CWebDataServices::ps_readFromSwiftDb);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Model reader signals");
            c = connect(this->m_modelDataReader, &CModelDataReader::dataRead, this, &CWebDataServices::dataRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "connect failed models");
            this->m_modelDataReader->start(QThread::LowPriority);
        }
    }

    void CWebDataServices::initWriters()
    {
        this->m_databaseWriter = new CDatabaseWriter(
            sApp->getGlobalSetup().getDbRootDirectoryUrl(),
            this);
    }

    void CWebDataServices::ps_receivedBookings(const CAtcStationList &stations)
    {
        CLogMessage(this).info("Read %1 ATC bookings from network") << stations.size();
    }

    void CWebDataServices::ps_receivedMetars(const CMetarSet &metars)
    {
        CLogMessage(this).info("Read %1 METARs") << metars.size();
    }

    void CWebDataServices::ps_dataFileRead(int lines)
    {
        CLogMessage(this).info("Read VATSIM data file, %1 lines") << lines;
    }

    void CWebDataServices::ps_readFromSwiftDb(CEntityFlags::Entity entity, CEntityFlags::ReadState state, int number)
    {
        static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::webservice()}));
        if (CEntityFlags::isWarningOrAbove(state))
        {
            CStatusMessage::StatusSeverity severity = CEntityFlags::flagToSeverity(state);
            if (severity == CStatusMessage::SeverityWarning)
            {
                CLogMessage(cats).warning("Read data %1 entries: %2 state: %3") << CEntityFlags::flagToString(entity) << number << CEntityFlags::flagToString(state);
            }
            else
            {
                CLogMessage(cats).error("Read data %1 entries: %2 state: %3") << CEntityFlags::flagToString(entity) << number << CEntityFlags::flagToString(state);
            }
        }
        else
        {
            CLogMessage(cats).info("Read data %1 entries: %2 state: %3") << CEntityFlags::flagToString(entity) << number << CEntityFlags::flagToString(state);
        }
    }

    void CWebDataServices::ps_setupChanged()
    {
        // void
    }

    void CWebDataServices::singleShotReadInBackground(CEntityFlags::Entity entities, int delayMs)
    {
        QTimer::singleShot(delayMs, [ = ]()
        {
            this->readInBackground(entities, delayMs);
        });
    }

    void CWebDataServices::readInBackground(CEntityFlags::Entity entities, int delayMs)
    {
        m_initialRead = true; // read started

        const int waitForInfoObjects = 1000; // ms
        const int maxWaitCycles = 6;

        // with info objects wait until info objects are loaded
        Q_ASSERT_X(!entities.testFlag(CEntityFlags::InfoObjectEntity), Q_FUNC_INFO, "Info object must be read upfront");
        if (this->m_infoDataReader && CEntityFlags::anySwiftDbEntity(entities))
        {
            // try to read
            if (this->m_infoObjectTrials > maxWaitCycles)
            {
                CLogMessage(this).error("Cannot read info objects from %1") << this->m_infoDataReader->getInfoObjectsUrl().toQString();
            }
            else if (this->m_infoDataReader->canConnect())
            {
                // read, but no idea if succesful/failure
                if (this->m_infoDataReader->getDbInfoObjectCount() > 0)
                {
                    CLogMessage(this).info("Info objects loaded from %1") << this->m_infoDataReader->getInfoObjectsUrl().toQString();
                }
                else
                {
                    CLogMessage(this).error("Info objects loading failed from %1, '%2'")
                            << this->m_infoDataReader->getInfoObjectsUrl().toQString()
                            << this->m_infoDataReader->getStatusMessage();
                }
            }
            else
            {
                // postpone by some time
                this->m_infoObjectTrials++;
                this->singleShotReadInBackground(entities, waitForInfoObjects);
                return;
            }
        }

        if (delayMs > 100)
        {
            this->singleShotReadInBackground(entities, 0);
        }
        else
        {
            this->triggerRead(entities);
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

        if (m_icaoDataReader)
        {
            bool s = m_icaoDataReader->writeToJsonFiles(directory.absolutePath());
            if (!s) { return false; }
        }
        if (m_modelDataReader)
        {
            bool s = m_modelDataReader->writeToJsonFiles(directory.absolutePath());
            if (!s) { return false; }
        }

        return true;
    }

    bool CWebDataServices::readDbDataFromDisk(const QString &dir, bool inBackground)
    {
        if (dir.isEmpty()) { return false; }
        QDir directory(dir);
        if (!directory.exists()) { return false; }

        bool s = false;
        if (this->m_icaoDataReader)
        {
            s = inBackground ?
                this->m_icaoDataReader->readFromJsonFilesInBackground(dir) :
                this->m_icaoDataReader->readFromJsonFiles(dir);
        }
        if (s && this->m_modelDataReader)
        {
            s = inBackground ?
                this->m_modelDataReader->readFromJsonFilesInBackground(dir) :
                this->m_modelDataReader->readFromJsonFiles(dir);
        }
        return s;
    }
} // ns
