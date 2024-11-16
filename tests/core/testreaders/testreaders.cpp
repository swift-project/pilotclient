// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testswiftcore

#include <QDateTime>
#include <QDebug>
#include <QString>
#include <QTest>
#include <QtDebug>

#include "test.h"

#include "core/application.h"
#include "core/data/globalsetup.h"
#include "core/db/airportdatareader.h"
#include "core/db/icaodatareader.h"
#include "core/db/modeldatareader.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/aircrafticaocodelist.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/airlineicaocodelist.h"
#include "misc/aviation/livery.h"
#include "misc/network/entityflags.h"
#include "misc/network/networkutils.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/aircraftmodellist.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::db;
using namespace swift::misc::simulation;
using namespace swift::misc::network;
using namespace swift::core;
using namespace swift::core::db;

namespace swiftcoretest
{
    //! Test data readers (for ICAOs, JSON, etc.)
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
        swift::core::db::CAirportDataReader *m_airportReader = nullptr;
        swift::core::db::CIcaoDataReader *m_icaoReader = nullptr;
        swift::core::db::CModelDataReader *m_modelReader = nullptr;

        //! Test if server is available
        static bool connectServer(const swift::misc::network::CUrl &url);
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
            if (this->m_icaoReader->getAircraftIcaoCodesCount() > 0 &&
                this->m_icaoReader->getAirlineIcaoCodesCount() > 0)
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

        if (!aircraftIcaosInvalid.isEmpty()) { qDebug() << aircraftIcaosInvalid.toQString(); }
        if (!airlineIcaosInvalid.isEmpty()) { qDebug() << airlineIcaosInvalid.toQString(); }

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
        using namespace swift::misc::geo;
        using namespace swift::misc::physical_quantities;

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
        airports.sortByRange(CCoordinateGeodetic(CLatitude(51.5085300, CAngleUnit::deg()),
                                                 CLongitude(-0.1257400, CAngleUnit::deg()), CAltitude()),
                             true);
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
} // namespace swiftcoretest

//! main
SWIFTTEST_APPLESS_MAIN(swiftcoretest::CTestReaders);

#include "testreaders.moc"

//! \endcond
