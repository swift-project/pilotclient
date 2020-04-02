/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleweatherdata

#include "blackcore/application.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/registermetadata.h"
#include "reader.h"
#include "weatherdataprinter.h"

#include <stdio.h>
#include <QCoreApplication>
#include <QObject>
#include <QTextStream>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackCore;

//! main
int main(int argc, char *argv[])
{
    QCoreApplication qa(argc, argv);
    CApplication a(CApplicationInfo::Sample);
    BlackMisc::registerMetadata();
    CLogHandler::instance()->install(true);
    CLogHandler::instance()->enableConsoleOutput(false); // default disable

    CLineReader lineReader(&a);
    CWeatherDataPrinter printer(&a);
    QObject::connect(&lineReader, &CLineReader::weatherDataRequest, &printer, &CWeatherDataPrinter::fetchAndPrintWeatherData);
    QObject::connect(&lineReader, &CLineReader::wantsToQuit, &lineReader, &CLineReader::terminate);
    QObject::connect(&lineReader, &CLineReader::finished, &a, &QCoreApplication::quit);

    QTextStream qtout(stdout);
    qtout << "Usage: <lat> <lon>" << Qt::endl;
    qtout << "Example: 48.5 11.5" << Qt::endl;
    qtout << "Type x to quit" << Qt::endl;

    lineReader.start();
    return a.exec();
}
