#include "weatherdataprinter.h"
#include "blackmisc/logmessage.h"
#include <QTextStream>

//! \file
//! \ingroup sampleweatherdata

using namespace BlackMisc;
using namespace BlackMisc::Weather;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackCore;


CWeatherDataPrinter::CWeatherDataPrinter(QObject *parent) : QObject(parent)
{ }

void CWeatherDataPrinter::fetchAndPrintWetherData(const CCoordinateGeodetic &position)
{
    QTextStream qtout(stdout);
    qtout << "Fetching weather data. This may take a while..." << endl;

    CWeatherGrid weatherGrid { { "", position } };
    m_weatherManger.requestWeatherGrid(weatherGrid, { this, &CWeatherDataPrinter::ps_printWeatherData });
}

void CWeatherDataPrinter::ps_printWeatherData(const BlackMisc::Weather::CWeatherGrid &weatherGrid)
{
    QTextStream qtout(stdout);
    qtout << "... finished." << endl;
    for (const CGridPoint &gridPoint : weatherGrid)
    {
        qtout << "Latitude:" << gridPoint.getPosition().latitude().toQString() << endl;
        qtout << "Longitude:" << gridPoint.getPosition().longitude().toQString() << endl;
        qtout << "    Surface Pressure: " << gridPoint.getSurfacePressure().toQString() << endl;

        CTemperatureLayerList temperatureLayers = gridPoint.getTemperatureLayers();
        temperatureLayers.sort([](const CTemperatureLayer &a, const CTemperatureLayer &b) { return a.getLevel() < b.getLevel(); });
        qtout << "    Temperature Layers: " << endl;
        for (const auto &temperatureLayer : as_const(temperatureLayers))
        {
            qtout << "        Level: " << temperatureLayer.getLevel().toQString() << endl;
            qtout << "            Temperature: " << temperatureLayer.getTemperature().toQString() << endl;
            qtout << "            Relative Humidity: " << temperatureLayer.getRelativeHumidity() << " %" << endl;
        }
        qtout << endl;

        CWindLayerList windLayers = gridPoint.getWindLayers();
        windLayers.sort([](const CWindLayer &a, const CWindLayer &b) { return a.getLevel() < b.getLevel(); });
        qtout << "    Wind Layers: " << endl;
        for (const auto &windLayer : as_const(windLayers))
        {
            qtout << "        Level: " << windLayer.getLevel().toQString() << endl;
            qtout << "            Wind: " << windLayer.getDirection().toQString() << " at " << windLayer.getSpeed().toQString() << endl;
        }
        qtout << endl;

        qtout << "    Cloud Layers: " << endl;
        CCloudLayerList cloudLayers = gridPoint.getCloudLayers();
        cloudLayers.sort([](const CCloudLayer &a, const CCloudLayer &b) { return a.getBase() < b.getBase(); });
        for (int i = 0; i < cloudLayers.size(); i++)
        {
            const CCloudLayer &cloudLayer = cloudLayers[i];
            qtout << "        Top: " << cloudLayer.getTop().toQString() << endl;
            qtout << "            Coverage: " << cloudLayer.getCoveragePercent() << " %" << endl;
            qtout << "            Precipitation type: " << cloudLayer.getPrecipitation() << endl;
            qtout << "            Precipitation rate: " << cloudLayer.getPrecipitationRate() << endl;
            qtout << "        Base: " << cloudLayer.getBase().toQString() << endl;
        }
        qtout << endl << endl;
    }
}
