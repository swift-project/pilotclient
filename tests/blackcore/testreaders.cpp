/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testreaders.h"
#include "expect.h"
#include "blackcore/global_reader_settings.h"
#include "blackmisc/networkutils.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackCoreTest
{

    CTestReaders::CTestReaders(QObject *parent) :
        QObject(parent),
        m_icaoReader(this,
                     CGlobalReaderSettings::instance().protocolIcaoReader(),
                     CGlobalReaderSettings::instance().serverIcaoReader(),
                     CGlobalReaderSettings::instance().baseUrlIcaoReader()
                    ),
        m_modelReader(this,
                      CGlobalReaderSettings::instance().protocolModelReader(),
                      CGlobalReaderSettings::instance().serverModelReader(),
                      CGlobalReaderSettings::instance().baseUrlModelReader()
                     )
    { }

    void CTestReaders::readIcaoData()
    {
        QString server(CGlobalReaderSettings::instance().serverIcaoReader());
        if (!pingServer(server)) { return; }
        m_icaoReader.start();
        Expect e(&this->m_icaoReader);
        EXPECT_UNIT(e)
        .send(&CIcaoDataReader::readInBackgroundThread)
        .expect(&CIcaoDataReader::readAll, [server]()
        {
            qDebug() << "Read ICAO data from" << server;
        })
        .wait(10);

        QVERIFY2(this->m_icaoReader.getAircraftIcaoCodesCount() > 0, "No aircraft ICAOs");
        QVERIFY2(this->m_icaoReader.getAirlineIcaoCodesCount() > 0, "No airline ICAOs");

        CAircraftIcaoCode aircraftIcao(this->m_icaoReader.getAircraftIcaoCodes().front());
        CAirlineIcaoCode airlineIcao(this->m_icaoReader.getAirlineIcaoCodes().front());
        QVERIFY2(aircraftIcao.hasCompleteData(), "Missing data for aircraft ICAO");
        QVERIFY2(airlineIcao.hasCompleteData(), "Missing data for airline ICAO");
    }

    void CTestReaders::readModelData()
    {
        QString server(CGlobalReaderSettings::instance().serverModelReader());
        if (!pingServer(server)) { return; }
        m_modelReader.start();
        Expect e(&this->m_modelReader);
        EXPECT_UNIT(e)
        .send(&CModelDataReader::readInBackgroundThread)
        .expect(&CModelDataReader::readAll, [server]()
        {
            qDebug() << "Read model data from" << server;
        })
        .wait(10);

        QVERIFY2(this->m_modelReader.getDistributorsCount() > 0, "No distributors");
        QVERIFY2(this->m_modelReader.getLiveriesCount() > 0, "No liveries");

        CLivery livery(this->m_modelReader.getLiveries().front());
        CDistributor distributor(this->m_modelReader.getDistributors().front());
        QVERIFY2(livery.hasCompleteData(), "Missing data for livery");
        QVERIFY2(distributor.hasCompleteData(), "Missing data for distributor");
    }

    bool CTestReaders::pingServer(const QString &server)
    {
        QString m;
        if (!CNetworkUtils::canConnect(server, 80, m, 2500))
        {
            qWarning() << "Skipping unit test as" << server << "cannot be connected";
            return false;
        }
        return true;
    }
} // ns
