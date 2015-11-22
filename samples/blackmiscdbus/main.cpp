/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/registermetadata.h"
#include "servicetool.h"
#include "blackcore/dbusserver.h"
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
    QString ip = "192.168.0.133";
    QString port = "45000";
    const QString executable = QString(cmdlineArgs.at(0)); // used as command to fork myself
    const bool clientFlag = cmdlineArgs.contains("client", Qt::CaseInsensitive);
    bool useSessionBusForServer;
    if (cmdlineArgs.contains("session", Qt::CaseInsensitive))
    {
        // session mode
        useSessionBusForServer = true;
    }
    else
    {
        // TCP/IP mode
        useSessionBusForServer = false;
        if (cmdlineArgs.length() > 2)
        {
            ip = cmdlineArgs.at(cmdlineArgs.length() - 2);
            port = cmdlineArgs.at(cmdlineArgs.length() - 1);
        }
    }
    QString addressTcp = QString("tcp:host=%1,port=%2").arg(ip).arg(port);
    QString address(useSessionBusForServer ? "session" : addressTcp); // testing with real transfer

    // Create a Testservice instance and register it with the session bus only if
    // the service isn't already available.
    if (!clientFlag)
    {
        // Configure tests
    Menu:

        qDebug() << "1 .. Run testservice to test data transfer" << addressTcp;
        qDebug() << "1sb. Run testservice via session bus";
        qDebug() << "----- Change address / port (no validation, do before starting server)";
        qDebug() << "loop Address to loopback, 127.0.0.1";
        qDebug() << "ip   some IP address, e.g 192.168.100.100";
        qDebug() << "port some port, e.g 12345";
        qDebug() << "-----";
        qDebug() << "x .. Bye";
        QTextStream qtin(stdin);
        QString mode = qtin.readLine().toLower().trimmed();

        if (mode.startsWith("l"))
        {
            ip = "127.0.0.1";
            addressTcp = QString("tcp:host=%1,port=%2").arg(ip).arg(port);
            goto Menu;
        }
        if (mode.startsWith("i"))
        {
            QStringList p = mode.split(QRegExp("\\s"));
            if (p.length() > 1)
            {
                ip = p.at(1);
                addressTcp = QString("tcp:host=%1,port=%2").arg(ip).arg(port);
            }
            goto Menu;
        }
        if (mode.startsWith("p"))
        {
            QStringList p = mode.split(QRegExp("\\s"));
            if (p.length() > 1)
            {
                port = p.at(1);
                addressTcp = QString("tcp:host=%1,port=%2").arg(ip).arg(port);
            }
            goto Menu;
        }

        // start DBus
        address = QString(useSessionBusForServer ? "session" : addressTcp); // testing with real transfer
        if (mode.contains("sb", Qt::CaseInsensitive)) address = "session";
        if (mode.startsWith("1"))
        {
            mode = "testservice";
        }
        else
        {
            return 0;
        }

        // I know I am in the "server process here", so I can safely create a CDBusServer
        // this runs in the original process and can be directly debugged
        qDebug();
        qDebug("--------------------------------------------------------");

        BlackCore::CDBusServer *dBusServer = new BlackCore::CDBusServer(useSessionBusForServer ? "session" : address);
        if (dBusServer->hasQDBusServer())
        {
            qDebug() << "server" << dBusServer->qDBusServer()->address() << "connected:" << dBusServer->qDBusServer()->isConnected();
        }
        // start client process
        QStringList args;
        args << "client";
        args << mode;
        if (address == "session")
        {
            args << address; // set session as cmd arg
        }
        else
        {
            args << ip;
            args << port;
        }
        BlackMiscTest::ServiceTool::startNewProcess(executable, args, &a);

        // run tests
        if (mode == "testservice")
        {
            BlackMiscTest::ServiceTool::dataTransferTestServer(dBusServer);
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

        // loop
        return a.exec();
    }
}
