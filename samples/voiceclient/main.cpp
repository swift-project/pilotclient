/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/voice_vatlib.h"
#include "blackcore/blackcorefreefunctions.h"
#include "blackmisc/blackmiscfreefunctions.h"

#include "client.h"
#include "reader.h"

#include <QCoreApplication>
#include <QDebug>


using namespace BlackMisc::Audio;

int main(int argc, char *argv[])
{
    QCoreApplication app (argc, argv);
    Client client(&app);
    BlackMisc::registerMetadata();
    BlackCore::registerMetadata();
    LineReader reader;
    QObject::connect(&reader, SIGNAL(command(const QString &)), &client, SLOT(command(const QString &)), Qt::QueuedConnection);
    QObject::connect(&client, SIGNAL(quit()), &reader, SLOT(terminate()));
    QObject::connect(&client, SIGNAL(quit()), &app, SLOT(quit()));

    reader.start();
    app.exec();
}
