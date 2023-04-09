/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
