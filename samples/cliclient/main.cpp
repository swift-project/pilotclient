// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplecliclient

#include "reader.h"
#include "client.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/loghandler.h"

#include <QCoreApplication>
#include <QObject>

using namespace BlackSample;
using namespace BlackMisc;

//! main
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    BlackMisc::CLogHandler::instance()->install();

    Client client(&app);
    LineReader reader;
    QObject::connect(&reader, &LineReader::command, &client, &Client::command);
    QObject::connect(&client, &Client::quit, &reader, &LineReader::terminate);
    QObject::connect(&client, &Client::quit, &app, &QCoreApplication::quit);

    reader.start();
    app.exec();
}
