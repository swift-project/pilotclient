/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "weatherdatagfs.h"
#include "blackcore/application.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/temperature.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <cmath>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Weather;
using namespace BlackMisc::Math;
using namespace BlackMisc::Network;

namespace BlackWxPlugin
{
    namespace Gfs
    {
        const CWeatherDataGfs::Grib2ParameterTable CWeatherDataGfs::m_grib2ParameterTable
        {
            { { {0, 0} }, { TMP, "Temperature", "K" } },
            { { {1, 1} }, { RH, "Relative Humidity", "%" } },
            { { {1, 7} }, { PRATE, "Precipitation Rate", "kg m-2 s-1" } },
            { { {1, 192} }, { CRAIN, "Categorical Rain", "-" } },
            { { {1, 195} }, { CSNOW, "Categorical Snow", "-" } },
            { { {2, 2} }, { UGRD, "U-Component of Wind", "m s-1" } },
            { { {2, 3} }, { VGRD, "V-Component of Wind", "m s-1" } },
            { { {3, 0} }, { PRES, "Pressure", "Pa" } },
            { { {3, 1} }, { PRMSL, "Pressure Reduced to MSL", "Pa" } },
            { { {6, 1} }, { TCDC, "Total Cloud Cover", "%" } },
        };

        double millibarToLevel(double millibar)
        {
            millibar /= 100;
            double level = (1 - std::pow(millibar / 1013.25, 0.190284)) * 145366.45;
            return level;
        }

        CWeatherDataGfs::CWeatherDataGfs(QObject *parent) :
            IWeatherData(parent)
        { }

        CWeatherDataGfs::~CWeatherDataGfs()
        {
            if (m_parseGribFileWorker && !m_parseGribFileWorker->isFinished()) { m_parseGribFileWorker->abandonAndWait(); }
        }

        void CWeatherDataGfs::fetchWeatherData(const CWeatherGrid &grid, const CLength &range)
        {
            m_grid = grid;
            m_maxRange = range;
            if (m_gribData.isEmpty())
            {
                if (!sApp->isInternetAccessible())
                {
                    CLogMessage(this).error("No weather download since network/internet not accessible");
                    return;
                }

                const QUrl url = getDownloadUrl().toQUrl();
                CLogMessage(this).debug() << "Started to download GFS data from" << url.toString();
                QNetworkRequest request(url);
                sApp->getFromNetwork(request, { this, &CWeatherDataGfs::ps_parseGfsFile });
            }
            else
            {
                CLogMessage(this).debug() << "Using cached data";
                Q_ASSERT_X(!m_parseGribFileWorker, Q_FUNC_INFO, "Worker already running");
                m_parseGribFileWorker = BlackMisc::CWorker::fromTask(this, "parseGribFile", [this]()
                {
                    parseGfsFileImpl(m_gribData);
                });
                m_parseGribFileWorker->then(this, &CWeatherDataGfs::ps_fetchingWeatherDataFinished);
            }
        }

        CWeatherGrid CWeatherDataGfs::getWeatherData() const
        {
            QReadLocker l(&m_lockData);
            return m_weatherGrid;
        }

        void CWeatherDataGfs::ps_fetchingWeatherDataFinished()
        {
            emit fetchingFinished();
        }

        void CWeatherDataGfs::ps_parseGfsFile(QNetworkReply *nwReplyPtr)
        {
            // wrap pointer, make sure any exit cleans up reply
            // required to use delete later as object is created in a different thread
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);

            m_gribData = nwReply->readAll();
            Q_ASSERT_X(!m_parseGribFileWorker, Q_FUNC_INFO, "Worker already running");
            m_parseGribFileWorker = BlackMisc::CWorker::fromTask(this, "parseGribFile", [this]()
            {
                parseGfsFileImpl(m_gribData);
            });
            m_parseGribFileWorker->then(this, &CWeatherDataGfs::ps_fetchingWeatherDataFinished);
        }

