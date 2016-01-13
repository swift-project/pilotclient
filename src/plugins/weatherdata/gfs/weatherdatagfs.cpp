/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "weatherdatagfs.h"
#include "blackmisc/worker.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Weather;
using namespace BlackMisc::Math;

namespace BlackWxPlugin
{
    namespace Gfs
    {
        const CWeatherDataGfs::Grib2ParameterTable CWeatherDataGfs::m_grib2ParameterTable
        {
            { {0, 0}, { TMP, "Temperature", "K" } },
            { {1, 1}, { RH, "Relative Humidity", "%" } },
            { {2, 2}, { UGRD, "U-Component of Wind", "m s-1" } },
            { {2, 3}, { VGRD, "V-Component of Wind", "m s-1" } },
            { {3, 0}, { PRES, "Pressure", "Pa" } },
            { {3, 1}, { PRMSL, "Pressure Reduced to MSL", "Pa" } },
            { {6, 1}, { TCDC, "Total Cloud Cover", "%" } },
        };

        double millibarToLevel(double millibar)
        {
            millibar /= 100;
            double level = (1 - std::pow(millibar / 1013.25, 0.190284)) * 145366.45;
            return level;
        }

        CWeatherDataGfs::CWeatherDataGfs(QObject *parent) :
            IWeatherData(parent),
            m_networkAccessManager(this)
        {
            connect(&m_networkAccessManager, &QNetworkAccessManager::finished, this, &CWeatherDataGfs::ps_parseGfsFile);
        }

