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
        double speedValue = 200.0;
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
            qDebug() << "Key  .......";
            getchar();


            // PQs
            CSpeed speed(speedValue++, BlackMisc::PhysicalQuantities::CSpeedUnit::km_h());
            testserviceInterface.receiveSpeed(speed);
            qDebug() << "Send speed via interface" << speed;

            speed.switchUnit(CSpeedUnit::kts());
            testserviceInterface.receiveSpeed(speed);
            qDebug() << "Send speed via interface" << speed;
            TestserviceTool::sleep(2500);

            // Aviation
            CComSystem comSystem = CComSystem("DBUS COM1", CPhysicalQuantitiesConstants::FrequencyInternationalAirDistress(), CPhysicalQuantitiesConstants::FrequencyUnicom());
            testserviceInterface.receiveComUnit(comSystem);
            qDebug() << "Send COM via interface" << comSystem;

            CAltitude al(1000, true, CLengthUnit::ft());
            QDBusVariant qv(QVariant::fromValue(al));
            testserviceInterface.receiveVariant(qv);
            testserviceInterface.receiveAltitude(al);
            qDebug() << "Send altitude via interface" << al;
            TestserviceTool::sleep(2500);

            // Math
            CMatrix3x3 m33;
            m33.setCellIndex();
            testserviceInterface.receiveMatrix(m33);
            qDebug() << "Send matrix" << m33;

            // next round?
            qDebug() << "Key  .......";
            getchar();
        }
    }

    return a.exec();
}
