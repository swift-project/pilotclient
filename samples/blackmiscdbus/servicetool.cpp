/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmiscdbus

#include "servicetool.h"
#include "blackcore/application.h"
#include "blackmisc/test/testservice.h"
#include "blackmisc/test/testserviceinterface.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/network/server.h"
#include "blackmisc/test/testdata.h"

#include <stdio.h>
#include <stdlib.h>
#include <QChar>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusError>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusPendingReply>
#include <QDBusReply>
#include <QDBusVariant>
#include <QDateTime>
#include <QDebug>
#include <QElapsedTimer>
#include <QFlags>
#include <QLatin1Char>
#include <QList>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QTextStream>
#include <QThread>
#include <QVariant>
#include <QtDebug>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Math;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Test;
using namespace BlackCore;

namespace BlackSample
{
    void ServiceTool::dataTransferTestServer(CDBusServer *dBusServer, bool verbose)
    {
        QDBusConnection sessionBusConnection = QDBusConnection::sessionBus();
        if (sessionBusConnection.interface()->isServiceRegistered(CTestService::InterfaceName()))
        {
            qFatal("Testservice already registed on session bus");
        }

        // as this is the receiver side, the slots can be debugged too
        CTestService *testService = CTestService::registerTestService(sessionBusConnection, verbose, QCoreApplication::instance());
        dBusServer->addObject(CTestService::ObjectPath(), testService);
    }

    void ServiceTool::dataTransferTestClient(const QString &address)
    {
        // send data as P2P to server (this can be session bus, too, but usually is P2P)
        const bool sb = address.startsWith("session", Qt::CaseInsensitive);
        QDBusConnection connection = sb ?
                                         QDBusConnection::sessionBus() :
                                         QDBusConnection::connectToPeer(address, "p2pConnection");

        // qtout << "server connection has interface? " << connection.interface(); // returns 0 with server and a real interface with session bus
        // qtout << "address: " << address;
        // qtout << "name: " << connection.name();

        ServiceTool::sendDataToTestservice(connection);
    }

