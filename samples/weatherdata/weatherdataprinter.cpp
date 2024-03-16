// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup sampleweatherdata

#include "weatherdataprinter.h"
#include "blackmisc/logmessage.h"
#include <QTextStream>

#include "weatherdataprinter.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/pq/pressure.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/temperature.h"
#include "blackmisc/range.h"
#include "blackmisc/sequence.h"
#include "blackmisc/weather/cloudlayer.h"
#include "blackmisc/weather/cloudlayerlist.h"
#include "blackmisc/weather/gridpoint.h"
#include "blackmisc/weather/temperaturelayer.h"
#include "blackmisc/weather/temperaturelayerlist.h"
#include "blackmisc/weather/windlayer.h"
#include "blackmisc/weather/windlayerlist.h"

#include <stdio.h>
#include <QTextStream>

using namespace BlackMisc;
using namespace BlackMisc::Weather;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackCore;

CWeatherDataPrinter::CWeatherDataPrinter(QObject *parent) : QObject(parent)
{}

void CWeatherDataPrinter::fetchAndPrintWeatherData(const CCoordinateGeodetic &position)
{
    QTextStream qtout(stdout);
    qtout << "Position:" << position.toQString(true) << Qt::endl;
    qtout << "Fetching weather data. This may take a while..." << Qt::endl;

    const CWeatherGrid weatherGrid { { "", position } };
    m_weatherManger.requestWeatherGrid(weatherGrid, { this, &CWeatherDataPrinter::printWeatherData });
}

void CWeatherDataPrinter::printWeatherData(const CWeatherGrid &weatherGrid)
{
    QTextStream qtout(stdout);
    qtout << "... finished." << Qt::endl;
    qtout << weatherGrid.getDescription();
    qtout << Qt::endl;
}
