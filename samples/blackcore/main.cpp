/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "tool.h"
#include "blackcore/context_runtime.h"
#include "blackcore/context_settings.h"
#include "blackcore/context_application.h"
#include "blackcore/context_application_impl.h"
#include "blackmisc/networkutils.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QtConcurrent/QtConcurrent>
#include <QMetaType>
#include <QMetaMethod>
#include <QApplication>
#include <QIcon>
#include <QTextStream>

/*!
 * DBus tests, tests marshalling / unmarshalling of many value classes.
 * Forks two processes and sends data via DBus among them.
 */
int main(int argc, char *argv[])
{
    // metadata are registered in runtime
    QApplication a(argc, argv); // not QCoreApplication because of icon, http://qt-project.org/forums/viewthread/15412
    QIcon icon(":/blackcore/icons/tower.png");
    QApplication::setWindowIcon(icon);
    QTextStream cin(stdin);

    qDebug() << "1 + la/ra .. session DBus server (default)";
    qDebug() << "2 + la/ra .. system DBus server";
    qDebug() << "3 + la/ra .. P2P DBus server";
    qDebug() << "la .. local audio, audio runs in this core here (default)";
    qDebug() << "ra .. remote audio, audio runs in the GUI or elsewhere";
    qDebug() << "x  .. exit";
    QString input = cin.readLine().toLower().trimmed();

    // configure DBus server
    QString dBusAddress = BlackCore::CDBusServer::sessionDBusServer();
    if (input.startsWith("2"))
        dBusAddress = BlackCore::CDBusServer::systemDBusServer();
    else if (input.startsWith("3"))
    {
        qDebug() << "found: " << BlackMisc::CNetworkUtils::getKnownIpAddresses();
        qDebug() << "enter ip/port, e.g. 127.0.0.1:45000 (default)";
        dBusAddress = cin.readLine().toLower();
        dBusAddress = BlackCore::CDBusServer::p2pAddress(dBusAddress);
    }
    else if (input.startsWith("x"))
    {
        return 0;
    }

    // with remote audio
    bool remoteAudio = input.contains("ra");
    BlackCore::CRuntime *core = remoteAudio ?
                                new BlackCore::CRuntime(BlackCore::CRuntimeConfig::forCoreAllLocalInDBusNoAudio(dBusAddress), &a) :
                                new BlackCore::CRuntime(BlackCore::CRuntimeConfig::forCoreAllLocalInDBus(dBusAddress), &a);
    QtConcurrent::run(BlackMiscTest::Tool::serverLoop, core); // QFuture<void> future
    qDebug() << "Server event loop, pid:" << BlackMiscTest::Tool::getPid();

    // end
    return a.exec();
}