        CUrl CWeatherDataGfs::getDownloadUrl() const
        {
            CUrl downloadUrl = sApp->getGlobalSetup().getNcepGlobalForecastSystemUrl();

            static const QStringList grib2Levels =
            {
                "surface",
                "100_mb",
                "150_mb",
                "200_mb",
                "300_mb",
                "400_mb",
                "500_mb",
                "600_mb",
                "700_mb",
                "800_mb",
                "900_mb",
                "1000_mb",
                "high_cloud_bottom_level",
                "high_cloud_layer",
                "high_cloud_top_level",
                "low_cloud_bottom_level",
                "low_cloud_layer",
                "low_cloud_top_level",
                "middle_cloud_bottom_level",
                "middle_cloud_layer",
                "middle_cloud_top_level",
            };

            static const QStringList grib2Variables =
            {
                "PRATE",
                "PRES",
                "PRMSL",
                "RH",
                "TCDC",
                "TMP",
                "UGRD",
                "VGRD",
                "CRAIN",
                "CSNOW"
            };

            static const std::array<int, 4> cycles = { { 0, 6, 12, 18 } };
            const QDateTime now = QDateTime::currentDateTimeUtc();

            // GFS data is published after 4 yours.
            const QDateTime cnow = now.addSecs(-5 * 60 * 60);

            int hourLastPublishedCycle = 0;
            for (const auto &cycle : cycles)
            {
                if (cnow.time().hour() > cycle) { hourLastPublishedCycle = cycle; }
            }

            // Forecast is published in 3 hours steps.
            // Round down to a multiple of 3
            int forecast = now.time().hour() - hourLastPublishedCycle;
            if (forecast < 0) { forecast += 24; }
            forecast = CMathUtils::roundToMultipleOf(forecast, 3);

            QString filename("gfs.t%1z.pgrb2full.0p50.f%2");
            filename = filename.arg(hourLastPublishedCycle, 2, 10, QLatin1Char('0'));
            filename = filename.arg(forecast, 3, 10, QLatin1Char('0'));

            QString directory("gfs.%1%2");
            directory = directory.arg(cnow.toString("yyyyMMdd"));
            directory = directory.arg(hourLastPublishedCycle, 2, 10, QLatin1Char('0'));

            downloadUrl.appendQuery("file", filename);
            for (const auto &level : grib2Levels)
            {
                downloadUrl.appendQuery("lev_" + level, "on");
            }
            for (const auto &variable : grib2Variables)
            {
                downloadUrl.appendQuery("var_" + variable, "on");
            }
            downloadUrl.appendQuery("leftlon", "0");
            downloadUrl.appendQuery("rightlon", "360");
            downloadUrl.appendQuery("toplat", "90");
            downloadUrl.appendQuery("bottomlat", "-90");
            downloadUrl.appendQuery("dir", "%2F" + directory);
            return downloadUrl;
        }

