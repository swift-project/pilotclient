/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/setupreader.h"
#include "blackcore/webdataservices.h"
#include "blackcore/modeldatareader.h"
#include "blackcore/icaodatareader.h"
#include "blackcore/databasewriter.h"
#include "blackcore/vatsimbookingreader.h"
#include "blackcore/vatsimdatafilereader.h"
#include "blackcore/vatsimmetarreader.h"
#include "data/globalsetup.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/worker.h"
#include "blackmisc/json.h"
#include <QJsonObject>
#include <QJsonDocument>

using namespace BlackCore;
using namespace BlackCore::Data;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Weather;

namespace BlackCore
{
    CWebDataServices::CWebDataServices(
        CWebReaderFlags::WebReader readerFlags, int autoReadAfterSetupSynchronized, QObject *parent) :
        QObject(parent), m_readerFlags(readerFlags), m_autoReadAfterSetupMs(autoReadAfterSetupSynchronized)
    {
        this->setObjectName("CWebDataReader");
        connect(&CSetupReader::instance(), &CSetupReader::setupSynchronized, this, &CWebDataServices::ps_setupRead);
        this->initReaders(readerFlags);
        this->initWriters();
    }

    QList<QMetaObject::Connection> CWebDataServices::connectDataReadSignal(QObject *receiver, std::function<void(CEntityFlags::Entity, CEntityFlags::ReadState, int)> dataRead)
    {
        Q_ASSERT_X(receiver, Q_FUNC_INFO, "Missing receiver");

        // bind does not allow to define connection type
        // so anything in its own thread will be sent with this thread affinity
        QList<QMetaObject::Connection> cl;

        if (m_readerFlags.testFlag(CWebReaderFlags::WebReaderFlag::IcaoDataReader))
        {
            Q_ASSERT_X(this->m_icaoDataReader, Q_FUNC_INFO, "Missing reader ICAO");
            QMetaObject::Connection con = connect(this->m_icaoDataReader, &CIcaoDataReader::dataRead, receiver, dataRead);
            Q_ASSERT_X(con, Q_FUNC_INFO, "connect failed ICAO");
            cl.append(con);
        }

        if (m_readerFlags.testFlag(CWebReaderFlags::WebReaderFlag::ModelReader))
        {
            Q_ASSERT_X(this->m_modelDataReader, Q_FUNC_INFO, "Missing reader models");
            QMetaObject::Connection con = connect(this->m_modelDataReader, &CModelDataReader::dataRead, receiver, dataRead);
            Q_ASSERT_X(con, Q_FUNC_INFO, "connect failed models");
            cl.append(con);
        }

        if (m_readerFlags.testFlag(CWebReaderFlags::WebReaderFlag::VatsimBookingReader))
        {
            Q_ASSERT_X(this->m_vatsimBookingReader, Q_FUNC_INFO, "Missing reader bookings");
            QMetaObject::Connection con = connect(this->m_vatsimBookingReader, &CVatsimBookingReader::dataRead, receiver, dataRead);
            Q_ASSERT_X(con, Q_FUNC_INFO, "connect failed bookings");
            cl.append(con);
        }

        if (m_readerFlags.testFlag(CWebReaderFlags::WebReaderFlag::VatsimDataReader))
        {
            Q_ASSERT_X(this->m_vatsimDataFileReader, Q_FUNC_INFO, "Missing reader data file");
            QMetaObject::Connection con = connect(this->m_vatsimDataFileReader, &CVatsimDataFileReader::dataRead, receiver, dataRead);
            Q_ASSERT_X(con, Q_FUNC_INFO, "connect failed VATSIM data file");
            cl.append(con);
        }

        if (m_readerFlags.testFlag(CWebReaderFlags::WebReaderFlag::VatsimMetarReader))
        {
            Q_ASSERT_X(this->m_vatsimMetarReader, Q_FUNC_INFO, "Missing reader metars");
            QMetaObject::Connection con = connect(this->m_vatsimMetarReader, &CVatsimMetarReader::dataRead, receiver, dataRead);
            Q_ASSERT_X(con, Q_FUNC_INFO, "connect failed VATSIM METAR");
            cl.append(con);
        }
        return cl;
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

    CStatusMessageList CWebDataServices::asyncWriteModel(CAircraftModel &model) const
    {
        if (m_databaseWriter) { return m_databaseWriter->asyncWriteModel(model);}
        return CStatusMessageList();
    }

    bool CWebDataServices::canConnectSwiftDb() const
    {
        if (!m_icaoDataReader && !m_modelDataReader) { return false; }

        // use the first one to test
        if (m_icaoDataReader)
        {
            return m_icaoDataReader->canConnect();
        }
        else if (m_modelDataReader)
        {
            return m_modelDataReader->canConnect();
        }
        return false;
    }

    CEntityFlags::Entity CWebDataServices::triggerRead(CEntityFlags::Entity whatToRead)
    {
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
                m_icaoDataReader->readInBackgroundThread(icaoEntities);
                triggeredRead |= icaoEntities;
            }
        }

        if (m_modelDataReader)
        {
            if (whatToRead.testFlag(CEntityFlags::LiveryEntity) || whatToRead.testFlag(CEntityFlags::DistributorEntity) || whatToRead.testFlag(CEntityFlags::ModelEntity))
            {
                CEntityFlags::Entity modelEntities = whatToRead & CEntityFlags::DistributorLiveryModel;
                m_modelDataReader->readInBackgroundThread(modelEntities);
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

    CAirlineIcaoCode CWebDataServices::getAirlineIcaoCodeForDesignator(const QString &designator) const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAirlineIcaoCodeForDesignator(designator); }
        return CAirlineIcaoCode();
    }

