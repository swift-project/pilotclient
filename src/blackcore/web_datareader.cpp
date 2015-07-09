/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/web_datareader.h"
#include "vatsimbookingreader.h"
#include "vatsimdatafilereader.h"
#include "icaodatareader.h"
#include "modeldatareader.h"
#include "reader_settings.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/worker.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackCore
{

    CWebDataReader::CWebDataReader(WebReader readerFlags, QObject *parent) :
        QObject(parent), m_readerFlags(readerFlags)
    {
        this->setObjectName("CWebDataReader");
        this->initReaders(readerFlags);
    }

    QList<QMetaObject::Connection> CWebDataReader::connectVatsimDataSignals(std::function<void(int)> bookingsRead, std::function<void(int)> dataFileRead)
    {
        // bind does not allow to define connection type
        // so anything in its own thread will be sent with this thread affinity
        QList<QMetaObject::Connection> cl;

        if (m_readerFlags.testFlag(VatsimBookingReader))
        {
            QMetaObject::Connection c1 = connect(this, &CWebDataReader::vatsimBookingsRead, bookingsRead);
            Q_ASSERT_X(c1, Q_FUNC_INFO, "connect failed");
            cl.append(c1);
        }

        if (m_readerFlags.testFlag(VatsimDataReader))
        {
            QMetaObject::Connection c2 = connect(this, &CWebDataReader::vatsimDataFileRead, dataFileRead);
            Q_ASSERT_X(c2, Q_FUNC_INFO, "connect failed");
            cl.append(c2);
        }
        return cl;
    }

    QList<QMetaObject::Connection> CWebDataReader::connectSwiftDatabaseSignals(
        QObject *receiver,
        std::function<void(int)> aircraftIcaoCodeRead, std::function<void(int)> airlineIcaoCodeRead,
        std::function<void(int)> liveriesRead, std::function<void(int)> distributorsRead,
        std::function<void(int)> modelsRead)
    {
        Q_ASSERT_X(receiver, Q_FUNC_INFO, "Missing receiver");

        // bind does not allow to define connection type
        // so anything in its own thread will be sent with this thread affinity
        QList<QMetaObject::Connection> cl;

        if (m_readerFlags.testFlag(IcaoDataReader))
        {
            QMetaObject::Connection c1 = connect(this, &CWebDataReader::aircraftIcaoCodeRead, receiver, aircraftIcaoCodeRead);
            Q_ASSERT_X(c1, Q_FUNC_INFO, "connect failed");
            cl.append(c1);
            QMetaObject::Connection c2 = connect(this, &CWebDataReader::airlineIcaoCodeRead, receiver, airlineIcaoCodeRead);
            Q_ASSERT_X(c2, Q_FUNC_INFO, "connect failed");
            cl.append(c2);
        }

        if (m_readerFlags.testFlag(ModelReader))
        {
            QMetaObject::Connection c1 = connect(this, &CWebDataReader::liveriesRead, receiver, liveriesRead);
            Q_ASSERT_X(c1, Q_FUNC_INFO, "connect failed");
            cl.append(c1);
            QMetaObject::Connection c2 = connect(this, &CWebDataReader::distributorsRead, receiver, distributorsRead);
            Q_ASSERT_X(c2, Q_FUNC_INFO, "connect failed");
            cl.append(c2);
            QMetaObject::Connection c3 = connect(this, &CWebDataReader::modelsRead, receiver, modelsRead);
            Q_ASSERT_X(c3, Q_FUNC_INFO, "connect failed");
            cl.append(c3);
        }
        return cl;
    }

    CServerList CWebDataReader::getVatsimFsdServers() const
    {
        if (m_vatsimDataFileReader) { return m_vatsimDataFileReader->getFsdServers(); }
        return CServerList();
    }

    CServerList CWebDataReader::getVatsimVoiceServers() const
    {
        if (m_vatsimDataFileReader) { return m_vatsimDataFileReader->getVoiceServers(); }
        return CServerList();
    }

    CDistributorList CWebDataReader::getDistributors() const
    {
        if (m_modelDataReader) { return m_modelDataReader->getDistributors(); }
        return CDistributorList();
    }

    CLiveryList CWebDataReader::getLiveries() const
    {
        if (m_modelDataReader) { return m_modelDataReader->getLiveries(); }
        return CLiveryList();
    }

    CAircraftModelList CWebDataReader::getModels() const
    {
        if (m_modelDataReader) { return m_modelDataReader->getModels(); }
        return CAircraftModelList();
    }

    CAircraftIcaoCodeList CWebDataReader::getAircraftIcaoCodes() const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAircraftIcaoCodes(); }
        return CAircraftIcaoCodeList();
    }

    CAirlineIcaoCodeList CWebDataReader::getAirlineIcaoCodes() const
    {
        if (m_icaoDataReader) { return m_icaoDataReader->getAirlineIcaoCodes(); }
        return CAirlineIcaoCodeList();
    }

    void CWebDataReader::gracefulShutdown()
    {
        this->disconnect(); // all signals
        if (this->m_vatsimBookingReader)  { this->m_vatsimBookingReader->requestStop();  this->m_vatsimBookingReader->quit(); }
        if (this->m_vatsimDataFileReader) { this->m_vatsimDataFileReader->requestStop(); this->m_vatsimDataFileReader->quit(); }
    }

    const CLogCategoryList &CWebDataReader::getLogCategories()
    {
        static const BlackMisc::CLogCategoryList cats { CLogCategory("swift.datareader") };
        return cats;
    }

    void CWebDataReader::initReaders(WebReader flags)
    {
        // 1. VATSIM bookings
        if (flags.testFlag(VatsimBookingReader))
        {
            this->m_vatsimBookingReader = new CVatsimBookingReader(this, CReaderSettings::instance().bookingsUrl());
            connect(this->m_vatsimBookingReader, &CVatsimBookingReader::dataRead, this, &CWebDataReader::ps_receivedBookings);
            this->m_vatsimBookingReader->start();
            this->m_vatsimBookingReader->setInterval(3 * 60 * 1000);
        }

        // 2. VATSIM data file
        if (flags.testFlag(VatsimDataReader))
        {
            this->m_vatsimDataFileReader = new CVatsimDataFileReader(this, CReaderSettings::instance().vatsimDataFileUrls());
            connect(this->m_vatsimDataFileReader, &CVatsimDataFileReader::dataRead, this, &CWebDataReader::ps_dataFileRead);
            this->m_vatsimDataFileReader->start();
            this->m_vatsimDataFileReader->setInterval(90 * 1000);
        }

        // 3. ICAO data reader
        if (flags.testFlag(IcaoDataReader))
        {
            this->m_icaoDataReader = new CIcaoDataReader(this, CReaderSettings::instance().protocolIcaoReader(), CReaderSettings::instance().serverIcaoReader(), CReaderSettings::instance().baseUrlIcaoReader());
            connect(this->m_icaoDataReader, &CIcaoDataReader::readAircraftIcaoCodes, this, &CWebDataReader::ps_readAircraftIcaoCodes);
            connect(this->m_icaoDataReader, &CIcaoDataReader::readAirlinesIcaoCodes, this, &CWebDataReader::ps_readAirlinesIcaoCodes);
            this->m_icaoDataReader->start();
        }

        // 4. Model reader
        if (flags.testFlag(ModelReader))
        {
            this->m_modelDataReader = new CModelDataReader(this, CReaderSettings::instance().protocolModelReader(), CReaderSettings::instance().serverModelReader(), CReaderSettings::instance().baseUrlModelReader());
            connect(this->m_modelDataReader, &CModelDataReader::readLiveries, this, &CWebDataReader::ps_readLiveries);
            connect(this->m_modelDataReader, &CModelDataReader::readDistributors, this, &CWebDataReader::ps_readDistributors);
            connect(this->m_modelDataReader, &CModelDataReader::readModels, this, &CWebDataReader::ps_readModels);
            this->m_modelDataReader->start();
        }
    }

    void CWebDataReader::ps_receivedBookings(const CAtcStationList &stations)
    {
        CLogMessage(this).info("Read bookings from network");
        emit vatsimBookingsRead(stations.size());
    }

    void CWebDataReader::ps_dataFileRead(int lines)
    {
        CLogMessage(this).info("Read VATSIM data file, %1 lines") << lines;
        emit vatsimDataFileRead(lines);
    }

    void CWebDataReader::ps_readAircraftIcaoCodes(int number)
    {
        CLogMessage(this).info("Read %1 aircraft ICAO codes") << number;
        emit aircraftIcaoCodeRead(number);
    }

    void CWebDataReader::ps_readAirlinesIcaoCodes(int number)
    {
        CLogMessage(this).info("Read %1 airline ICAO codes") << number;
        emit airlineIcaoCodeRead(number);
    }

    void CWebDataReader::ps_readLiveries(int number)
    {
        CLogMessage(this).info("Read %1 liveries") << number;
        emit liveriesRead(number);
    }

    void CWebDataReader::ps_readDistributors(int number)
    {
        CLogMessage(this).info("Read %1 distributors") << number;
        emit distributorsRead(number);
    }

    void CWebDataReader::ps_readModels(int number)
    {
        CLogMessage(this).info("Read %1 models") << number;
        emit modelsRead(number);
    }

    void CWebDataReader::readAllInBackground(int delayMs)
    {
        if (delayMs > 100)
        {
            BlackMisc::singleShot(delayMs, QThread::currentThread(), [ = ]()
            {
                this->readAllInBackground(0);
            });
            return;
        }

        if (this->m_vatsimBookingReader) {this->m_vatsimBookingReader->readInBackgroundThread(); }
        if (this->m_vatsimDataFileReader) this->m_vatsimDataFileReader->readInBackgroundThread();
        if (this->m_icaoDataReader) { this->m_icaoDataReader->readInBackgroundThread(); }
        if (this->m_modelDataReader) { this->m_modelDataReader->readInBackgroundThread(); }
    }

    void CWebDataReader::readAtcBookingsInBackground() const
    {
        if (!this->m_vatsimBookingReader) { return; }
        this->m_vatsimBookingReader->readInBackgroundThread();
    }

    void CWebDataReader::readModelDataInBackground() const
    {
        if (!this->m_modelDataReader) { return; }
        this->m_modelDataReader->readInBackgroundThread();
    }

} // ns
