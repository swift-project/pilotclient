/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackcore/modeldatareader.h"
#include "blackcore/icaodatareader.h"
#include "blackcore/databasewriter.h"
#include "blackcore/vatsimbookingreader.h"
#include "blackcore/vatsimdatafilereader.h"
#include "blackcore/vatsimmetarreader.h"
#include "settings/global_reader_settings.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/worker.h"

using namespace BlackCore::Settings;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Weather;

namespace BlackCore
{
    CWebDataServices::CWebDataServices(CWebReaderFlags::WebReader readerFlags, QObject *parent) :
        QObject(parent), m_readerFlags(readerFlags)
    {
        this->setObjectName("CWebDataReader");
        this->initReaders(readerFlags);
        this->initWriters();
    }

    QList<QMetaObject::Connection> CWebDataServices::connectVatsimDataSignals(QObject *receiver,
            std::function<void(int)> bookingsRead,
            std::function<void(int)> dataFileRead,
            std::function<void(int)> metarsRead)
    {
        // bind does not allow to define connection type
        // so anything in its own thread will be sent with this thread affinity
        Q_ASSERT_X(receiver, Q_FUNC_INFO, "Missing receiver");
        QList<QMetaObject::Connection> cl;

        if (m_readerFlags.testFlag(CWebReaderFlags::WebReaderFlags::VatsimBookingReader))
        {
            QMetaObject::Connection c1 = connect(this, &CWebDataServices::vatsimBookingsRead, receiver, bookingsRead);
            Q_ASSERT_X(c1, Q_FUNC_INFO, "connect failed (bookings)");
            cl.append(c1);
        }

        if (m_readerFlags.testFlag(CWebReaderFlags::WebReaderFlags::VatsimDataReader))
        {
            QMetaObject::Connection c2 = connect(this, &CWebDataServices::vatsimDataFileRead, receiver, dataFileRead);
            Q_ASSERT_X(c2, Q_FUNC_INFO, "connect failed (data)");
            cl.append(c2);
        }

        if (m_readerFlags.testFlag(CWebReaderFlags::WebReaderFlags::VatsimMetarReader))
        {
            QMetaObject::Connection c3 = connect(this, &CWebDataServices::vatsimMetarsRead, receiver, metarsRead);
            Q_ASSERT_X(c3, Q_FUNC_INFO, "connect failed (METARs)");
            cl.append(c3);
        }
        return cl;
    }