    void CWebDataServices::gracefulShutdown()
    {
        this->disconnect(); // all signals
        if (this->m_vatsimBookingReader)  { this->m_vatsimBookingReader->gracefulShutdown(); }
        if (this->m_vatsimDataFileReader) { this->m_vatsimDataFileReader->gracefulShutdown(); }
        if (this->m_vatsimMetarReader)    { this->m_vatsimMetarReader->gracefulShutdown(); }
        if (this->m_modelDataReader)      { this->m_modelDataReader->gracefulShutdown(); }
        if (this->m_icaoDataReader)       { this->m_icaoDataReader->gracefulShutdown(); }
        if (this->m_databaseWriter)       { this->m_databaseWriter->gracefulShutdown(); }
    }

    const CLogCategoryList &CWebDataServices::getLogCategories()
    {
        static const BlackMisc::CLogCategoryList cats { CLogCategory("swift.datareader") };
        return cats;
    }

    void CWebDataServices::initReaders(CWebReaderFlags::WebReader flags)
    {
        // 1. VATSIM bookings
        if (flags.testFlag(CWebReaderFlags::WebReaderFlag::VatsimBookingReader))
        {
            this->m_vatsimBookingReader = new CVatsimBookingReader(this);
            bool c = connect(this->m_vatsimBookingReader, &CVatsimBookingReader::atcBookingsRead, this, &CWebDataServices::ps_receivedBookings);
            Q_ASSERT_X(c, Q_FUNC_INFO, "VATSIM booking reader signals");
            Q_UNUSED(c);
            this->m_vatsimBookingReader->start(QThread::LowPriority);
            this->m_vatsimBookingReader->setInterval(3 * 60 * 1000);
        }

        // 2. VATSIM data file
        if (flags.testFlag(CWebReaderFlags::WebReaderFlag::VatsimDataReader))
        {
            this->m_vatsimDataFileReader = new CVatsimDataFileReader(this);
            bool c = connect(this->m_vatsimDataFileReader, &CVatsimDataFileReader::dataFileRead, this, &CWebDataServices::ps_dataFileRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "VATSIM data reader signals");
            Q_UNUSED(c);
            this->m_vatsimDataFileReader->start(QThread::LowPriority);
            this->m_vatsimDataFileReader->setInterval(90 * 1000);
        }

        // 3. VATSIM metar data
        if (flags.testFlag(CWebReaderFlags::WebReaderFlag::VatsimMetarReader))
        {
            this->m_vatsimMetarReader = new CVatsimMetarReader(this);
            bool c = connect(this->m_vatsimMetarReader, &CVatsimMetarReader::metarsRead, this, &CWebDataServices::ps_receivedMetars);
            Q_ASSERT_X(c, Q_FUNC_INFO, "VATSIM METAR reader signals");
            Q_UNUSED(c);
            this->m_vatsimMetarReader->start(QThread::LowPriority);
            this->m_vatsimMetarReader->setInterval(90 * 1000);
        }

        // 4. ICAO data reader
        if (flags.testFlag(CWebReaderFlags::WebReaderFlag::IcaoDataReader))
        {
            bool c;
            this->m_icaoDataReader = new CIcaoDataReader(this);
            c = connect(this->m_icaoDataReader, &CIcaoDataReader::dataRead, this, &CWebDataServices::ps_readFromSwiftDb);
            Q_ASSERT_X(c, Q_FUNC_INFO, "ICAO reader signals");
            Q_UNUSED(c);
            this->m_icaoDataReader->start(QThread::LowPriority);
        }

        // 5. Model reader
        if (flags.testFlag(CWebReaderFlags::WebReaderFlag::ModelReader))
        {
            this->m_modelDataReader = new CModelDataReader(this);
            bool c = connect(this->m_modelDataReader, &CModelDataReader::dataRead, this, &CWebDataServices::ps_readFromSwiftDb);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Model reader signals");
            Q_UNUSED(c);
            this->m_modelDataReader->start(QThread::LowPriority);
        }
    }

    void CWebDataServices::initWriters()
    {
        this->m_databaseWriter = new CDatabaseWriter(
            m_setup.get().dbModelReaderUrl(),
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
        if (CEntityFlags::isWarningOrAbove(state))
        {
            CLogMessage(this).warning("Read data %1 entries: %2 state: %3") << CEntityFlags::flagToString(entity) << number << CEntityFlags::flagToString(state);
        }
        else
        {
            CLogMessage(this).info("Read data %1 entries: %2 state: %3") << CEntityFlags::flagToString(entity) << number << CEntityFlags::flagToString(state);
        }
    }

    void CWebDataServices::ps_setupRead(bool success)
    {
        // setup has been changed
        if (success)
        {
            if (m_autoReadAfterSetupMs >= 0)
            {
                CLogMessage(this).info("Setup synchronized, will trigger read of web service data");
                this->readInBackground(CEntityFlags::AllEntities, m_autoReadAfterSetupMs);
            }
        }
        else
        {
            CLogMessage(this).error("Failed to read setup, will not(!) triggrr web data read");
        }
    }

    void CWebDataServices::ps_setupChanged()
    {
        CLogMessage(this).debug() << "Setup changed";
    }

    void CWebDataServices::readInBackground(CEntityFlags::Entity entities, int delayMs)
    {
        if (delayMs > 100)
        {
            BlackMisc::singleShot(delayMs, QThread::currentThread(), [ = ]()
            {
                this->readInBackground(entities, 0);
            });
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
