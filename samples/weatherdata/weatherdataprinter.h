/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKSAMPLE_WEATERDATA_WEATHERDATAPRINTER_H
#define BLACKSAMPLE_WEATERDATA_WEATHERDATAPRINTER_H

//! \file
//! \ingroup sampleweatherdata

#include "blackcore/weathermanager.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/weather/weathergrid.h"

#include <QObject>

/*!
 * CWeatherDataPrinter fetches and prints weather data
 */
class CWeatherDataPrinter : public QObject
{
    Q_OBJECT

public:
    //! Constructor
    CWeatherDataPrinter(QObject *parent = nullptr);

    //! Fetch new weather data for given position and print it once received
    void fetchAndPrintWeatherData(const BlackMisc::Geo::CCoordinateGeodetic &position);

private:
    //! Print weather data to stdout
    void printWeatherData(const BlackMisc::Weather::CWeatherGrid &weatherGrid);

    BlackCore::CWeatherManager m_weatherManger { this };
};

#endif // guard
