/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/networkvatlib.h"
#include "blackmisc/loghandler.h"
#include "client.h"
#include "reader.h"
#include <QCoreApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QCoreApplication app (argc, argv);
    BlackMisc::CLogHandler::instance()->install();

    Client client(&app);
    LineReader reader;
    QObject::connect(&reader, SIGNAL(command(const QString&)), &client, SLOT(command(const QString&)));
    QObject::connect(&client, SIGNAL(quit()), &reader, SLOT(terminate()));
    QObject::connect(&client, SIGNAL(quit()), &app, SLOT(quit()));

    reader.start();
    app.exec();
}
