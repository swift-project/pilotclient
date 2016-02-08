/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSAMPLE_WEATERDATA_WEATHERDATAPRINTER_H
#define BLACKSAMPLE_WEATERDATA_WEATHERDATAPRINTER_H

#include "blackcore/pluginmanagerweatherdata.h"
#include "blackcore/weatherdata.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
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

public slots:
    //! Fetch new weather data for given position and print it once received
    void fetchAndPrintWetherData(const BlackMisc::Geo::CLatitude &lat, const BlackMisc::Geo::CLongitude &lon);

private slots:
    //! Print weather data to stdout
    void ps_printWeatherData();

private:
    BlackCore::CPluginManagerWeatherData m_plugins;
    QScopedPointer<BlackCore::IWeatherDataFactory> m_weatherDataFactory;
    BlackCore::IWeatherData *m_weatherData = nullptr;
};

#endif // guard