        void CWeatherDataGfs::fetchWeatherData(const CLatitude &latitude, const CLongitude &longitude, double maxDistance)
        {
            m_latitude = latitude;
            m_longitude = longitude;
            m_maxDistance = maxDistance;
            if (m_gribData.isEmpty())
            {
                CLogMessage(this).debug() << "Started to download GFS data...";
                QUrl url = getDownloadUrl();
                CLogMessage(this).debug() << "Download url:" << url.toString();
                QNetworkRequest request(url);
                m_networkAccessManager.get(request);
            }
            else
            {
                CLogMessage(this).debug() << "Using cached data";
                CWorker *worker = BlackMisc::CWorker::fromTask(this, "parseGribFile", [this]()
                {
                    parseGfsFileImpl(m_gribData);
                });
                worker->then(this, &CWeatherDataGfs::ps_fetchingWeatherDataFinished);
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

        void CWeatherDataGfs::ps_parseGfsFile(QNetworkReply *reply)
        {
            CLogMessage(this).debug() << "finished";
            m_gribData = reply->readAll();
            CWorker *worker = BlackMisc::CWorker::fromTask(this, "parseGribFile", [this]()
            {
                parseGfsFileImpl(m_gribData);
            });
            worker->then(this, &CWeatherDataGfs::ps_fetchingWeatherDataFinished);
            reply->deleteLater();
        }

        QUrl CWeatherDataGfs::getDownloadUrl() const
        {
            QString baseurl = "http://nomads.ncep.noaa.gov/cgi-bin/filter_gfs_0p50.pl?";

            static const QStringList grib2Levels =
            {
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
                "PRES",
                "PRMSL",
                "RH",
                "TCDC",
                "TMP",
                "UGRD",
                "VGRD",
            };

            static const std::array<int, 4> cycles = { 0, 6, 12, 18 };
            const QDateTime now = QDateTime::currentDateTimeUtc();

            // GFS data is published after 4 yours.
            const QDateTime cnow = now.addSecs( -5 * 60 * 60);

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

            QStringList params;
            params.append("file=" + filename);
            for (const auto &level : grib2Levels)
            {
                params.append("lev_" + level + "=on");
            }
            for (const auto &variable : grib2Variables)
            {
                params.append("var_" + variable + "=on");
            }
            params.append("leftlon=0");
            params.append("rightlon=360");
            params.append("toplat=90");
            params.append("bottomlat=-90");
            params.append("dir=%2F" + directory);

            return QUrl(baseurl + params.join('&'));
        }

        void CWeatherDataGfs::parseGfsFileImpl(const QByteArray &gribData)
        {
            QWriteLocker lock(&m_lockData);

            m_gfsWeatherGrid.clear();
            m_weatherGrid.clear();

            int messageNo = 0;
            g2int iseek = 0;
            for(;;)
            {
                // Search next grib field
                g2int lskip = 0;
                g2int lgrib = 0;
                findNextGribMessage((unsigned char*)gribData.data(), gribData.size(), iseek, &lskip, &lgrib);
                if (lgrib == 0) { break; }

                unsigned char *readPtr = (unsigned char*)gribData.data() + lskip;
                iseek=lskip + lgrib;

                g2int sec0[3];
                g2int sec1[13];
                g2int numlocal = 0;
                g2int numfields = 0;
                g2_info(readPtr, sec0, sec1, &numfields, &numlocal);

                CLogMessage(this).debug() << "Parsing grib message no:" << messageNo;
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

            for (const GfsGridPoint &gfsGridPoint : m_gfsWeatherGrid)
            {
                CTemperatureLayerList temperatureLayers;
                CWindLayerList windLayers;
                for (auto isobaricLayerIt = gfsGridPoint.isobaricLayers.begin(); isobaricLayerIt != gfsGridPoint.isobaricLayers.end(); ++isobaricLayerIt)
                {
                    GfsIsobaricLayer isobaricLayer = isobaricLayerIt.value();
                    CAltitude level(isobaricLayerIt.key(), CAltitude::MeanSeaLevel, CLengthUnit::ft());

                    CTemperatureLayer temperature(level, CTemperature(isobaricLayer.temperature, CTemperatureUnit::K()), isobaricLayer.relativeHumidity);
                    temperatureLayers.insert(temperature);

                    double windDirection = -1 * CMathUtils::rad2deg(std::atan2(-isobaricLayer.windU, isobaricLayer.windV));
                    windDirection += 180.0;
                    if (windDirection < 0.0) { windDirection += 360.0; }
                    if (windDirection >= 360.0) { windDirection -= 360.0; }
                    double windSpeed = std::hypot(isobaricLayer.windU, isobaricLayer.windV);
                    CWindLayer windLayer(level, CAngle(windDirection, CAngleUnit::deg()), CSpeed(windSpeed, CSpeedUnit::m_s()), {});
                    windLayers.insert(windLayer);
                }

                CCloudLayerList cloudLayers;
                for(auto cloudLayerIt = gfsGridPoint.cloudLayers.begin(); cloudLayerIt != gfsGridPoint.cloudLayers.end(); ++cloudLayerIt)
                {
                    CCloudLayer cloudLayer;
                    cloudLayer.setBase(CAltitude(cloudLayerIt.value().bottomLevel, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
                    cloudLayer.setCeiling(CAltitude(cloudLayerIt.value().topLevel, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
                    cloudLayer.setCoveragePercent(cloudLayerIt.value().totalCoverage);
                    cloudLayers.insert(cloudLayer);
                }

                CLatitude latitude(gfsGridPoint.latitude, CAngleUnit::deg());
                CLongitude longitude(gfsGridPoint.longitude, CAngleUnit::deg());
                BlackMisc::Weather::CGridPoint gridPoint(latitude, longitude, cloudLayers, temperatureLayers, windLayers);
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
                gbit(bufferStart,&start,(k+0)*8,4*8);
                gbit(bufferStart,&vers,(k+7)*8,1*8);
                if (start == 1196575042 && (vers == 1 || vers == 2))
                {
                    g2int lengrib;
                    //  Look for '7777' at end of GRIB message
                    if (vers == 1) gbit(bufferStart,&lengrib,(k+4)*8,3*8);
                    if (vers == 2) gbit(bufferStart,&lengrib,(k+12)*8,4*8);

                    // Check the limits
                    g2int endPos = ipos + k + lengrib;
                    if (endPos > size) return;
                    unsigned char *bufferEnd = buffer + endPos - 4;
                    // Hard code to 4 instead of sizeof(g2int)
                    g2int end;
                    gbit(bufferEnd,&end, 0, 4*8);
                    if (end == 926365495)
                    {
                        // GRIB message found
                        *lskip=ipos+k;
                        *lgrib=lengrib;
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
                        if(gridPoint.longitude >= 360.0) { gridPoint.longitude -= 360.0; }
                        if(gridPoint.longitude < 0.0) { gridPoint.longitude += 360.0; }
                        gridPoint.fieldPosition = ix + i;
                        CCoordinateGeodetic gridPointPosition(gridPoint.latitude, gridPoint.longitude, 0);
                        CCoordinateGeodetic centralPosition(m_latitude, m_longitude, {0});
                        if ((m_maxDistance == -1) || calculateEuclideanDistance(gridPointPosition, centralPosition) < m_maxDistance )
                        {
                            m_gfsWeatherGrid.append(gridPoint);
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

            g2int parameterCategory = gfld->ipdtmpl[0];
            g2int parameterNumber = gfld->ipdtmpl[1];
            g2int valueFirstFixedSurface = gfld->ipdtmpl[11];

            std::array<g2int, 2> key { parameterCategory, parameterNumber };
            // Make sure the key exists
            if (!m_grib2ParameterTable.contains(key))
            {
                CLogMessage(this).warning("Unknown GRIB2 parameter: %1 - %2") << parameterCategory << parameterNumber;
                return;
            }

            double level = std::round(millibarToLevel(valueFirstFixedSurface));
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

            std::array<g2int, 2> key { parameterCategory, parameterNumber };
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
            default:
                break;
            }
        }

        void CWeatherDataGfs::setTemperature(const g2float *fld, double level)
        {
            for (auto gridPointIt = m_gfsWeatherGrid.begin(); gridPointIt < m_gfsWeatherGrid.end(); ++gridPointIt)
            {
                gridPointIt->isobaricLayers[level].temperature = fld[gridPointIt->fieldPosition];
            }
        }

        void CWeatherDataGfs::setHumidity(const g2float *fld, double level)
        {
            for (auto gridPointIt = m_gfsWeatherGrid.begin(); gridPointIt < m_gfsWeatherGrid.end(); ++gridPointIt)
            {
                gridPointIt->isobaricLayers[level].relativeHumidity = fld[gridPointIt->fieldPosition];
            }
        }

        void CWeatherDataGfs::setWindV(const g2float *fld, double level)
        {
            for (auto gridPointIt = m_gfsWeatherGrid.begin(); gridPointIt < m_gfsWeatherGrid.end(); ++gridPointIt)
            {
                gridPointIt->isobaricLayers[level].windV = fld[gridPointIt->fieldPosition];
            }
        }

        void CWeatherDataGfs::setWindU(const g2float *fld, double level)
        {
            for (auto gridPointIt = m_gfsWeatherGrid.begin(); gridPointIt < m_gfsWeatherGrid.end(); ++gridPointIt)
            {
                gridPointIt->isobaricLayers[level].windU = fld[gridPointIt->fieldPosition];
            }
        }

        void CWeatherDataGfs::setCloudCoverage(const g2float *fld, int level)
        {
            for (auto gridPointIt = m_gfsWeatherGrid.begin(); gridPointIt < m_gfsWeatherGrid.end(); ++gridPointIt)
            {
                if(fld[gridPointIt->fieldPosition] > 0.0) { gridPointIt->cloudLayers[level].totalCoverage = fld[gridPointIt->fieldPosition]; }
            }
        }

        void CWeatherDataGfs::setCloudLevel(const g2float *fld, int surfaceType, int level)
        {
            for (auto gridPointIt = m_gfsWeatherGrid.begin(); gridPointIt < m_gfsWeatherGrid.end(); ++gridPointIt)
            {
                static const g2float minimumLayer = 0.0;
                switch(surfaceType)
                {
                case LowCloudBottomLevel:
                case MiddleCloudBottomLevel:
                case HighCloudBottomLevel:
                    if (fld[gridPointIt->fieldPosition] > minimumLayer) { gridPointIt->cloudLayers[level].bottomLevel = millibarToLevel(fld[gridPointIt->fieldPosition]); }
                    break;
                case LowCloudTopLevel:
                case MiddleCloudTopLevel:
                case HighCloudTopLevel:
                    if (fld[gridPointIt->fieldPosition] > minimumLayer) { gridPointIt->cloudLayers[level].topLevel = millibarToLevel(fld[gridPointIt->fieldPosition]); }
                    break;
                default:
                    Q_ASSERT(false);
                    break;
                }
            }
        }

        BlackCore::IWeatherData *CWeatherDataGfsFactory::create(QObject *parent)
        {
            return new CWeatherDataGfs(parent);
        }

    } // namespace
} // namespace