    QList<QMetaObject::Connection> CWebDataServices::connectSwiftDatabaseSignals(QObject *receiver, std::function<void(CDbFlags::Entity, CDbFlags::ReadState, int)> dataRead)
    {
        Q_ASSERT_X(receiver, Q_FUNC_INFO, "Missing receiver");

        // bind does not allow to define connection type
        // so anything in its own thread will be sent with this thread affinity
        QList<QMetaObject::Connection> cl;

        if (m_readerFlags.testFlag(CWebReaderFlags::WebReaderFlags::IcaoDataReader))
        {
            Q_ASSERT_X(this->m_icaoDataReader, Q_FUNC_INFO, "Missing reader ICAO");
            QMetaObject::Connection con = connect(this->m_icaoDataReader, &CIcaoDataReader::readData, receiver, dataRead);
            Q_ASSERT_X(con, Q_FUNC_INFO, "connect failed ICAO");
            cl.append(con);
        }

        if (m_readerFlags.testFlag(CWebReaderFlags::WebReaderFlags::ModelReader))
        {
            Q_ASSERT_X(this->m_modelDataReader, Q_FUNC_INFO, "Missing reader models");
            QMetaObject::Connection con = connect(this->m_modelDataReader, &CModelDataReader::readData, receiver, dataRead);
            Q_ASSERT_X(con, Q_FUNC_INFO, "connect failed models");
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
        if (m_icaoDataReader)
        {
            if (!m_icaoDataReader->canConnect()) { return false; }
        }
        if (m_modelDataReader)
        {
            if (!m_modelDataReader->canConnect()) { return false; }
        }
        return true;
    }

    CDbFlags::Entity CWebDataServices::triggerRead(CDbFlags::Entity whatToRead)
    {
        CDbFlags::Entity triggeredRead = CDbFlags::NoEntity;
        if (m_vatsimDataFileReader)
        {
            if (whatToRead.testFlag(CDbFlags::VatsimDataFile))
            {
                m_vatsimDataFileReader->readInBackgroundThread();
                triggeredRead |= CDbFlags::VatsimDataFile;
            }
        }

        if (m_vatsimBookingReader)
        {
            if (whatToRead.testFlag(CDbFlags::VatsimBookings))
            {
                m_vatsimBookingReader->readInBackgroundThread();
                triggeredRead |= CDbFlags::VatsimBookings;
            }
        }

        if (m_icaoDataReader)
        {
            if (whatToRead.testFlag(CDbFlags::AircraftIcaoEntity) || whatToRead.testFlag(CDbFlags::AirlineIcaoEntity) || whatToRead.testFlag(CDbFlags::CountryEntity))
            {
                CDbFlags::Entity icaoEntities = whatToRead & CDbFlags::AllIcaoAndCountries;
                m_icaoDataReader->readInBackgroundThread(icaoEntities);
                triggeredRead |= icaoEntities;
            }
        }

        if (m_modelDataReader)
        {
            if (whatToRead.testFlag(CDbFlags::LiveryEntity) || whatToRead.testFlag(CDbFlags::DistributorEntity) || whatToRead.testFlag(CDbFlags::ModelEntity))
            {
                CDbFlags::Entity modelEntities = whatToRead & CDbFlags::DistributorLiveryModel;
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
        if (this->m_modelDataReader) { this->m_modelDataReader->gracefulShutdown(); }
        if (this->m_icaoDataReader) { this->m_icaoDataReader->gracefulShutdown(); }
        if (this->m_databaseWriter) { this->m_databaseWriter->gracefulShutdown(); }
    }

    const CLogCategoryList &CWebDataServices::getLogCategories()
    {
        static const BlackMisc::CLogCategoryList cats { CLogCategory("swift.datareader") };
        return cats;
    }

    void CWebDataServices::initReaders(CWebReaderFlags::WebReader flags)
    {
        // 1. VATSIM bookings
        if (flags.testFlag(CWebReaderFlags::WebReaderFlags::VatsimBookingReader))
        {
            this->m_vatsimBookingReader = new CVatsimBookingReader(this, CGlobalReaderSettings::instance().bookingsUrl());
            bool c = connect(this->m_vatsimBookingReader, &CVatsimBookingReader::dataRead, this, &CWebDataServices::ps_receivedBookings);
            Q_ASSERT_X(c, Q_FUNC_INFO, "VATSIM booking reader signals");
            Q_UNUSED(c);
            this->m_vatsimBookingReader->start(QThread::LowPriority);
            this->m_vatsimBookingReader->setInterval(3 * 60 * 1000);
        }

        // 2. VATSIM data file
        if (flags.testFlag(CWebReaderFlags::WebReaderFlags::VatsimDataReader))
        {
            this->m_vatsimDataFileReader = new CVatsimDataFileReader(this, CGlobalReaderSettings::instance().vatsimDataFileUrls());
            bool c = connect(this->m_vatsimDataFileReader, &CVatsimDataFileReader::dataRead, this, &CWebDataServices::ps_dataFileRead);
            Q_ASSERT_X(c, Q_FUNC_INFO, "VATSIM data reader signals");
            Q_UNUSED(c);
            this->m_vatsimDataFileReader->start(QThread::LowPriority);
            this->m_vatsimDataFileReader->setInterval(90 * 1000);
        }

        // 3. VATSIM metar data
        if (flags.testFlag(CWebReaderFlags::WebReaderFlags::VatsimMetarReader))
        {
            this->m_vatsimMetarReader = new CVatsimMetarReader(this, CGlobalReaderSettings::instance().urlVatsimMetars());
            bool c = connect(this->m_vatsimMetarReader, &CVatsimMetarReader::dataRead, this, &CWebDataServices::ps_receivedMetars);
            Q_ASSERT_X(c, Q_FUNC_INFO, "VATSIM METAR reader signals");
            Q_UNUSED(c);
            this->m_vatsimMetarReader->start(QThread::LowPriority);
            this->m_vatsimMetarReader->setInterval(90 * 1000);
        }

        // 4. ICAO data reader
        if (flags.testFlag(CWebReaderFlags::WebReaderFlags::IcaoDataReader))
        {
            bool c;
            this->m_icaoDataReader = new CIcaoDataReader(this, CGlobalReaderSettings::instance().protocolIcaoReader(), CGlobalReaderSettings::instance().serverIcaoReader(), CGlobalReaderSettings::instance().baseUrlIcaoReader());
            c = connect(this->m_icaoDataReader, &CIcaoDataReader::readData, this, &CWebDataServices::ps_readFromSwiftDb);
            Q_ASSERT_X(c, Q_FUNC_INFO, "ICAO reader signals");
            Q_UNUSED(c);
            this->m_icaoDataReader->start();
        }

        // 5. Model reader
        if (flags.testFlag(CWebReaderFlags::WebReaderFlags::ModelReader))
        {
            this->m_modelDataReader = new CModelDataReader(this, CGlobalReaderSettings::instance().protocolModelReader(), CGlobalReaderSettings::instance().serverModelReader(), CGlobalReaderSettings::instance().baseUrlModelReader());
            bool c = connect(this->m_modelDataReader, &CModelDataReader::readData, this, &CWebDataServices::ps_readFromSwiftDb);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Model reader signals");
            Q_UNUSED(c);
            this->m_modelDataReader->start();
        }
    }

    void CWebDataServices::initWriters()
    {
        this->m_databaseWriter = new CDatabaseWriter(
            CGlobalReaderSettings::instance().protocolModelReader(),
            CGlobalReaderSettings::instance().serverModelReader(),
            CGlobalReaderSettings::instance().baseUrlModelReader(),
            this);
    }

    void CWebDataServices::ps_receivedBookings(const CAtcStationList &stations)
    {
        CLogMessage(this).info("Read %1 ATC bookings from network") << stations.size();
        emit vatsimBookingsRead(stations.size());
    }

    void CWebDataServices::ps_receivedMetars(const CMetarSet &metars)
    {
        CLogMessage(this).info("Read %1 METARs") << metars.size();
        emit vatsimDataFileRead(metars.size());
    }

    void CWebDataServices::ps_dataFileRead(int lines)
    {
        CLogMessage(this).info("Read VATSIM data file, %1 lines") << lines;
        emit vatsimDataFileRead(lines);
    }

    void CWebDataServices::ps_readFromSwiftDb(CDbFlags::Entity entity, CDbFlags::ReadState state, int number)
    {
        CLogMessage(this).info("Read data %1 %3 %2") << CDbFlags::flagToString(entity) << number << CDbFlags::flagToString(state);
        emit readSwiftDbData(entity, state, number);
    }

    void CWebDataServices::readAllInBackground(int delayMs)
    {
        if (delayMs > 100)
        {
            BlackMisc::singleShot(delayMs, QThread::currentThread(), [ = ]()
            {
                this->readAllInBackground(0);
            });
        }
        else
        {
            // only readers requested will be read
            if (this->m_vatsimBookingReader) {this->m_vatsimBookingReader->readInBackgroundThread(); }
            if (this->m_vatsimDataFileReader) this->m_vatsimDataFileReader->readInBackgroundThread();
            if (this->m_icaoDataReader) { this->m_icaoDataReader->readInBackgroundThread(CDbFlags::AllIcaoAndCountries); }
            if (this->m_modelDataReader) { this->m_modelDataReader->readInBackgroundThread(CDbFlags::DistributorLiveryModel); }
        }
    }

    void CWebDataServices::readAtcBookingsInBackground() const
    {
        if (!this->m_vatsimBookingReader) { return; }
        this->m_vatsimBookingReader->readInBackgroundThread();
    }

} // ns
