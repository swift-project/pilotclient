/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "weatherdatagfs.h"
#include "blackcore/application.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/temperature.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/verify.h"
#include "blackmisc/logmessage.h"
#include "blackconfig/buildconfig.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QStringBuilder>
#include <cmath>

using namespace BlackConfig;
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
        //! \cond PRIVATE
        enum Grib2CloudLevel
        {
            LowCloud,
            MiddleCloud,
            HighCloud
        };

        enum Grib2ParameterCode
        {
            UNKNOWN,
            TMP,
            RH,
            UGRD,
            VGRD,
            PRATE,
            PRES,
            PRMSL,
            TCDC,
            CRAIN,
            CSNOW
        };

        enum Grib2FixedSurfaceTypes
        {
            GroundOrWaterSurface = 1,
            IsobaricSurface = 100,
            MeanSeaLevel = 101,
            LowCloudBottomLevel = 212,
            LowCloudTopLevel = 213,
            LowCloudLayer = 214,
            MiddleCloudBottomLevel = 222,
            MiddleCloudTopLevel = 223,
            MiddleCloudLayer = 224,
            HighCloudBottomLevel = 232,
            HighCloudTopLevel = 233,
            HighCloudLayer = 234
        };

        struct Grib2ParameterValue
        {
            Grib2ParameterValue() = default;
            Grib2ParameterValue(Grib2ParameterCode code_, const QString &name_, const QString &unit_) : code(code_), name(name_), unit(unit_) {}
            Grib2ParameterCode code = UNKNOWN;
            QString name;
            QString unit;
        };

        struct GfsIsobaricLayer
        {
            float temperature = 0.0;
            float relativeHumidity = 0.0;
            float windU = 0.0;
            float windV = 0.0;
        };

        inline bool operator==(const GfsIsobaricLayer &lhs, const GfsIsobaricLayer &rhs)
        {
            return qFuzzyCompare(lhs.temperature, rhs.temperature) &&
                   qFuzzyCompare(lhs.relativeHumidity, rhs.relativeHumidity) &&
                   qFuzzyCompare(lhs.windU, rhs.windU) &&
                   qFuzzyCompare(lhs.windV, rhs.windV);
        }

        struct GfsCloudLayer
        {
            float bottomLevelPressure = 0.0;
            float topLevelPressure = 0.0;
            float totalCoverage = 0.0;
            float topLevelTemperature = 0.0;
        };

        struct GfsGridPoint
        {
            float latitude = 0.0;
            float longitude = 0.0;
            int fieldPosition = 0;
            QHash<int, GfsCloudLayer> cloudLayers;
            QHash<float, GfsIsobaricLayer> isobaricLayers;
            float surfaceRain = 0;
            float surfaceSnow = 0;
            float surfacePrecipitationRate = 0;
            float pressureAtMsl = 0.0;
        };
        //! \endcond

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

        // https://physics.stackexchange.com/questions/333475/how-to-calculate-altitude-from-current-temperature-and-pressure
        double calculateAltitudeFt(float seaLevelPressurePa, float atmosphericPressurePa, float temperatureK)
        {
            double altitude = (std::pow(seaLevelPressurePa / atmosphericPressurePa, 0.19022) - 1) * temperatureK * 3.28084 /  0.0065;
            return altitude;
        }

        CWeatherDataGfs::CWeatherDataGfs(QObject *parent) :
            IWeatherData(parent)
        { }

        CWeatherDataGfs::~CWeatherDataGfs()
        {
            if (m_parseGribFileWorker && !m_parseGribFileWorker->isFinished()) { m_parseGribFileWorker->abandonAndWait(); }
        }

        void CWeatherDataGfs::fetchWeatherData(const CWeatherGrid &initialGrid, const CLength &range)
        {
            if (!sApp || sApp->isShuttingDown()) { return; }
            m_grid = initialGrid;
            m_maxRange = range;
            if (m_gribData.isEmpty())
            {
                if (!sApp || !sApp->isInternetAccessible())
                {
                    CLogMessage(this).error(u"No weather download since network/internet not accessible");
                    return;
                }

                const QUrl url = getDownloadUrl().toQUrl();
                CLogMessage(this).debug() << "Started to download GFS data from" << url.toString();
                QNetworkRequest request(url);
                sApp->getFromNetwork(request, { this, &CWeatherDataGfs::parseGfsFile });
            }
            else
            {
                CLogMessage(this).debug() << "Using cached data";
                Q_ASSERT_X(!m_parseGribFileWorker, Q_FUNC_INFO, "Worker already running");
                m_parseGribFileWorker = CWorker::fromTask(this, "parseGribFile", [this]()
                {
                    parseGfsFileImpl(m_gribData);
                });
                m_parseGribFileWorker->then(this, &CWeatherDataGfs::fetchingWeatherDataFinished);
            }
        }

        void CWeatherDataGfs::fetchWeatherDataFromFile(const QString &filePath, const CWeatherGrid &grid, const CLength &range)
        {
            m_grid = grid;
            m_maxRange = range;

            QFile file(filePath);
            if (!file.exists() || !file.open(QIODevice::ReadOnly)) { return; }
            m_gribData = file.readAll();

            Q_ASSERT_X(!m_parseGribFileWorker, Q_FUNC_INFO, "Worker already running");
            m_parseGribFileWorker = CWorker::fromTask(this, "parseGribFile", [this]()
            {
                parseGfsFileImpl(m_gribData);
            });
            m_parseGribFileWorker->then(this, &CWeatherDataGfs::fetchingWeatherDataFinished);
        }

        CWeatherGrid CWeatherDataGfs::getWeatherData() const
        {
            QReadLocker l(&m_lockData);
            return m_weatherGrid;
        }

        void CWeatherDataGfs::fetchingWeatherDataFinished()
        {
            // If the worker is not destroyed yet, try again in 10 ms.
            if (m_parseGribFileWorker)
            {
                QPointer<CWeatherDataGfs> myself(this);
                QTimer::singleShot(25, this, [ = ]
                {
                    // try again until finished
                    if (!myself) { return; }
                    myself->fetchingWeatherDataFinished();
                });
            }
            else
            {
                emit fetchingFinished();
            }
        }

        void CWeatherDataGfs::parseGfsFile(QNetworkReply *nwReplyPtr)
        {
            // wrap pointer, make sure any exit cleans up reply
            // required to use delete later as object is created in a different thread
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);

            m_gribData = nwReply->readAll();
            Q_ASSERT_X(!m_parseGribFileWorker, Q_FUNC_INFO, "Worker already running");
            m_parseGribFileWorker = CWorker::fromTask(this, "parseGribFile", [this]()
            {
                parseGfsFileImpl(m_gribData);
            });
            m_parseGribFileWorker->then(this, &CWeatherDataGfs::fetchingWeatherDataFinished);
        }

        CUrl CWeatherDataGfs::getDownloadUrl() const
        {
            CUrl downloadUrl = sApp->getGlobalSetup().getNcepGlobalForecastSystemUrl25();

            static const QStringList grib2Levels =
            {
                "mean_sea_level",
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

            // GFS data is published after 5 yours.
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

            // The 0 hour forecast, does not contain all required parameters. Hence use 1 hour forecast instead.
            if (forecast == 0) { forecast = 1; }

            const QString filename = u"gfs." % QStringLiteral("t%1z").arg(hourLastPublishedCycle, 2, 10, QLatin1Char('0'))
                                     % u".pgrb2.0p25."
                                     % QStringLiteral("f%2").arg(forecast, 3, 10, QLatin1Char('0'));
            const QString directory = u"/gfs." % cnow.toString("yyyyMMdd") % u"/" % QStringLiteral("%1").arg(hourLastPublishedCycle, 2, 10, QLatin1Char('0')) % u"/atmos";

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
            downloadUrl.appendQuery("dir", directory);
            return downloadUrl;
        }

        bool CWeatherDataGfs::parseGfsFileImpl(const QByteArray &gribData)
        {
            if (!m_lockData.tryLockForWrite(1000))
            {
                CLogMessage(this).warning(u"Cannot log CWeatherDataGfs data");
                return false;
            }

            m_lockData.unlock();
            QWriteLocker lock(&m_lockData);

            m_gfsWeatherGrid.clear();
            m_weatherGrid.clear();

            // Messages should be 76. This is a combination
            // of requested values (e.g. temperature, clouds etc) at specific layers (2 mbar, 10 mbar, surface).
            constexpr int maxMessages = 76;
            int messageNo = 0;
            g2int iseek = 0;
            for (;;)
            {
                if (QThread::currentThread()->isInterruptionRequested()) { return false; }

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
                    if (gfld->idsectlen < 12) { CLogMessage(this).warning(u"Identification section: wrong length!"); continue; }

                    if (gfld->igdtnum != 0) { CLogMessage(this).warning(u"Can handle only grid definition template number = 0"); }

                    int nscan = gfld->igdtmpl[18];
                    int npnts = gfld->ngrdpts;
                    int nx = gfld->igdtmpl[7];
                    int ny = gfld->igdtmpl[8];
                    if (nscan != 0) {  CLogMessage(this).error(u"Can only handle scanning mode NS:WE."); }
                    if (npnts != nx * ny) {  CLogMessage(this).error(u"Cannot handle non-regular grid."); }

                    if (m_gfsWeatherGrid.empty()) { createWeatherGrid(gfld); }

                    if (gfld->ipdtnum == 0) { handleProductDefinitionTemplate40(gfld); }
                    else if (gfld->ipdtnum == 8) { handleProductDefinitionTemplate48(gfld); }
                    else { CLogMessage(this).warning(u"Cannot handle product definition template %1") << gfld->ipdtnum; continue; }

                    g2_free(gfld);
                }
                messageNo++;
            }

            // validate
            if (messageNo > maxMessages && CBuildConfig::isLocalDeveloperDebugBuild())
            {
                // as discussed this means a format change
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "Format change in GRIB, too many messages");
            }

            const int weatherGridPointsNo = m_gfsWeatherGrid.size();
            CLogMessage(this).debug() << "Parsed"   << messageNo << "GRIB messages.";
            CLogMessage(this).debug() << "Obtained" << weatherGridPointsNo << "grid points.";

            constexpr int maxPoints = 200;
            for (const GfsGridPoint &gfsGridPoint : std::as_const(m_gfsWeatherGrid))
            {
                if (QThread::currentThread()->isInterruptionRequested()) { return false; }

                CTemperatureLayerList temperatureLayers;
                CWindLayerList windLayers;
                for (const GfsIsobaricLayer &isobaricLayer : gfsGridPoint.isobaricLayers)
                {
                    float level = gfsGridPoint.isobaricLayers.key(isobaricLayer);
                    double altitudeFt = calculateAltitudeFt(gfsGridPoint.pressureAtMsl, level, isobaricLayer.temperature);

                    CAltitude altitude(altitudeFt, CAltitude::MeanSeaLevel, CLengthUnit::ft());

                    auto temperature = CTemperature { isobaricLayer.temperature, CTemperatureUnit::K() };
                    auto dewPoint = calculateDewPoint(temperature, isobaricLayer.relativeHumidity);

                    CTemperatureLayer temperatureLayer(altitude, temperature, dewPoint, isobaricLayer.relativeHumidity);
                    temperatureLayers.push_back(temperatureLayer);

                    double windDirection = -1 * CMathUtils::rad2deg(std::atan2(-isobaricLayer.windU, isobaricLayer.windV));
                    windDirection += 180.0;
                    if (windDirection < 0.0) { windDirection += 360.0; }
                    if (windDirection >= 360.0) { windDirection -= 360.0; }
                    double windSpeed = std::hypot(isobaricLayer.windU, isobaricLayer.windV);
                    CWindLayer windLayer(altitude, CAngle(windDirection, CAngleUnit::deg()), CSpeed(windSpeed, CSpeedUnit::m_s()), {});
                    windLayers.push_back(windLayer);
                }

                CCloudLayerList cloudLayers;
                for (const GfsCloudLayer &gfsCloudLayer : std::as_const(gfsGridPoint.cloudLayers))
                {
                    if (std::isnan(gfsCloudLayer.bottomLevelPressure) || std::isnan(gfsCloudLayer.topLevelPressure) || std::isnan(gfsCloudLayer.topLevelTemperature)) { continue; }

                    CCloudLayer cloudLayer;
                    double bottomLevelFt = calculateAltitudeFt(gfsGridPoint.pressureAtMsl, gfsCloudLayer.bottomLevelPressure, gfsCloudLayer.topLevelTemperature);
                    double topLevelFt = calculateAltitudeFt(gfsGridPoint.pressureAtMsl, gfsCloudLayer.topLevelPressure, gfsCloudLayer.topLevelTemperature);
                    cloudLayer.setBase(CAltitude(bottomLevelFt, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
                    cloudLayer.setTop(CAltitude(topLevelFt, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
                    cloudLayer.setCoveragePercent(qRound(gfsCloudLayer.totalCoverage));
                    if (gfsGridPoint.surfaceSnow > 0.0) { cloudLayer.setPrecipitation(CCloudLayer::Snow); }
                    if (gfsGridPoint.surfaceRain > 0.0) { cloudLayer.setPrecipitation(CCloudLayer::Rain); }

                    // Precipitation rate is in kg m-2 s-1, which is equal to mm/s
                    // Multiply with 3600 to convert to mm/h
                    cloudLayer.setPrecipitationRate(gfsGridPoint.surfacePrecipitationRate * 3600.0);
                    cloudLayer.setClouds(CCloudLayer::CloudsUnknown);
                    cloudLayers.push_back(cloudLayer);
                }

                auto pressureAtMsl = PhysicalQuantities::CPressure { gfsGridPoint.pressureAtMsl, PhysicalQuantities::CPressureUnit::Pa() };

                const CLatitude latitude(gfsGridPoint.latitude, CAngleUnit::deg());
                const CLongitude longitude(gfsGridPoint.longitude, CAngleUnit::deg());
                const auto position = CCoordinateGeodetic { latitude, longitude };
                const CGridPoint gridPoint({}, position, cloudLayers, temperatureLayers, {}, windLayers, pressureAtMsl);
                m_weatherGrid.push_back(gridPoint);
                if (m_weatherGrid.size() >= maxPoints)
                {
                    // too many points lead to extreme memory consumption and CPU usage
                    // we stop here, no use case so far in swift where we need that
                    BLACK_VERIFY_X(!CBuildConfig::isLocalDeveloperDebugBuild(), Q_FUNC_INFO, "Too many grid points");
                    CLogMessage(this).warning(u"Too many weather grid points: %1") << m_weatherGrid.size();
                    break;
                }
            }

            return true;
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
            float units = 0.000001f;
            float latitude1  = gfld->igdtmpl[11] * units;
            float longitude1 = gfld->igdtmpl[12] * units;
            int nres = gfld->igdtmpl[13];
            float latitude2  = gfld->igdtmpl[14] * units;
            float longitude2 = gfld->igdtmpl[15] * units;
            float dlatitude  = gfld->igdtmpl[16] * units;
            float dlongitude = gfld->igdtmpl[17] * units;

            if (latitude1 < -90.0f || latitude2 < -90.0f || latitude1 > 90.0f || latitude2 > 90.0f)
            {
                CLogMessage(this).warning(u"Invalid grid definition: lat1 = %1 - lat2 = %2") << latitude1 << latitude2;
                return;
            }
            if (longitude1 < 0.0f || longitude2 < 0.0f || longitude1 > 360.0f || longitude2 > 360.0f)
            {
                CLogMessage(this).warning(u"Invalid grid definition: lon1 = %1 - lon2 = %2") << longitude1 << longitude2;
                return;
            }
            if (npnts != nx * ny)
            {
                CLogMessage(this).warning(u"Invalid grid definition: npnts != nx * ny!");
                return;
            }

            // Scan direction is North -> South
            float north = latitude1;
            float south = latitude2;

            if (south > north)
            {
                CLogMessage(this).warning(u"Invalid grid definition: South = %1 - North = %2") << south << north;
                return;
            }

            float dy = 0.0f;
            if (ny != 1)
            {
                dy = (north - south) / (ny - 1.0f);
                if (nres & 16)
                {
                    if (fabs(dy - dlatitude) > 0.001f)
                    {
                        CLogMessage(this).warning(u"Invalid grid definition: delta latitude is inconsistent");
                        return;
                    }
                }
            }
            else
            {
                dy = 0.0;
            }

            // Scan direction is West -> East
            float west = longitude1;
            float east = longitude2;
            if (east <= west) { east += 360.0f; }
            if (east - west > 360.0f) { east -= 360.0f; }
            if (west < 0)
            {
                west += 360.0f;
                east += 360.0f;
            }

            float dx = 0;
            if (nx != 1)
            {
                dx = (east - west) / (nx - 1);
                dx = fabs(dx);
                if (nres & 32)
                {
                    if (fabs(dx - fabs(dlongitude)) > 0.001f)
                    {
                        CLogMessage(this).warning(u"Invalid grid definition: delta longitude is inconsistent");
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
                        if (gridPoint.longitude >= 360.0f) { gridPoint.longitude -= 360.0f; }
                        if (gridPoint.longitude  <   0.0f) { gridPoint.longitude += 360.0f; }
                        gridPoint.fieldPosition = ix + i;
                        const CCoordinateGeodetic gridPointPosition(gridPoint.latitude, gridPoint.longitude, 0);
                        if (m_maxRange == CLength())
                        {
                            m_gfsWeatherGrid.append(gridPoint);
                        }
                        else
                        {
                            for (const CGridPoint &fixedGridPoint : std::as_const(m_grid))
                            {
                                const CLength distance = calculateGreatCircleDistance(gridPointPosition, fixedGridPoint.getPosition());
                                if (distance.isNull())
                                {
                                    BLACK_VERIFY_X(!CBuildConfig::isLocalDeveloperDebugBuild(), Q_FUNC_INFO, "Suspicious value, why is that?");
                                }
                                else
                                {
                                    if (distance < m_maxRange)
                                    {
                                        m_gfsWeatherGrid.append(gridPoint);
                                        break;
                                    }
                                }
                            }
                        }
                    } // for
                } // for
            } // if
        }

        void CWeatherDataGfs::handleProductDefinitionTemplate40(const gribfield *gfld)
        {
            if (gfld->ipdtlen != 15)
            {
                CLogMessage(this).warning(u"Template 4.0 has wrong length");
                return;
            }

            // https://www.nco.ncep.noaa.gov/pmb/docs/grib2/grib2_doc/grib2_temp4-0.shtml
            g2int parameterCategory = gfld->ipdtmpl[0];
            g2int parameterNumber = gfld->ipdtmpl[1];
            g2int typeFirstFixedSurface = gfld->ipdtmpl[9];
            g2int valueFirstFixedSurface = gfld->ipdtmpl[11];

            std::array<g2int, 2> key { { parameterCategory, parameterNumber } };
            // Make sure the key exists
            if (!m_grib2ParameterTable.contains(key))
            {
                CLogMessage(this).warning(u"Unknown GRIB2 parameter: %1 - %2") << parameterCategory << parameterNumber;
                return;
            }

            float level = 0.0;

            switch (typeFirstFixedSurface)
            {
            case GroundOrWaterSurface: level = 0.0; break;
            case IsobaricSurface: level = valueFirstFixedSurface; break;
            case MeanSeaLevel: level = 0.0; break;
            default: CLogMessage(this).warning(u"Unexpected first fixed surface type: %1") << typeFirstFixedSurface; return;
            }

            auto parameterValue = m_grib2ParameterTable[key];
            switch (parameterValue.code)
            {
            case TMP: setTemperature(gfld->fld, level); break;
            case RH: setHumidity(gfld->fld, level); break;
            case UGRD: setWindU(gfld->fld, level); break;
            case VGRD: setWindV(gfld->fld, level); break;
            case PRMSL: setPressureAtMsl(gfld->fld); break;
            case PRES: /* Do nothing */ break;
            case TCDC: /* Do nothing */ break;
            case PRATE: /* Do nothing */ break;
            case CSNOW: break;
            case CRAIN: break;
            default: CLogMessage(this).error(u"Unexpected parameterValue in Template 4.0: %1 (%2)") << parameterValue.code << parameterValue.name; return;
            }
        }

        void CWeatherDataGfs::handleProductDefinitionTemplate48(const gribfield *gfld)
        {
            if (gfld->ipdtlen != 29)
            {
                CLogMessage(this).warning(u"Template 4.8 has wrong length.");
                return;
            }

            g2int parameterCategory = gfld->ipdtmpl[0];
            g2int parameterNumber = gfld->ipdtmpl[1];
            g2int typeFirstFixedSurface = gfld->ipdtmpl[9];

            std::array<g2int, 2> key { { parameterCategory, parameterNumber } };
            // Make sure the key exists
            if (!m_grib2ParameterTable.contains(key))
            {
                CLogMessage(this).warning(u"Unknown GRIB2 parameter: %1 - %2") << parameterCategory << parameterNumber;
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
            case TCDC: setCloudCoverage(gfld->fld, grib2CloudLevelHash.value(typeFirstFixedSurface)); break;
            case PRES: setCloudLevel(gfld->fld, typeFirstFixedSurface, grib2CloudLevelHash.value(typeFirstFixedSurface)); break;
            case PRATE: setPrecipitationRate(gfld->fld); break;
            case CRAIN: setSurfaceRain(gfld->fld); break;
            case CSNOW: setSurfaceSnow(gfld->fld); break;
            case TMP: setCloudTemperature(gfld->fld, typeFirstFixedSurface, grib2CloudLevelHash.value(typeFirstFixedSurface)); break;
            default: CLogMessage(this).warning(u"Unexpected parameterValue in Template 4.8: %1 (%2)") << parameterValue.code << parameterValue.name; return;
            }
        }

        void CWeatherDataGfs::setTemperature(const g2float *fld, float level)
        {
            for (auto &gridPoint : m_gfsWeatherGrid)
            {
                if (level > 0) { gridPoint.isobaricLayers[level].temperature = fld[gridPoint.fieldPosition]; }
            }
        }

        void CWeatherDataGfs::setHumidity(const g2float *fld, float level)
        {
            for (auto &gridPoint : m_gfsWeatherGrid)
            {
                gridPoint.isobaricLayers[level].relativeHumidity = fld[gridPoint.fieldPosition];
            }
        }

        void CWeatherDataGfs::setWindV(const g2float *fld, float level)
        {
            for (auto &gridPoint : m_gfsWeatherGrid)
            {
                gridPoint.isobaricLayers[level].windV = fld[gridPoint.fieldPosition];
            }
        }

        void CWeatherDataGfs::setWindU(const g2float *fld, float level)
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
                if (fld[gridPoint.fieldPosition] > 0.0f) { gridPoint.cloudLayers[level].totalCoverage = fld[gridPoint.fieldPosition]; }
            }
        }

        void CWeatherDataGfs::setCloudLevel(const g2float *fld, int surfaceType, int level)
        {
            for (auto &gridPoint : m_gfsWeatherGrid)
            {
                static const g2float minimumLevel = 1000.0;
                float levelPressure = std::numeric_limits<float>::quiet_NaN();
                g2float fieldValue = fld[gridPoint.fieldPosition];
                // A value of 9.999e20 is undefined. Check that the pressure value is below
                if (fieldValue < 9.998e20f && fieldValue > minimumLevel) { levelPressure = fld[gridPoint.fieldPosition]; }
                switch (surfaceType)
                {
                case LowCloudBottomLevel:
                case MiddleCloudBottomLevel:
                case HighCloudBottomLevel:
                    gridPoint.cloudLayers[level].bottomLevelPressure = levelPressure;
                    break;
                case LowCloudTopLevel:
                case MiddleCloudTopLevel:
                case HighCloudTopLevel:
                    gridPoint.cloudLayers[level].topLevelPressure = levelPressure;
                    break;
                default:
                    Q_ASSERT(false);
                    break;
                }
            }
        }

        void CWeatherDataGfs::setCloudTemperature(const g2float *fld, int surfaceType, int level)
        {
            for (auto &gridPoint : m_gfsWeatherGrid)
            {
                float temperature = std::numeric_limits<float>::quiet_NaN();
                g2float fieldValue = fld[gridPoint.fieldPosition];
                if (fieldValue < 9.998e20f) { temperature = fld[gridPoint.fieldPosition]; }
                switch (surfaceType)
                {
                case LowCloudTopLevel:
                case MiddleCloudTopLevel:
                case HighCloudTopLevel:
                    gridPoint.cloudLayers[level].topLevelTemperature = temperature;
                    break;
                default:
                    Q_ASSERT(false);
                    break;
                }
            }
        }

        void CWeatherDataGfs::setPressureAtMsl(const g2float *fld)
        {
            for (auto &gridPoint : m_gfsWeatherGrid)
            {
                gridPoint.pressureAtMsl = fld[gridPoint.fieldPosition];
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
            // https://en.wikipedia.org/wiki/Dew_point#Calculating_the_dew_point
            const double a =  6.112;
            const double b = 17.67;
            const double c = 243.5;

            double temperatureInCelsius = temperature.value(CTemperatureUnit::C());
            double saturationVaporPressure = a * std::exp((b * temperatureInCelsius) / (c + temperatureInCelsius));

            double vaporPressure = (relativeHumidity / 100.0) * saturationVaporPressure;
            // std::log(0.0) is not defined. Hence we use the smallest value possible close to 0.0, if RH is 0.0.
            // In real weather, RH 0.0 % will never exist.
            vaporPressure = qMax(vaporPressure, std::numeric_limits<double>::min());

            double dewPointInCelsius = c * std::log(vaporPressure / a) / (b - std::log(vaporPressure / a));
            return { dewPointInCelsius, CTemperatureUnit::C() };
        }

        BlackCore::IWeatherData *CWeatherDataGfsFactory::create(QObject *parent)
        {
            return new CWeatherDataGfs(parent);
        }

    } // namespace
} // namespace
