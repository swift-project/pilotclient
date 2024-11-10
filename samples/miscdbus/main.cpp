// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemiscdbus

#include "misc/dbusserver.h"
#include "misc/directoryutils.h"
#include "misc/registermetadata.h"
#include "misc/network/networkutils.h"
#include "misc/dbusutils.h"
#include "misc/processctrl.h"
#include "servicetool.h"
#include <stdio.h>
#include <QCoreApplication>
#include <QDBusServer>
#include <QDebug>
#include <QProcess>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <Qt>
#include <QtDebug>

using namespace swift::misc;

//! main
int main(int argc, char *argv[])
{
    // Of course the code here is containing too many lines, but as it
    // is just for testing, I did not split it up

    swift::misc::registerMetadata();
    QCoreApplication a(argc, argv);
    QTextStream out(stdout, QIODevice::WriteOnly);
    QTextStream qtin(stdin);
    const bool verbose = false;

    // trying to get the arguments into a list
    const QStringList cmdlineArgs = QCoreApplication::arguments();
    if (cmdlineArgs.length() < 1)
    {
        qFatal("Missing name of executable");
        return EXIT_FAILURE;
    }

    // some runtime settings
    const QStringList ipV4Addresses = swift::misc::network::CNetworkUtils::getKnownLocalIpV4Addresses();
    QString ip = ipV4Addresses.isEmpty() ? "192.168.0.125" : ipV4Addresses.last();
    QString port = "45000";
    const QString executable = QString(cmdlineArgs.at(0)); // used as command to fork myself
    const bool clientFlag = cmdlineArgs.contains("client", Qt::CaseInsensitive);
    bool useSessionBusForServer = false;
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
    QString addressTcp = QStringLiteral("tcp:host=%1,port=%2").arg(ip, port);
    QString address(useSessionBusForServer ? "session" : addressTcp); // testing with real transfer

    // Create a Testservice instance and register it with the session bus only if
    // the service isn't already available.
    if (clientFlag)
    {
        // 2nd Process !!! Running on the client's side
        // This runs in a second process, hence cannot be directly debugged within Qt Creator
        out << "Running client side " << QCoreApplication::applicationPid() << Qt::endl;

        // run tests
        if (cmdlineArgs.contains("testservice", Qt::CaseInsensitive))
        {
            BlackSample::ServiceTool::dataTransferTestClient(address);
        }

        // loop
        return a.exec();
    }
    else
    {
    Menu:
        out << "Pid: " << QCoreApplication::applicationPid() << Qt::endl;
        out << "1 .. Run testservice to test data transfer" << addressTcp << Qt::endl;
        out << "1sb. Run testservice via session bus" << Qt::endl;
        out << "2 .. Show signatures" << Qt::endl;
        out << "----- Change address / port (no validation, do before starting server)" << Qt::endl;
        out << "loop Address to loopback, 127.0.0.1" << Qt::endl;
        out << "ip   some IP address, e.g " << ip << Qt::endl;
        out << "port some port, e.g 12345" << Qt::endl;
        out << "-----" << Qt::endl;
        out << "x .. Bye" << Qt::endl;
        QString mode = qtin.readLine().toLower().trimmed();

        if (mode.startsWith("l"))
        {
            ip = "127.0.0.1";
            addressTcp = QStringLiteral("tcp:host=%1,port=%2").arg(ip, port);
            goto Menu;
        }
        if (mode.startsWith("i"))
        {
            const QStringList p = mode.split(QRegExp("\\s"));
            if (p.length() > 1)
            {
                ip = p.at(1);
                addressTcp = QStringLiteral("tcp:host=%1,port=%2").arg(ip, port);
            }
            goto Menu;
        }
        if (mode.startsWith("p"))
        {
            const QStringList p = mode.split(QRegExp("\\s"));
            if (p.length() > 1)
            {
                port = p.at(1);
                addressTcp = QStringLiteral("tcp:host=%1,port=%2").arg(ip, port);
            }
            goto Menu;
        }
        if (mode.startsWith("2"))
        {
            out << "---------------------------------" << Qt::endl;
            // swift::misc::CDBusUtils::showDBusSignatures(out);
            out << "---------------------------------" << Qt::endl;
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
        out << "--------------------------------------------------------" << Qt::endl;

        CDBusServer *dBusServer = new CDBusServer(useSessionBusForServer ? "session" : address);
        if (dBusServer->hasQDBusServer())
        {
            out << "server" << dBusServer->qDBusServer()->address()
                << " connected:" << dBusServer->qDBusServer()->isConnected() << Qt::endl;
        }
        // start client process
        QStringList args;
        args << "client";
        args << mode;
        if (address == "session")
        {
            args << "session"; // set session as cmd arg
        }
        else
        {
            args << ip;
            args << port;
        }

        // run tests
        if (mode == "testservice")
        {
            BlackSample::ServiceTool::dataTransferTestServer(dBusServer, verbose);
        }

        // testing in new process
        CProcessCtrl::startDetached(executable, args, true);

        // testing in same process
        // BlackSample::ServiceTool::dataTransferTestClient(address);

        // loop
        return a.exec();
    }
}