    void ServiceTool::sendDataToTestservice(const QDBusConnection &connection)
    {
        // on the client's side
        ITestServiceInterface testServiceInterface(CTestService::InterfaceName(), CTestService::ObjectPath(), connection);
        QTextStream qtin(stdin);
        QTextStream qtout(stdout);

        const QList<double> list { 1.0, 2.0, 3.0 };

        while (true)
        {
            QDBusMessage m = QDBusMessage::createSignal(
                CTestService::ObjectPath(), CTestService::InterfaceName(),
                "sendStringMessage");

            // The << operator is used to add the parameters for the slot
            const QDateTime dtnow = QDateTime::currentDateTimeUtc();
            const QString msg = QStringLiteral("Con.: %1, message at %2").arg(connection.name(), dtnow.toString("MM/dd/yyyy @ hh:mm:ss"));
            m << msg;

            // We send this as a non-replying message. This is used for sending errors, replys, signals.
            // Values can be seen on the receiver side
            qtout << "----------------- receiver tests ----------------" << Qt::endl;

            // Low level test
            if (connection.send(m)) { qtout << "Send via low level method " << Qt::endl; }

            // same as interface message
            // but call the slot
            testServiceInterface.receiveStringMessage(msg);
            qtout << "Send string via interface " << msg << Qt::endl;

            // a list
            testServiceInterface.receiveList(list);
            qtout << "Send list via interface " << list.size() << Qt::endl;

            // PQs
            CSpeed speed(200, CSpeedUnit::km_h());
            const CSpeed speedNull(0, nullptr);

            testServiceInterface.receiveSpeed(speed);
            qtout << "Send speed via interface " << speed << Qt::endl;
            testServiceInterface.receiveSpeed(speedNull);
            qtout << "Send null speed via interface " << speedNull << Qt::endl;
            speed.switchUnit(CSpeedUnit::kts());
            testServiceInterface.receiveSpeed(speed);
            qtout << "Send speed via interface " << speed << Qt::endl;
            speed.switchUnit(CSpeedUnit::km_h());
            speed.addValueSameUnit(1.0);

            // Network
            const CServer trafficServer = CTestData::getTrafficServer();
            QVariant tsqv = QVariant::fromValue(trafficServer);
            QDBusVariant tsv(tsqv);
            testServiceInterface.receiveVariant(tsv, tsqv.userType());
            qtout << "Send server via interface and variant '" << trafficServer << QLatin1String("' ") << tsqv.userType() << Qt::endl;

            // Aviation
            const CComSystem comSystem = CComSystem("DBUS COM1", CPhysicalQuantitiesConstants::FrequencyInternationalAirDistress(), CPhysicalQuantitiesConstants::FrequencyUnicom());
            testServiceInterface.receiveComUnit(comSystem);
            qtout << "Send COM via interface " << comSystem << Qt::endl;

            CAltitude altitude(1000, CAltitude::MeanSeaLevel, CLengthUnit::ft());
            QVariant qvAl = QVariant::fromValue(altitude);
            QDBusVariant qv(qvAl);
            testServiceInterface.receiveVariant(qv, qvAl.userType());
            testServiceInterface.receiveAltitude(altitude);
            qtout << "Send altitude via interface and variant " << altitude << qvAl.userType() << Qt::endl;
            altitude.addValueSameUnit(1);

            const CTransponder transponder(7000, CTransponder::ModeC);
            testServiceInterface.receiveTransponder(transponder);
            qtout << "Send transponder via interface " << transponder << Qt::endl;

            const CTrack track(123.45, CTrack::Magnetic, CAngleUnit::deg());
            testServiceInterface.receiveTrack(track);
            qtout << "Send track via interface " << track << Qt::endl;

            const CLength len(33, CLengthUnit::m());
            testServiceInterface.receiveLength(len);
            qtout << "Send length via interface " << len << Qt::endl;

            const CAltitude alt(44, CAltitude::MeanSeaLevel, CLengthUnit::m());
            testServiceInterface.receiveLength(alt);
            qtout << "Send altitude via interface " << alt << Qt::endl;

            const CCallsign callsign = CTestData::getRandomPilotCallsign();
            testServiceInterface.receiveCallsign(callsign);
            qtout << "Send callsign via interface " << callsign << Qt::endl;

            const CAtcStation station = CTestData::getMunichTower();
            testServiceInterface.receiveAtcStation(station);
            qtout << "Send ATC " << station << Qt::endl;

            // Geo
            const CCoordinateGeodetic geoPos = CTestData::getCoordinateFrankfurtTower();
            testServiceInterface.receiveGeoPosition(geoPos);
            qtout << "Send geo position " << geoPos << Qt::endl;
            CApplication::processEventsFor(1000);

            qtout << "----------------- variant tests ----------------" << Qt::endl;
            const CVariantList cvList = CTestData::getCVariantList();
            testServiceInterface.receiveVariantList(cvList);
            qtout << "Send " << cvList.size() << " variants via interface as CVariantList" << Qt::endl;

            const CPropertyIndexVariantMap valueMap = CTestData::getCPropertyIndexVariantMap();
            testServiceInterface.receiveValueMap(valueMap);
            qtout << "Send " << valueMap.size() << " index variant map entries" << Qt::endl;
            CApplication::processEventsFor(1000);

            qtout << "----------------- pings ----------------" << Qt::endl;
            const int errors = ITestServiceInterface::pingTests(testServiceInterface, false);
            qtout << "Ping errors " << errors << Qt::endl;
            CApplication::processEventsFor(1000);

            // Performance tools
            qtout << "----------------- performance ----------------" << Qt::endl;

            QElapsedTimer timer;
            timer.start();
            for (int i = 0; i < 10; i++)
            {
                CSpeed speedDummy = testServiceInterface.getSpeed();
                Q_UNUSED(speedDummy);
            }
            qint64 t10 = timer.elapsed(); // ms

            timer.restart();
            for (int i = 0; i < 100; i++)
            {
                CSpeed speedDummy = testServiceInterface.getSpeed();
                Q_UNUSED(speedDummy);
            }
            qint64 t100 = timer.elapsed(); // ms

            timer.restart();
            for (int i = 0; i < 1000; i++)
            {
                CSpeed speedDummy = testServiceInterface.getSpeed();
                Q_UNUSED(speedDummy);
            }
            qint64 t1000 = timer.elapsed(); // ms
            timer.invalidate();
            qtout << "Reading speed objects 10/100/1000 in ms: " << t10 << " " << t100 << " " << t1000 << Qt::endl;

            timer.start();
            for (int i = 0; i < 10; i++)
            {
                CAtcStation stationDummy = testServiceInterface.getAtcStation();
                Q_UNUSED(stationDummy);
            }
            t10 = timer.elapsed(); // ms
            timer.restart();
            for (int i = 0; i < 100; i++)
            {
                CAtcStation stationDummy = testServiceInterface.getAtcStation();
                Q_UNUSED(stationDummy);
            }
            t100 = timer.elapsed(); // ms
            timer.restart();
            for (int i = 0; i < 1000; i++)
            {
                CAtcStation stationDummy = testServiceInterface.getAtcStation();
                Q_UNUSED(stationDummy);
            }
            t1000 = timer.elapsed(); // ms
            qtout << "Reading station objects 10/100/1000 in ms: " << t10 << " " << t100 << " " << t1000 << Qt::endl;

            timer.restart();
            CAtcStationList atcStationList = testServiceInterface.getAtcStationList(10);
            if (atcStationList.size() != 10) qtout << "wrong list size" << atcStationList.size() << Qt::endl;
            t10 = timer.elapsed(); // ms
            timer.restart();
            atcStationList = testServiceInterface.getAtcStationList(100);
            if (atcStationList.size() != 100) qtout << "wrong list size" << atcStationList.size() << Qt::endl;
            t100 = timer.elapsed(); // ms
            timer.restart();
            atcStationList = testServiceInterface.getAtcStationList(1000);
            if (atcStationList.size() != 1000) qtout << "wrong list size" << atcStationList.size() << Qt::endl;
            t1000 = timer.elapsed(); // ms
            qtout << "Reading station list 10/100/1000 in ms: " << t10 << " " << t100 << " " << t1000 << Qt::endl;

            // test reading model entries with a realistic size
            timer.restart();
            CAircraftCfgEntriesList entriesList = testServiceInterface.getAircraftCfgEntriesList(5000);
            if (entriesList.size() != 5000) qtout << "wrong list size" << entriesList.size() << Qt::endl;
            qint64 t5000 = timer.elapsed(); // ms
            qtout << "Reading aircraft cfg entries in ms: " << t5000 << Qt::endl;

            // object paths
            timer.restart();
            QList<QDBusObjectPath> objectPaths = testServiceInterface.getObjectPaths(10);
            if (objectPaths.size() != 10) qtout << "wrong list size" << objectPaths.size() << Qt::endl;
            t10 = timer.elapsed(); // ms
            timer.restart();
            objectPaths = testServiceInterface.getObjectPaths(100);
            if (objectPaths.size() != 100) qtout << "wrong list size" << objectPaths.size() << Qt::endl;
            t100 = timer.elapsed(); // ms
            timer.restart();
            objectPaths = testServiceInterface.getObjectPaths(1000);
            if (objectPaths.size() != 1000) qtout << "wrong list size" << objectPaths.size() << Qt::endl;
            t1000 = timer.elapsed(); // ms
            qtout << "Reading paths list 10/100/1000 in ms: " << t10 << " " << t100 << " " << t1000 << Qt::endl;
            timer.invalidate();

            // next round?
            qtout << "---------------------------------------" << Qt::endl;
            qtout << "Key  ....... x to exit" << Qt::endl;
            QString line = qtin.readLine().toLower().trimmed();
            if (line.startsWith('x'))
            {
                qtout << "Ending!" << Qt::endl;
                break;
            }
        }
    }
} // namespace
