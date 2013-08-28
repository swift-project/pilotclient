/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/network_vatlib.h"
#include "client.h"
#include "reader.h"
#include <QCoreApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QCoreApplication app (argc, argv);
    BlackMisc::CApplicationContext myApplicationContext;
    BlackMisc::IContext::getInstance().setSingleton(new BlackMisc::CDebug());
    BlackMisc::IContext::getInstance().setSingleton<BlackCore::INetwork>(new BlackCore::NetworkVatlib());

    Client client;
    LineReader reader;
    QObject::connect(&reader, SIGNAL(command(const QString&)), &client, SLOT(command(const QString&)));
    QObject::connect(&client, SIGNAL(quit()), &reader, SLOT(terminate()));
    QObject::connect(&client, SIGNAL(quit()), &app, SLOT(quit()));

    reader.start();
    app.exec();
}
