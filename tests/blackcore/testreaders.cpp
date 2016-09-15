/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackcore

#include "testreaders.h"
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
    CTestReaders::CTestReaders(QObject *parent) :
        QObject(parent),
        m_airportReader(new CAirportDataReader(this, CDatabaseReaderConfigList::allDirectDbAccess())),
        m_icaoReader(new CIcaoDataReader(this, CDatabaseReaderConfigList::allDirectDbAccess())),
        m_modelReader(new CModelDataReader(this, CDatabaseReaderConfigList::allDirectDbAccess()))
    { }

    CTestReaders::~CTestReaders()
    {
        this->m_airportReader->gracefulShutdown();
        this->m_icaoReader->gracefulShutdown();
        this->m_modelReader->gracefulShutdown();
    }

    void CTestReaders::readIcaoData()
    {
        const CUrl url(sApp->getGlobalSetup().getDbIcaoReaderUrl());
        if (!this->pingServer(url)) { QSKIP("Server not reachable."); }
        m_icaoReader->start();
        m_icaoReader->readInBackgroundThread(CEntityFlags::AllIcaoEntities, QDateTime());

        // expect does not work here, as I receive multiple signals for all entities and read states
        // I have to run my "own event loop"
        for (int i = 0; i < 60; i++)
        {
            CApplication::processEventsFor(500);
            QTest::qWait(10); // process events completes if there are no events. So I need to make sure we wait properly for a response
            if (this->m_icaoReader->getAircraftIcaoCodesCount() > 0 && this->m_icaoReader->getAirlineIcaoCodesCount() > 0)
            {
                break;
            }
        }

        const QString m1("No aircraft ICAOs " + url.getFullUrl());
        const QString m2("No airline ICAOs " + url.getFullUrl());
        QVERIFY2(this->m_icaoReader->getAircraftIcaoCodesCount() > 0, m1.toLocal8Bit().constData());
        QVERIFY2(this->m_icaoReader->getAirlineIcaoCodesCount() > 0, m2.toLocal8Bit().constData());

        const CAircraftIcaoCode aircraftIcao(this->m_icaoReader->getAircraftIcaoCodes().frontOrDefault());
        const CAirlineIcaoCode airlineIcao(this->m_icaoReader->getAirlineIcaoCodes().frontOrDefault());
        QVERIFY2(aircraftIcao.hasDesignator(), "Missing data for aircraft ICAO");
        QVERIFY2(airlineIcao.hasValidDesignator(), "Missing data for airline ICAO");

        CApplication::processEventsFor(2500); // make sure events are processed
    }

    void CTestReaders::readModelData()
    {
        const CUrl url(sApp->getGlobalSetup().getDbModelReaderUrl());
        if (!this->pingServer(url)) { QSKIP("Server not reachable."); }
        m_modelReader->start();
        m_modelReader->readInBackgroundThread(CEntityFlags::ModelEntity, QDateTime());

        for (int i = 0; i < 120; i++)
        {
            CApplication::processEventsFor(500);
            QTest::qWait(10); // process events completes if there are no events. So I need to make sure we wait properly for a response
            if (this->m_modelReader->getModelsCount() > 0)
            {
                break;
            }
        }

        const QString m1("No models " + url.getFullUrl());
        QVERIFY2(this->m_modelReader->getModelsCount() > 0, m1.toLocal8Bit().constData());

        const CAircraftModel model(m_modelReader->getModels().frontOrDefault());
        QVERIFY2(model.getLivery().hasCompleteData(), "Missing data for livery");

        CApplication::processEventsFor(2500); // make sure events are processed
    }

    void CTestReaders::readAirportData()
    {
        using namespace BlackMisc::Geo;
        using namespace BlackMisc::PhysicalQuantities;

        const CUrl url(sApp->getGlobalSetup().getSwiftAirportUrls().getRandomWorkingUrl());
        if (!this->pingServer(url)) { return; }
        m_airportReader->start();
        m_airportReader->readInBackgroundThread(CEntityFlags::AirportEntity, QDateTime());

        for (int i = 0; i < 120; ++i)
        {
            CApplication::processEventsFor(500);
            QTest::qWait(10); // process events completes if there are no events. So I need to make sure we wait properly for a response
            if (this->m_airportReader->getAirports().size() > 0) break;
        }

        QVERIFY2(this->m_airportReader->getAirports().size() > 0, "No airports");
        auto heathrow = this->m_airportReader->getAirports().findByIcao("EGLL");
        QVERIFY2(heathrow.size() == 1, "No Heathrow");

        auto airports = m_airportReader->getAirports();
        airports.sortByRange(CCoordinateGeodetic(CLatitude(51.5085300, CAngleUnit::deg()), CLongitude(-0.1257400, CAngleUnit::deg()), CLength()), true);
        qDebug() << airports[0].getIcao() << airports[1].getIcao();
        QVERIFY2(airports[0].getIcao() == CAirportIcaoCode("EGLW"), "Wrong airport data");

        CApplication::processEventsFor(2500); // make sure events are processed
    }

    bool CTestReaders::pingServer(const CUrl &url)
    {
        QString m;
        if (!CNetworkUtils::canConnect(url, m, 2500))
        {
            qWarning() << "Skipping unit test as" << url.getFullUrl() << "cannot be connected";
            return false;
        }
        return true;
    }
} // ns

//! \endcond
