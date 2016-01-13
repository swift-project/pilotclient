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
