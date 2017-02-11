/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmiscdbus

#include "servicetool.h"
#include "blackcore/application.h"
#include "blackmisc/test/testservice.h"
#include "blackmisc/test/testserviceinterface.h"
#include "blackmisc/test/testutils.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/network/server.h"

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
    QProcess *ServiceTool::startNewProcess(const QString &executable, const QStringList &arguments, QObject *parent)
    {
        QProcess *process = new QProcess(parent);
        process->startDetached(executable, arguments);
        return process;
    }

    void ServiceTool::dataTransferTestServer(BlackMisc::CDBusServer *dBusServer, bool verbose)
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
        const bool sb = (address.toLower().startsWith("session"));
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
        QString line;

        while (true)
        {
            QDBusMessage m = QDBusMessage::createSignal(
                                 CTestService::ObjectPath(), CTestService::InterfaceName(),
                                 "sendStringMessage");

            // The << operator is used to add the parameters for the slot
            const QDateTime dtnow = QDateTime::currentDateTimeUtc();
            const QString msg = QString("Message at %1").arg(dtnow.toString("MM/dd/yyyy @ hh:mm:ss"));
            m << msg;

            // We send this as a non-replying message. This is used for sending errors, replys, signals.
            // Values can be seen on the receiver side
            qtout << "----------------- receiver tests ----------------" << endl;

            // Low level test
            if (connection.send(m)) { qtout << "Send via low level method " << endl; }

            // same as interface message
            // but call the slot
            testServiceInterface.receiveStringMessage(msg);
            qtout << "Send string via interface " << msg << endl;

            // a list
            QList<double> list;
            list << 1.0 << 2.0 << 3.0;
            testServiceInterface.receiveList(list);
            qtout << "Send list via interface " << list.size() << endl;

            // PQs
            CSpeed speed(200, CSpeedUnit::km_h());
            const CSpeed speedNull(0, nullptr);

            testServiceInterface.receiveSpeed(speed);
            qtout << "Send speed via interface " << speed << endl;
            testServiceInterface.receiveSpeed(speedNull);
            qtout << "Send null speed via interface " << speedNull << endl;
            speed.switchUnit(CSpeedUnit::kts());
            testServiceInterface.receiveSpeed(speed);
            qtout << "Send speed via interface " << speed << endl;
            QThread::msleep(2500);
            speed.switchUnit(CSpeedUnit::km_h());
            speed.addValueSameUnit(1.0);

            // Network
            const CServer trafficServer("fooserver", "a foo server", "localhost", 1234,
                                        CUser("112233", "Some real name", "email@xyz.com", "secret"));
            QVariant tsqv = QVariant::fromValue(trafficServer);
            QDBusVariant tsv(tsqv);
            testServiceInterface.receiveVariant(tsv, tsqv.userType());
            qtout << "Send server via interface and variant " << trafficServer <<  tsqv.userType() << endl;

            // Aviation
            const CComSystem comSystem = CComSystem("DBUS COM1", CPhysicalQuantitiesConstants::FrequencyInternationalAirDistress(), CPhysicalQuantitiesConstants::FrequencyUnicom());
            testServiceInterface.receiveComUnit(comSystem);
            qtout << "Send COM via interface " << comSystem << endl;

            CAltitude altitude(1000, CAltitude::MeanSeaLevel, CLengthUnit::ft());
            QVariant qvAl = QVariant::fromValue(altitude);
            QDBusVariant qv(qvAl);
            testServiceInterface.receiveVariant(qv, qvAl.userType());
            testServiceInterface.receiveAltitude(altitude);
            qtout << "Send altitude via interface and variant " << altitude << qvAl.userType() << endl;
            altitude.addValueSameUnit(1);

            const CTransponder transponder(7000, CTransponder::ModeC);
            testServiceInterface.receiveTransponder(transponder);
            qtout << "Send transponder via interface " << transponder << endl;

            const CTrack track(123.45, CTrack::Magnetic, CAngleUnit::deg());
            testServiceInterface.receiveTrack(track);
            qtout << "Send track via interface " << track << endl;

            const CLength len(33, CLengthUnit::m());
            testServiceInterface.receiveLength(len);
            qtout << "Send length via interface " << len << endl;

            const CAltitude alt(44, CAltitude::MeanSeaLevel, CLengthUnit::m());
            testServiceInterface.receiveLength(alt);
            qtout << "Send altitude via interface " << alt << endl;

            const CCallsign callsign("d-ambz");
            testServiceInterface.receiveCallsign(callsign);
            qtout << "Send callsign via interface " << callsign << endl;

            CCoordinateGeodetic geoPos = CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", { 1487, CLengthUnit::ft() }); // Munich
            CAtcStation station(CCallsign("eddm_twr"), CUser("123456", "Joe Controller"),
                                CFrequency(118.7, CFrequencyUnit::MHz()),
                                geoPos, CLength(50, CLengthUnit::km()));

            testServiceInterface.receiveAtcStation(station);
            qtout << "Send ATC " << station << endl;

            // Geo
            // EDDF: 50° 2′ 0″ N, 8° 34′ 14″ E, 100m MSL
            geoPos = CCoordinateGeodetic::fromWgs84("50° 2′ 1″ 23 N", "8° 34′ 14″ E", { 111, CLengthUnit::m() });
            testServiceInterface.receiveGeoPosition(geoPos);
            qtout << "Send geo position " << geoPos << endl;

            qtout << "----------------- variant tests ----------------" << endl;
            CVariantList cvList;
            cvList.push_back(CVariant::fromValue(len));
            cvList.push_back(CVariant::fromValue(alt));
            CVariantList lengthsV;
            lengthsV.push_back(CVariant::fromValue(len));
            lengthsV.push_back(CVariant::fromValue(alt));
            testServiceInterface.receiveLengthsQvl(cvList);
            qtout << "Send lengths via interface as CVariantList" << endl;
            testServiceInterface.receiveLengthsQl(lengthsV);
            qtout << "Send lengths via interface as QList<CVariant>" << endl;
            for (const CVariant &lv : cvList)
            {
                qtout << "  " << "Send length in list: " << lv << endl;
            }
            CApplication::processEventsFor(2000);

            // Value map
            qtout << "----------------- index variant map ----------------" << endl;

            CPropertyIndexVariantMap valueMap;
            valueMap.addValue(1000, 111.222);
            valueMap.addValue(2000, callsign);
            valueMap.addValue(3000, alt);
            valueMap.addValue(4000, track);
            valueMap.addValue(5000, QDateTime::currentDateTime().addDays(1));
            valueMap.addValue(6000, QString("foobar"));
            testServiceInterface.receiveValueMap(valueMap);
            qtout << "Send index variant map " << valueMap << endl;
            CApplication::processEventsFor(2000);

            qtout << "----------------- pings ----------------" << endl;
            int errors = ITestServiceInterface::pingTests(testServiceInterface, false);
            qtout << "Ping errors " << errors << endl;
            CApplication::processEventsFor(2000);

            // Performance tools
            qtout << "----------------- performance ----------------" << endl;

            QElapsedTimer timer;
            timer.start();
            for (int i = 0; i < 10; i++)
            {
                CSpeed speedDummy(i, CSpeedUnit::km_h());
                speedDummy = testServiceInterface.getSpeed();
            }
            qint64 t10 = timer.elapsed(); // ms

            timer.restart();
            for (int i = 0; i < 100; i++)
            {
                CSpeed speedDummy(i, CSpeedUnit::km_h());
                speedDummy = testServiceInterface.getSpeed();
            }
            qint64 t100 = timer.elapsed(); // ms

            timer.restart();
            for (int i = 0; i < 1000; i++)
            {
                CSpeed speedDummy(i, CSpeedUnit::km_h());
                speedDummy = testServiceInterface.getSpeed();
            }
            qint64 t1000 = timer.elapsed(); // ms
            timer.invalidate();
            qtout << "Reading speed objects 10/100/1000 in ms: " << t10 << " " << t100 << " " << t1000 << endl;

            timer.start();
            for (int i = 0; i < 10; i++)
            {
                CAtcStation stationDummy(QString::number(i));
                stationDummy = testServiceInterface.getAtcStation();
            }
            t10 = timer.elapsed(); // ms
            timer.restart();
            for (int i = 0; i < 100; i++)
            {
                CAtcStation stationDummy(QString::number(i));
                stationDummy = testServiceInterface.getAtcStation();
            }
            t100 = timer.elapsed(); // ms
            timer.restart();
            for (int i = 0; i < 1000; i++)
            {
                CAtcStation stationDummy(QString::number(i));
                stationDummy = testServiceInterface.getAtcStation();
            }
            t1000 = timer.elapsed(); // ms
            qtout << "Reading station objects 10/100/1000 in ms: " << t10 << " " << t100 << " " << t1000 << endl;

            timer.restart();
            CAtcStationList atcStationList = testServiceInterface.getAtcStationList(10);
            if (atcStationList.size() != 10) qtout << "wrong list size" << atcStationList.size() << endl;
            t10 = timer.elapsed(); // ms
            timer.restart();
            atcStationList = testServiceInterface.getAtcStationList(100);
            if (atcStationList.size() != 100) qtout << "wrong list size" << atcStationList.size() << endl;
            t100 = timer.elapsed(); // ms
            timer.restart();
            atcStationList = testServiceInterface.getAtcStationList(1000);
            if (atcStationList.size() != 1000) qtout << "wrong list size" << atcStationList.size() << endl;
            t1000 = timer.elapsed(); // ms
            qtout << "Reading station list 10/100/1000 in ms: "  << t10 << " " << t100 << " " << t1000 << endl;

            // test reading model entries with a realistic size
            timer.restart();
            CAircraftCfgEntriesList entriesList = testServiceInterface.getAircraftCfgEntriesList(5000);
            if (entriesList.size() != 5000) qtout << "wrong list size" << entriesList.size() << endl;
            qint64 t5000 = timer.elapsed(); // ms
            qtout << "Reading aircraft cfg entries in ms: " << t5000 << endl;

            // object paths
            timer.restart();
            QList<QDBusObjectPath> objectPaths = testServiceInterface.getObjectPaths(10);
            if (objectPaths.size() != 10) qtout << "wrong list size" << objectPaths.size() << endl;
            t10 = timer.elapsed(); // ms
            timer.restart();
            objectPaths = testServiceInterface.getObjectPaths(100);
            if (objectPaths.size() != 100) qtout << "wrong list size" << objectPaths.size() << endl;
            t100 = timer.elapsed(); // ms
            timer.restart();
            objectPaths = testServiceInterface.getObjectPaths(1000);
            if (objectPaths.size() != 1000) qtout << "wrong list size" << objectPaths.size() << endl;
            t1000 = timer.elapsed(); // ms
            qtout << "Reading paths list 10/100/1000 in ms: "  << t10 << " " << t100 << " " << t1000 << endl;
            timer.invalidate();

            // next round?
            qtout << "---------------------------------------" << endl;
            qtout << "Key  ....... x to exit" << endl;
            line = qtin.readLine().toLower().trimmed();
            if (line.startsWith('x'))
            {
                qtout << "Ending!" << endl;
                break;
            }
        }
    }
} // namespace
