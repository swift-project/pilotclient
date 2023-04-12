/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackcore

#include "blackcore/application.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/db/airportdatareader.h"
#include "blackcore/db/icaodatareader.h"
#include "blackcore/db/modeldatareader.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "test.h"

#include <QDateTime>
#include <QDebug>
#include <QTest>
#include <QString>
#include <QtDebug>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Db;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackCore;
using namespace BlackCore::Db;

namespace BlackCoreTest
{
    //! Test data readers (for bookings, JSON, etc.)
    class CTestReaders : public QObject
    {
        Q_OBJECT

    private slots:
        void initTestCase();

        //! Read ICAO data
        void readIcaoData();

        //! Read model data
        void readModelData();

        //! Read airport data
        void readAirportData();

        void cleanupTestCase();

    private:
        BlackCore::Db::CAirportDataReader *m_airportReader = nullptr;
        BlackCore::Db::CIcaoDataReader *m_icaoReader = nullptr;
        BlackCore::Db::CModelDataReader *m_modelReader = nullptr;

        //! Test if server is available
        static bool connectServer(const BlackMisc::Network::CUrl &url);
    };

    void CTestReaders::initTestCase()
    {
        m_airportReader = new CAirportDataReader(this, CDatabaseReaderConfigList::allDirectDbAccess());
        m_icaoReader = new CIcaoDataReader(this, CDatabaseReaderConfigList::allDirectDbAccess());
        m_modelReader = new CModelDataReader(this, CDatabaseReaderConfigList::allDirectDbAccess());
        m_airportReader->markAsUsedInUnitTest();
        m_icaoReader->markAsUsedInUnitTest();
        m_modelReader->markAsUsedInUnitTest();
    }

    void CTestReaders::readIcaoData()
    {
        const CUrl url(sApp->getGlobalSetup().getDbIcaoReaderUrl());
        qDebug() << "Reader URL" << url.toQString();
        if (!this->connectServer(url))
        {
            QSKIP("Server not reachable.");
            return;
        }
        m_icaoReader->start();
        m_icaoReader->readInBackgroundThread(CEntityFlags::AllIcaoEntities, QDateTime());

        // expect does not work here, as I receive multiple signals for all entities and read states
        // I have to run my "own event loop"
        for (int i = 0; i < 60; i++)
        {
            CApplication::processEventsFor(500); // process events and sleep
            if (this->m_icaoReader->getAircraftIcaoCodesCount() > 0 && this->m_icaoReader->getAirlineIcaoCodesCount() > 0)
            {
                break;
            }
        }

        const QString m1("No aircraft ICAOs " + url.getFullUrl());
        const QString m2("No airline ICAOs " + url.getFullUrl());
        QVERIFY2(this->m_icaoReader->getAircraftIcaoCodesCount() > 0, qUtf8Printable(m1));
        QVERIFY2(this->m_icaoReader->getAirlineIcaoCodesCount() > 0, qUtf8Printable(m2));

        // reader set to ignore incomplete ICAO data, so sizes must match
        const CAircraftIcaoCodeList aircraftIcaos = this->m_icaoReader->getAircraftIcaoCodes();
        const CAirlineIcaoCodeList airlineIcaos = this->m_icaoReader->getAirlineIcaoCodes();
        const CAircraftIcaoCodeList aircraftIcaosValid(aircraftIcaos.findByValidDesignator());
        const CAirlineIcaoCodeList airlineIcaosValid(airlineIcaos.findByValidDesignator());
        const CAircraftIcaoCodeList aircraftIcaosInvalid(aircraftIcaos.findByInvalidDesignator());
        const CAirlineIcaoCodeList airlineIcaosInvalid(airlineIcaos.findByInvalidDesignator());
        const int aircraftValidSize = aircraftIcaosValid.size();
        const int aircraftSize = aircraftIcaos.size();
        const int airlineValidSize = airlineIcaosValid.size();
        const int airlineSize = airlineIcaos.size();

        if (!aircraftIcaosInvalid.isEmpty())
        {
            qDebug() << aircraftIcaosInvalid.toQString();
        }
        if (!airlineIcaosInvalid.isEmpty())
        {
            qDebug() << airlineIcaosInvalid.toQString();
        }

        QVERIFY2(aircraftValidSize == aircraftSize, "All aircraft ICAOs must be valid");
        QVERIFY2(airlineValidSize == airlineSize, "Some airline ICAOs must be valid");

        CApplication::processEventsFor(2500); // make sure events are processed
    }

    void CTestReaders::readModelData()
    {
        const CUrl url(sApp->getGlobalSetup().getDbModelReaderUrl());
        qDebug() << "Reader URL" << url.toQString();
        if (!this->connectServer(url))
        {
            QSKIP("Server not reachable.");
            return;
        }
        m_modelReader->start();
        m_modelReader->readInBackgroundThread(CEntityFlags::ModelEntity, QDateTime());

        for (int i = 0; i < 120; i++)
        {
            CApplication::processEventsFor(500);
            if (this->m_modelReader->getModelsCount() > 0) { break; }
        }

        const QString m1("No models " + url.getFullUrl());
        QVERIFY2(this->m_modelReader->getModelsCount() > 0, qUtf8Printable(m1));

        const CAircraftModel model(m_modelReader->getModels().frontOrDefault());
        QVERIFY2(model.getLivery().hasCompleteData(), "Missing data for livery");

        CApplication::processEventsFor(2500); // make sure events are processed
    }

    void CTestReaders::readAirportData()
    {
        using namespace BlackMisc::Geo;
        using namespace BlackMisc::PhysicalQuantities;

        const CUrl url(sApp->getGlobalSetup().getDbAirportReaderUrl());
        qDebug() << "Reader URL" << url.toQString();
        if (!this->connectServer(url))
        {
            QSKIP("Server not reachable.");
            return;
        }
        m_airportReader->start();
        m_airportReader->readInBackgroundThread(CEntityFlags::AirportEntity, QDateTime());

        for (int i = 0; i < 120; ++i)
        {
            CApplication::processEventsFor(500);
            if (this->m_airportReader->getAirports().size() > 0) { break; }
        }

        QVERIFY2(this->m_airportReader->getAirports().size() > 0, "No airports");
        const auto heathrow = this->m_airportReader->getAirports().findByIcao("EGLL");
        QVERIFY2(heathrow.size() == 1, "No Heathrow");
        QVERIFY2(heathrow[0].isOperating(), "Wrong airport data");

        auto airports = m_airportReader->getAirports();
        airports.sortByRange(CCoordinateGeodetic(CLatitude(51.5085300, CAngleUnit::deg()), CLongitude(-0.1257400, CAngleUnit::deg()), CAltitude()), true);
        QVERIFY2(airports[0].getIcao() == CAirportIcaoCode("EGLW"), "Wrong airport data");

        CApplication::processEventsFor(2500); // make sure events are processed
    }

    bool CTestReaders::connectServer(const CUrl &url)
    {
        QString m;
        if (CNetworkUtils::canConnect(url, m, 2500))
        {
            qDebug() << "URL" << url.getFullUrl() << "connected";
            return true;
        }
        else
        {
            qWarning() << "URL" << url.getFullUrl() << "cannot be connected";
            return false;
        }
    }

    void CTestReaders::cleanupTestCase()
    {
        m_airportReader->quitAndWait();
        m_icaoReader->quitAndWait();
        m_modelReader->quitAndWait();
    }
} // ns

//! main
BLACKTEST_APPLESS_MAIN(BlackCoreTest::CTestReaders);

#include "testreaders.moc"

//! \endcond
