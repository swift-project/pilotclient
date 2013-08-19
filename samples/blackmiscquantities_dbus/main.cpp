/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <QDBusMetaType>
#include <QtDBus/qdbusabstractinterface.h>
#include <QtDBus/qdbusconnection.h>
#include <QCoreApplication>
#include <QList>

#include "testservice.h"
#include "testservice_adaptor.h"
#include "testservice_interface.h"
#include "testservicetool.h"

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Math;
using namespace BlackMisc::Geo;
using namespace BlackMiscTest;

/*!
 * Sample tests
 */
int main(int argc, char *argv[])
{
    BlackMisc::registerMetadata();
    QCoreApplication a(argc, argv);

    // init
    if (argc < 1) {
        qFatal("Missing name of executable");
    }
    const QString executable = QString(argv[0]);
    Testservice* pTestservice = NULL;
    TestserviceAdaptor* pTestserviceAdaptor = NULL;

    // Create a Testservice instance and register it with the session bus only if
    // the service isn't already available.
    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.interface()->isServiceRegistered(Testservice::ServiceName)) {
        pTestservice = new Testservice(&a);
        pTestserviceAdaptor = new TestserviceAdaptor(pTestservice);

        if (!connection.registerService(Testservice::ServiceName)) {
            QDBusError err = connection.lastError();
            qWarning() << err.message();
            qWarning() << "Started dbus-daemon.exe --session?";
            qWarning() << "Created directory session.d? See https://dev.vatsim-germany.org/projects/vatpilotclient/wiki/DBusExample#Running-the-example";
            qFatal("Could not register service!");
        }

        if (!connection.registerObject(Testservice::ServicePath, pTestservice)) {
            qFatal("Could not register service object!");
        }

        qDebug() << "Registration running as pid: " << TestserviceTool::getPid();
        if (pTestservice) qDebug() << "Service registered";
        if (pTestserviceAdaptor) qDebug() << "Adaptor object registered";

        new TestserviceAdaptor(pTestservice); // adaptor
        QString service; // service not needed
        if (QDBusConnection::sessionBus().connect(
                    service, Testservice::ServicePath, Testservice::ServiceName,
                    "sendStringMessage", pTestservice, SLOT(receiveStringMessage(const QString &)))) {
            qDebug() << "Connected object with bus sendStringMessage";
        } else {
            qFatal("Cannot connect service with DBus");
        }

        // Call myself to implement client
        TestserviceTool::startNewProcess(executable, &a);

    } else {
        qDebug() << "Already registered, assuming 2nd pid: " << TestserviceTool::getPid();
        BlackmisctestTestserviceInterface testserviceInterface(Testservice::ServiceName, Testservice::ServicePath, connection, &a);

        CSpeed speed(200, BlackMisc::PhysicalQuantities::CSpeedUnit::km_h());
        CAltitude al(1000, CAltitude::MeanSeaLevel, CLengthUnit::ft());

        while (true) {
            QDBusMessage m = QDBusMessage::createSignal(
                        Testservice::ServicePath, Testservice::ServiceName,
                        "sendStringMessage");

            //The << operator is used to add the parameters for the slot
            QDateTime dtnow = QDateTime::currentDateTimeUtc();
            QString msg = QString("Message from %1 at %2").arg(TestserviceTool::getPid()).arg(dtnow.toString("MM/dd/yyyy @ hh:mm:ss"));
            m << msg;

            // We send this as a non-replying message. This is used for sending errors, replys, signals,
            // and method calls (slots) that don't return
            if (connection.send(m)) {
                qDebug() << "Send via low level method" << m;
            }

            // same as interface message
            testserviceInterface.receiveStringMessage(msg);
            qDebug() << "Send string via interface" << msg;

            // a list
            QList<double> list;
            list << 1.0 << 2.0 << 3.0;
            testserviceInterface.receiveList(list);
            qDebug() << "Send list via interface" << list;

            // PQs
            testserviceInterface.receiveSpeed(speed);
            qDebug() << "Send speed via interface" << speed;

            speed.switchUnit(CSpeedUnit::kts());
            testserviceInterface.receiveSpeed(speed);
            qDebug() << "Send speed via interface" << speed;
            TestserviceTool::sleep(2500);
            speed.switchUnit(CSpeedUnit::km_h());
            speed.addValueSameUnit(1.0);

            // Aviation
            CComSystem comSystem = CComSystem("DBUS COM1", CPhysicalQuantitiesConstants::FrequencyInternationalAirDistress(), CPhysicalQuantitiesConstants::FrequencyUnicom());
            testserviceInterface.receiveComUnit(comSystem);
            qDebug() << "Send COM via interface" << comSystem;

            QDBusVariant qv(QVariant::fromValue(al));
            testserviceInterface.receiveVariant(qv);
            testserviceInterface.receiveAltitude(al);
            qDebug() << "Send altitude via interface" << al;
            al.addValueSameUnit(1);

            CTransponder transponder("transponder", 7000, CTransponder::ModeC);
            testserviceInterface.receiveTransponder(transponder);
            qDebug() << "Send transponder via interface" << transponder;

            CTrack track(123.45, CTrack::Magnetic, CAngleUnit::deg());
            testserviceInterface.receiveTrack(track);
            qDebug() << "Send track via interface" << track;

            CLength len(33, CLengthUnit::m());
            testserviceInterface.receiveLength(len);
            qDebug() << "Send length via interface" << len;

            CAltitude alt(33, CAltitude::MeanSeaLevel, CLengthUnit::m());
            testserviceInterface.receiveLength(alt);
            qDebug() << "Send altitude via interface" << alt;

            QVariantList lengths;
            lengths << QVariant::fromValue(len);
            lengths << QVariant::fromValue(alt);
            testserviceInterface.receiveLengths(lengths);
            qDebug() << "Send lengths via interface";

            TestserviceTool::sleep(2500);

            // Math
            CMatrix3x3 m33;
            m33.setCellIndex();
            testserviceInterface.receiveMatrix(m33);
            qDebug() << "Send matrix" << m33;

            // Geo
            // EDDF: 50° 2′ 0″ N, 8° 34′ 14″ E, 100m MSL
            CLatitude lat = CLatitude::fromWgs84("50° 2′ 1″ 23 N");
            CLongitude lon = CLongitude::fromWgs84("8° 34′ 14″ E");
            CLength height(100, CLengthUnit::m());
            CCoordinateGeodetic geoPos(lat, lon, height);
            testserviceInterface.receiveGeoPosition(geoPos);
            qDebug() << "Send geo position" << geoPos;

            // next round?
            qDebug() << "Key  .......";
            getchar();
        }
    }

    return a.exec();
}