        void CWeatherDataGfs::parseGfsFileImpl(const QByteArray &gribData)
        {
            QWriteLocker lock(&m_lockData);

            m_gfsWeatherGrid.clear();
            m_weatherGrid.clear();

            int messageNo = 0;
            g2int iseek = 0;
            for (;;)
            {
                if (QThread::currentThread()->isInterruptionRequested()) { return; }

                // Search next grib field
                g2int lskip = 0;
                g2int lgrib = 0;
                auto constData = reinterpret_cast<unsigned char *>(const_cast<char *>(gribData.data()));
                findNextGribMessage(constData, gribData.size(), iseek, &lskip, &lgrib);
                if (lgrib == 0) { break; }

                unsigned char *readPtr = constData + lskip;
                iseek = lskip + lgrib;

                g2int sec0[3];
                g2int sec1[13];
                g2int numlocal = 0;
                g2int numfields = 0;
                g2_info(readPtr, sec0, sec1, &numfields, &numlocal);

                for (int n = 0; n < numfields; n++)
                {
                    g2int unpack = 1;
                    g2int expand = 1;
                    gribfield *gfld = nullptr;
                    g2_getfld(readPtr, n + 1, unpack, expand, &gfld);
                    if (gfld->idsectlen < 12) { CLogMessage(this).warning("Identification section: wrong length!"); continue; }

                    if (gfld->igdtnum != 0) { CLogMessage(this).warning("Can handle only grid definition template number = 0"); }

                    int nscan = gfld->igdtmpl[18];
                    int npnts = gfld->ngrdpts;
                    int nx = gfld->igdtmpl[7];
                    int ny = gfld->igdtmpl[8];
                    if (nscan != 0) {  CLogMessage(this).error("Can only handle scanning mode NS:WE."); }
                    if (npnts != nx * ny) {  CLogMessage(this).error("Cannot handle non-regular grid."); }

                    if (m_gfsWeatherGrid.empty()) { createWeatherGrid(gfld); }

                    if (gfld->ipdtnum == 0) { handleProductDefinitionTemplate40(gfld); }
                    else if (gfld->ipdtnum == 8) { handleProductDefinitionTemplate48(gfld); }
                    else { CLogMessage(this).warning("Cannot handle product definition template %1") << gfld->ipdtnum; continue; }

                    g2_free(gfld);
                }
                messageNo++;
            }
            CLogMessage(this).debug() << "Parsed" << messageNo << "GRIB messages.";

            for (const GfsGridPoint &gfsGridPoint : as_const(m_gfsWeatherGrid))
            {
                if (QThread::currentThread()->isInterruptionRequested()) { return; }

                CTemperatureLayerList temperatureLayers;

                CAltitude surfaceAltitude(0, CAltitude::AboveGround, CLengthUnit::defaultUnit());
                CTemperatureLayer surfaceTemperature(surfaceAltitude, CTemperature(gfsGridPoint.surfaceTemperature, CTemperatureUnit::K()), {}, {});
                temperatureLayers.insert(surfaceTemperature);

                CWindLayerList windLayers;
                for (auto isobaricLayerIt = gfsGridPoint.isobaricLayers.begin(); isobaricLayerIt != gfsGridPoint.isobaricLayers.end(); ++isobaricLayerIt)
                {
                    GfsIsobaricLayer isobaricLayer = isobaricLayerIt.value();
                    CAltitude level(isobaricLayerIt.key(), CAltitude::MeanSeaLevel, CLengthUnit::ft());

                    auto temperature = CTemperature { isobaricLayer.temperature, CTemperatureUnit::K() };
                    auto dewPoint = calculateDewPoint(temperature, isobaricLayer.relativeHumidity);

                    CTemperatureLayer temperatureLayer(level, temperature, dewPoint, isobaricLayer.relativeHumidity);
                    temperatureLayers.insert(temperatureLayer);

                    double windDirection = -1 * CMathUtils::rad2deg(std::atan2(-isobaricLayer.windU, isobaricLayer.windV));
                    windDirection += 180.0;
                    if (windDirection < 0.0) { windDirection += 360.0; }
                    if (windDirection >= 360.0) { windDirection -= 360.0; }
                    double windSpeed = std::hypot(isobaricLayer.windU, isobaricLayer.windV);
                    CWindLayer windLayer(level, CAngle(windDirection, CAngleUnit::deg()), CSpeed(windSpeed, CSpeedUnit::m_s()), {});
                    windLayers.insert(windLayer);
                }

                CCloudLayerList cloudLayers;
                for (auto cloudLayerIt = gfsGridPoint.cloudLayers.begin(); cloudLayerIt != gfsGridPoint.cloudLayers.end(); ++cloudLayerIt)
                {
                    CCloudLayer cloudLayer;
                    cloudLayer.setBase(CAltitude(cloudLayerIt.value().bottomLevel, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
                    cloudLayer.setTop(CAltitude(cloudLayerIt.value().topLevel, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
                    cloudLayer.setCoveragePercent(cloudLayerIt.value().totalCoverage);
                    if (gfsGridPoint.surfaceSnow > 0.0) { cloudLayer.setPrecipitation(CCloudLayer::Snow); }
                    if (gfsGridPoint.surfaceRain > 0.0) { cloudLayer.setPrecipitation(CCloudLayer::Rain); }

                    // Precipitation rate is in kg m-2 s-1, which is equal to mm/s
                    // Multiply with 3600 to convert to mm/h
                    cloudLayer.setPrecipitationRate(gfsGridPoint.surfacePrecipitationRate * 3600.0);
                    cloudLayer.setClouds(CCloudLayer::CloudsUnknown);
                    cloudLayers.insert(cloudLayer);
                }

                auto surfacePressure = PhysicalQuantities::CPressure { gfsGridPoint.surfacePressure, PhysicalQuantities::CPressureUnit::Pa() };

                CLatitude latitude(gfsGridPoint.latitude, CAngleUnit::deg());
                CLongitude longitude(gfsGridPoint.longitude, CAngleUnit::deg());
                auto position = CCoordinateGeodetic { latitude, longitude, {0} };
                BlackMisc::Weather::CGridPoint gridPoint({}, position, cloudLayers, temperatureLayers, {}, windLayers, surfacePressure);
                m_weatherGrid.insert(gridPoint);
            }
        }

        void CWeatherDataGfs::findNextGribMessage(unsigned char *buffer, g2int size, g2int iseek, g2int *lskip, g2int *lgrib)
        {
            *lgrib = 0;

            if (iseek >= size) return;
            g2int ipos = iseek;
            unsigned char *bufferStart = buffer + iseek;

            // READ PARTIAL SECTION
            g2int lim = size - 8;

            for (g2int k = 0; k < lim; k++)
            {
                g2int start;
                g2int vers;
                // Look for "GRIB" at the beginning
                gbit(bufferStart, &start, (k + 0) * 8, 4 * 8);
                gbit(bufferStart, &vers, (k + 7) * 8, 1 * 8);
                if (start == 1196575042 && (vers == 1 || vers == 2))
                {
                    g2int lengrib = 0;
                    //  Look for '7777' at end of GRIB message
                    if (vers == 1) gbit(bufferStart, &lengrib, (k + 4) * 8, 3 * 8);
                    if (vers == 2) gbit(bufferStart, &lengrib, (k + 12) * 8, 4 * 8);

                    // Check the limits
                    g2int endPos = ipos + k + lengrib;
                    if (endPos > size) return;
                    unsigned char *bufferEnd = buffer + endPos - 4;
                    // Hard code to 4 instead of sizeof(g2int)
                    g2int end;
                    gbit(bufferEnd, &end, 0, 4 * 8);
                    if (end == 926365495)
                    {
                        // GRIB message found
                        *lskip = ipos + k;
                        *lgrib = lengrib;
                        return;
                    }
                }
            }
        }

        void CWeatherDataGfs::createWeatherGrid(const gribfield *gfld)
        {
            int npnts = gfld->ngrdpts;
            int nx = gfld->igdtmpl[7];
            int ny = gfld->igdtmpl[8];
            double units = 0.000001;
            double latitude1 = gfld->igdtmpl[11] * units;
            double longitude1 = gfld->igdtmpl[12] * units;
            int nres = gfld->igdtmpl[13];
            double latitude2 = gfld->igdtmpl[14] * units;
            double longitude2 = gfld->igdtmpl[15] * units;
            double dlatitude = gfld->igdtmpl[16] * units;
            double dlongitude = gfld->igdtmpl[17] * units;

            if (latitude1 < -90.0 || latitude2 < -90.0 || latitude1 > 90.0 || latitude2 > 90.0)
            {
                CLogMessage(this).warning("Invalid grid definition: lat1 = %1 - lat2 = %2") << latitude1 << latitude2;
                return;
            }
            if (longitude1 < 0.0 || longitude2 < 0.0 || longitude1 > 360.0 || longitude2 > 360.0)
            {
                CLogMessage(this).warning("Invalid grid definition: lon1 = %1 - lon2 = %2") << longitude1 << longitude2;
                return;
            }
            if (npnts != nx * ny)
            {
                CLogMessage(this).warning("Invalid grid definition: npnts != nx * ny!");
                return;
            }

            // Scan direction is North -> South
            double north = latitude1;
            double south = latitude2;

            if (south > north)
            {
                CLogMessage(this).warning("Invalid grid definition: South = %1 - North = %2") << south << north;
                return;
            }

            double dy = 0;
            if (ny != 1)
            {
                dy = (north - south) / (ny - 1.0);
                if (nres & 16)
                {
                    if (fabs(dy - dlatitude) > 0.001)
                    {
                        CLogMessage(this).warning("Invalid grid definition: delta latitude is inconsistent");
                        return;
                    }
                }
            }
            else
            {
                dy = 0.0;
            }

            // Scan direction is West -> East
            double west = longitude1;
            double east = longitude2;
            if (east <= west) { east += 360.0; }
            if (east - west > 360.0) { east -= 360.0; }
            if (west < 0)
            {
                west += 360.0;
                east += 360.0;
            }

            double dx = 0;
            if (nx != 1)
            {
                dx = (east - west) / (nx - 1);
                dx = fabs(dx);
                if (nres & 32)
                {
                    if (fabs(dx - fabs(dlongitude)) > 0.001)
                    {
                        CLogMessage(this).warning("Invalid grid definition: delta longitude is inconsistent");
                        return;
                    }
                }
            }
            else
            {
                dx = 0.0;
            }
            dy = fabs(dy);

            if (nx > 0 && ny > 0)
            {
                for (int iy = 0; iy < ny; iy++)
                {
                    int i = nx * iy;
                    for (int ix = 0; ix < nx; ix++)
                    {
                        GfsGridPoint gridPoint;
                        gridPoint.latitude = latitude1 - iy * dy;
                        gridPoint.longitude = longitude1 + ix * dx;
                        if (gridPoint.longitude >= 360.0) { gridPoint.longitude -= 360.0; }
                        if (gridPoint.longitude < 0.0) { gridPoint.longitude += 360.0; }
                        gridPoint.fieldPosition = ix + i;
                        CCoordinateGeodetic gridPointPosition(gridPoint.latitude, gridPoint.longitude, 0);
                        if (m_maxRange == CLength())
                        {
                            m_gfsWeatherGrid.append(gridPoint);
                        }
                        else
                        {
                            for (const CGridPoint &fixedGridPoint : as_const(m_grid))
                            {
                                auto distance = calculateGreatCircleDistance(gridPointPosition, fixedGridPoint.getPosition()).value(CLengthUnit::m());
                                auto maxRange = m_maxRange.value(CLengthUnit::m());
                                if (distance < maxRange)
                                {
                                    m_gfsWeatherGrid.append(gridPoint);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        void CWeatherDataGfs::handleProductDefinitionTemplate40(const gribfield *gfld)
        {
            if (gfld->ipdtlen != 15)
            {
                CLogMessage(this).warning("Template 4.0 has wrong length");
                return;
            }

            // http://www.nco.ncep.noaa.gov/pmb/docs/grib2/grib2_temp4-0.shtml
            g2int parameterCategory = gfld->ipdtmpl[0];
            g2int parameterNumber = gfld->ipdtmpl[1];
            g2int typeFirstFixedSurface = gfld->ipdtmpl[9];
            g2int valueFirstFixedSurface = gfld->ipdtmpl[11];

            std::array<g2int, 2> key { { parameterCategory, parameterNumber } };
            // Make sure the key exists
            if (!m_grib2ParameterTable.contains(key))
            {
                CLogMessage(this).warning("Unknown GRIB2 parameter: %1 - %2") << parameterCategory << parameterNumber;
                return;
            }

            double level = 0;
            switch (typeFirstFixedSurface)
            {
            case GroundOrWaterSurface:
                level = 0;
                break;
            case IsobaricSurface:
                level = std::round(millibarToLevel(valueFirstFixedSurface));
                break;
            default:
                CLogMessage(this).warning("Unexpected first fixed surface type: %1") << typeFirstFixedSurface;
                return;
            }

            auto parameterValue = m_grib2ParameterTable[key];
            switch (parameterValue.code)
            {
            case TMP:
                setTemperature(gfld->fld, level);
                break;
            case RH:
                setHumidity(gfld->fld, level);
                break;
            case UGRD:
                setWindU(gfld->fld, level);
                break;
            case VGRD:
                setWindV(gfld->fld, level);
                break;
            case PRMSL:
                break;
            case PRES:
                setCloudPressure(gfld->fld, level);
                break;
            default:
                Q_ASSERT(false);
                break;
            }
        }

        void CWeatherDataGfs::handleProductDefinitionTemplate48(const gribfield *gfld)
        {
            if (gfld->ipdtlen != 29)
            {
                CLogMessage(this).warning("Template 4.8 has wrong length.");
                return;
            }

            g2int parameterCategory = gfld->ipdtmpl[0];
            g2int parameterNumber = gfld->ipdtmpl[1];
            g2int typeFirstFixedSurface = gfld->ipdtmpl[9];

            std::array<g2int, 2> key { { parameterCategory, parameterNumber } };
            // Make sure the key exists
            if (!m_grib2ParameterTable.contains(key))
            {
                CLogMessage(this).warning("Unknown GRIB2 parameter: %1 - %2") << parameterCategory << parameterNumber;
                return;
            }

            static const QHash<int, int> grib2CloudLevelHash =
            {
                { LowCloudBottomLevel, LowCloud },
                { LowCloudTopLevel, LowCloud },
                { LowCloudLayer, LowCloud },
                { MiddleCloudBottomLevel, MiddleCloud },
                { MiddleCloudTopLevel, MiddleCloud },
                { MiddleCloudLayer, MiddleCloud },
                { HighCloudBottomLevel, HighCloud },
                { HighCloudTopLevel, HighCloud },
                { HighCloudLayer, HighCloud },
            };

            auto parameterValue = m_grib2ParameterTable[key];
            switch (parameterValue.code)
            {
            case TCDC:
                setCloudCoverage(gfld->fld, grib2CloudLevelHash.value(typeFirstFixedSurface));
                break;
            case PRES:
                setCloudLevel(gfld->fld, typeFirstFixedSurface, grib2CloudLevelHash.value(typeFirstFixedSurface));
                break;
            case PRATE:
                setPrecipitationRate(gfld->fld);
                break;
            case CRAIN:
                setSurfaceRain(gfld->fld);
                break;
            case CSNOW:
                setSurfaceSnow(gfld->fld);
                break;
            default:
                break;
            }
        }

        void CWeatherDataGfs::setTemperature(const g2float *fld, double level)
        {
            for (auto &gridPoint : m_gfsWeatherGrid)
            {
                if (level > 0) { gridPoint.isobaricLayers[level].temperature = fld[gridPoint.fieldPosition]; }
                else { gridPoint.surfaceTemperature = fld[gridPoint.fieldPosition]; }
            }
        }

        void CWeatherDataGfs::setHumidity(const g2float *fld, double level)
        {
            for (auto &gridPoint : m_gfsWeatherGrid)
            {
                gridPoint.isobaricLayers[level].relativeHumidity = fld[gridPoint.fieldPosition];
            }
        }

        void CWeatherDataGfs::setWindV(const g2float *fld, double level)
        {
            for (auto &gridPoint : m_gfsWeatherGrid)
            {
                gridPoint.isobaricLayers[level].windV = fld[gridPoint.fieldPosition];
            }
        }

        void CWeatherDataGfs::setWindU(const g2float *fld, double level)
        {
            for (auto &gridPoint : m_gfsWeatherGrid)
            {
                gridPoint.isobaricLayers[level].windU = fld[gridPoint.fieldPosition];
            }
        }

        void CWeatherDataGfs::setCloudCoverage(const g2float *fld, int level)
        {
            for (auto &gridPoint : m_gfsWeatherGrid)
            {
                if (fld[gridPoint.fieldPosition] > 0.0) { gridPoint.cloudLayers[level].totalCoverage = fld[gridPoint.fieldPosition]; }
            }
        }

        void CWeatherDataGfs::setCloudLevel(const g2float *fld, int surfaceType, int level)
        {
            for (auto &gridPoint : m_gfsWeatherGrid)
            {
                static const g2float minimumLayer = 0.0;
                switch (surfaceType)
                {
                case LowCloudBottomLevel:
                case MiddleCloudBottomLevel:
                case HighCloudBottomLevel:
                    if (fld[gridPoint.fieldPosition] > minimumLayer) { gridPoint.cloudLayers[level].bottomLevel = millibarToLevel(fld[gridPoint.fieldPosition]); }
                    break;
                case LowCloudTopLevel:
                case MiddleCloudTopLevel:
                case HighCloudTopLevel:
                    if (fld[gridPoint.fieldPosition] > minimumLayer) { gridPoint.cloudLayers[level].topLevel = millibarToLevel(fld[gridPoint.fieldPosition]); }
                    break;
                default:
                    Q_ASSERT(false);
                    break;
                }
            }
        }

        void CWeatherDataGfs::setCloudPressure(const g2float *fld, double level)
        {
            for (auto &gridPoint : m_gfsWeatherGrid)
            {
                if (level > 0) { /* todo */ }
                else { gridPoint.surfacePressure = fld[gridPoint.fieldPosition];  }
            }
        }

        void CWeatherDataGfs::setSurfaceRain(const g2float *fld)
        {
            for (auto &gridPoint : m_gfsWeatherGrid)
            {
                gridPoint.surfaceRain = fld[gridPoint.fieldPosition];
            }
        }

        void CWeatherDataGfs::setSurfaceSnow(const g2float *fld)
        {
            for (auto &gridPoint : m_gfsWeatherGrid)
            {
                gridPoint.surfaceSnow = fld[gridPoint.fieldPosition];
            }
        }

        void CWeatherDataGfs::setPrecipitationRate(const g2float *fld)
        {
            for (auto &gridPoint : m_gfsWeatherGrid)
            {
                gridPoint.surfacePrecipitationRate = fld[gridPoint.fieldPosition];
            }
        }

        CTemperature CWeatherDataGfs::calculateDewPoint(const CTemperature &temperature, double relativeHumidity)
        {
            double temperatureInCelsius = temperature.value(CTemperatureUnit::C());
            double saturationVaporPressure = 6.112 * std::exp((17.67 * temperatureInCelsius) / (temperatureInCelsius + 243.5));
            double vaporPressure = saturationVaporPressure * (relativeHumidity / 100.0);
            double dewPointInCelsius = std::log(vaporPressure / 6.112) * 243.5 / (17.67 - std::log(vaporPressure / 6.112));
            return { dewPointInCelsius, CTemperatureUnit::C() };
        }

        BlackCore::IWeatherData *CWeatherDataGfsFactory::create(QObject *parent)
        {
            return new CWeatherDataGfs(parent);
        }

    } // namespace
} // namespace
