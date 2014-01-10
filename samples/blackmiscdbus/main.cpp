/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/blackmiscfreefunctions.h"
#include "servicetool.h"
#include "blackcore/dbus_server.h"
#include <QDBusMetaType>
#include <QtDBus/qdbusabstractinterface.h>
#include <QtDBus/qdbusconnection.h>
#include <QCoreApplication>
#include <QList>

/*!
 * DBus tests, tests marshalling / unmarshalling of many value classes.
 * Forks two processes and sends data via DBus among them.
 */
int main(int argc, char *argv[])
{
    // of course the code here is containing too many lines, but as it
    // is just for testing, I did not split it up

    BlackMisc::registerMetadata();
    QCoreApplication a(argc, argv);

    // trying to get the arguments into a list
    QStringList cmdlineArgs = QCoreApplication::arguments();
    if (cmdlineArgs.length() < 1)
    {
        qFatal("Missing name of executable");
        return 1;
    }

    // some runtime settings
    const QString executable = QString(cmdlineArgs.at(0)); // used as command to fork myself
    const bool clientFlag = cmdlineArgs.contains("client", Qt::CaseInsensitive);
    const bool useSessionBusForServer = cmdlineArgs.contains("session", Qt::CaseInsensitive); // flag for session bus
    QString address(useSessionBusForServer ? "session" : "tcp:host=192.168.0.133,port=45000"); // testing with real transfer
    // loopback 127.0.0.1

    // Create a Testservice instance and register it with the session bus only if
    // the service isn't already available.
    if (!clientFlag)
    {
        // Configure tests
        qDebug() << "1 .. Run testservice to test data transfer" << address;
        qDebug() << "1sb. Run testservice via session bus";
        qDebug() << "2 .. Data context example (via TCP)" << address;
        qDebug() << "2sb. Data context example (via session bus)";
        qDebug() << "3 .. Data context example, only server (via TCP)" << address;
        qDebug() << "3sb. Data context example, only server (via session bus)";

        qDebug() << "x .. Bye";
        QTextStream qtin(stdin);
        QString mode = qtin.readLine().toLower();
        bool startServer = !mode.startsWith('3');
        if (mode.contains("sb", Qt::CaseInsensitive)) address = "session";
        if (mode.startsWith("1"))
        {
            mode = "testservice";
        }
        else if (mode.startsWith('2') || mode.startsWith('3'))
        {
            mode = "context";
        }
        else
        {
            return 0;
        }

        // I know I am in the "server process here", so I can safely create a CDBusServer
        // this runs in the original process and can be directly debugged
        BlackCore::CDBusServer *dBusServer = new BlackCore::CDBusServer(useSessionBusForServer ? "session" : address);
        qDebug() << "server" << dBusServer->address() << "connected:" << dBusServer->isConnected();

        // start client process
        QStringList args;
        args << "client";
        args << mode;
        if (address == "session") args << address;

        if (startServer) BlackMiscTest::ServiceTool::startNewProcess(executable, args, &a);

        // run tests
        if (mode == "testservice")
        {
            BlackMiscTest::ServiceTool::dataTransferTestServer(dBusServer);
        }
        else if (mode == "context")
        {
            BlackMiscTest::ServiceTool::contextTestServer(dBusServer);
        }

        // loop
        return a.exec();
    }
    else
    {
        // 2nd Process !!! Running on the client's side
        // This runs in a second process, hence cannot be directly debugged within Qt Creators

        // run tests
        if (cmdlineArgs.contains("testservice", Qt::CaseInsensitive))
        {
            BlackMiscTest::ServiceTool::dataTransferTestClient(address);
        }
        else if (cmdlineArgs.contains("context", Qt::CaseInsensitive))
        {
            BlackMiscTest::ServiceTool::contextTestClient(address);
        }

        // loop
        return a.exec();
    }
}
