// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/simulation/fsx/simconnectutilities.h"
#include "blackmisc/stringutils.h"

#include <QCoreApplication>
#include <QFile>
#include <QFlags>
#include <QIODevice>
#include <QLatin1String>
#include <QMetaEnum>
#include <QMetaObject>
#include <QTextStream>
#include <QStandardPaths>
#include <QStringBuilder>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Weather;

namespace BlackMisc::Simulation::Fsx
{
    CSimConnectUtilities::CSimConnectUtilities() {}

    const QString &CSimConnectUtilities::simConnectFilename()
    {
        static const QString fn("SimConnect.cfg");
        return fn;
    }

    const QString &CSimConnectUtilities::getSwiftLocalSimConnectCfgFilename()
    {
        static const QString n = CFileUtils::appendFilePaths(QCoreApplication::applicationDirPath(), simConnectFilename());
        return n;
    }

    const QString &CSimConnectUtilities::getUserSimConnectCfgFilename()
    {
        static const QString n = CFileUtils::appendFilePaths(QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory), simConnectFilename());
        return n;
    }

    bool CSimConnectUtilities::hasSwiftLocalSimConnectCfgFile()
    {
        const QFile f(getSwiftLocalSimConnectCfgFilename());
        return f.exists();
    }

    bool CSimConnectUtilities::hasUserSimConnectCfgFile()
    {
        const QFile f(getUserSimConnectCfgFilename());
        return f.exists();
    }

    QSharedPointer<QSettings> CSimConnectUtilities::simConnectFileAsSettings(const QString &fileName)
    {
        QSharedPointer<QSettings> sp;
        const QFile file(fileName);
        if (!file.exists()) { return sp; }
        sp.reset(new QSettings(fileName, QSettings::IniFormat));
        return sp;
    }

    QString CSimConnectUtilities::ipAddress(const QSettings *simConnectSettings)
    {
        if (!simConnectSettings) { return {}; }
        return simConnectSettings->value("SimConnect/Address").toString();
    }

    int CSimConnectUtilities::ipPort(const QSettings *simConnectSettings)
    {
        if (!simConnectSettings) { return -1; }
        return simConnectSettings->value("SimConnect/Port", QVariant::fromValue(-1)).toInt();
    }

    bool CSimConnectUtilities::writeSimConnectCfg(const QString &fileName, const QString &ip, int port)
    {
        const QString sc = CSimConnectUtilities::simConnectCfg(ip, port);
        QFile file(fileName);
        bool success = false;
        if ((success = file.open(QIODevice::WriteOnly | QIODevice::Text)))
        {
            QTextStream out(&file);
            out << sc;
            file.close();
        }
        return success;
    }

    QString CSimConnectUtilities::simConnectCfg(const QString &ip, int port)
    {
        const QString sc = QStringLiteral("[SimConnect]\nProtocol=Ipv4\nAddress=%1\nPort=%2\n"
                                          "MaxReceiveSize=4096\nDisableNagle=0")
                               .arg(ip)
                               .arg(port);
        return sc;
    }

    QString CSimConnectUtilities::resolveEnumToString(const DWORD id, const char *enumName)
    {
        const int i = CSimConnectUtilities::staticMetaObject.indexOfEnumerator(enumName);
        if (i < 0) { return QStringLiteral("No enumerator for %1").arg(enumName); }
        const QMetaEnum m = CSimConnectUtilities::staticMetaObject.enumerator(i);
        const char *k = m.valueToKey(static_cast<int>(id));
        return (k) ? QLatin1String(k) : QStringLiteral("Id '%1' not found for %2").arg(id).arg(enumName);
    }

    const QString &CSimConnectUtilities::simConnectIniFilename()
    {
        static const QString n("SimConnect.ini");
        return n;
    }

    QString CSimConnectUtilities::simConnectExceptionToString(const DWORD id)
    {
        return CSimConnectUtilities::resolveEnumToString(id, "SIMCONNECT_EXCEPTION");
    }

    QString CSimConnectUtilities::simConnectSurfaceTypeToString(const DWORD type, bool beautify)
    {
        QString sf = CSimConnectUtilities::resolveEnumToString(type, "SIMCONNECT_SURFACE");
        return beautify ? sf.replace('_', ' ') : sf;
    }

    QStringList CSimConnectUtilities::getSimConnectIniFileDirectories()
    {
        const QString docDir = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory);
        if (docDir.isEmpty()) return QStringList();

        QDir directory(docDir);
        directory.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
        const QStringList dirList = directory.entryList();
        QStringList simDirs;
        for (const QString &dir : dirList)
        {
            if (dir.contains("Flight Simulator", Qt::CaseInsensitive) || dir.contains("Prepar3D", Qt::CaseInsensitive))
            {
                simDirs.push_back(CFileUtils::appendFilePaths(docDir, dir));
            }
        }

        // gets the latest P3D as first
        simDirs.sort();
        std::reverse(std::begin(simDirs), std::end(simDirs));
        return simDirs;
    }

    QStringList CSimConnectUtilities::getSimConnectIniFiles()
    {
        QStringList files;
        for (const QString &dir : getSimConnectIniFileDirectories())
        {
            const QFileInfo f(CFileUtils::appendFilePaths(dir, simConnectIniFilename()));
            if (f.exists()) { files.push_back(f.absoluteFilePath()); }
        }
        return files;
    }

    QString CSimConnectUtilities::getSimConnectIniFileDirectory(CSimulatorInfo &simulator)
    {
        static const QString docDir = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "", QStandardPaths::LocateDirectory);
        if (docDir.isEmpty()) { return {}; }
        if (!simulator.isSingleSimulator() || !simulator.isFsxP3DFamily()) return {};

        const QString iniDir = CFileUtils::appendFilePaths(docDir, simulator.isP3D() ? "Prepar3D v4 Files" : "Flight Simulator X Files");
        if (getSimConnectIniFileDirectories().isEmpty()) { return iniDir; }

        for (const QString &dir : getSimConnectIniFileDirectories())
        {
            if (simulator.isP3D())
            {
                if (dir.contains("Prepar3D", Qt::CaseInsensitive)) { return dir; }
            }
            else if (simulator.isFSX())
            {
                if (dir.contains("Flight Simulator", Qt::CaseInsensitive)) { return dir; }
            }
        }

        return iniDir;
    }

    QString CSimConnectUtilities::simConnectReceiveIdToString(DWORD type)
    {
        const QString ri = CSimConnectUtilities::resolveEnumToString(type, "SIMCONNECT_RECV_ID");
        return ri;
    }

    int CSimConnectUtilities::lightsToLightStates(const CAircraftLights &lights)
    {
        int lightMask = 0;
        if (lights.isBeaconOn()) { lightMask |= Beacon; }
        if (lights.isLandingOn()) { lightMask |= Landing; }
        if (lights.isLogoOn()) { lightMask |= Logo; }
        if (lights.isNavOn()) { lightMask |= Nav; }
        if (lights.isStrobeOn()) { lightMask |= Strobe; }
        if (lights.isTaxiOn()) { lightMask |= Taxi; }
        return lightMask;
    }

    QString CSimConnectUtilities::convertToSimConnectMetar(const CGridPoint &gridPoint, bool isFSX, bool useWindLayers, bool useVisibilityLayers, bool useCloudLayers, bool useTempLayers)
    {
        // STATION ID
        Q_ASSERT(!gridPoint.getIdentifier().isEmpty());
        QString simconnectMetar = gridPoint.getIdentifier();

        // SURFACE WINDS/WINDS ALOFT
        const CWindLayerList windLayers = useWindLayers ? gridPoint.getWindLayers().sortedBy(&CWindLayer::getLevel) : CWindLayerList();
        simconnectMetar += windsToSimConnectMetar(windLayers, isFSX);

        // VISIBILITY
        const CVisibilityLayerList visibilityLayers = useVisibilityLayers ? gridPoint.getVisibilityLayers().sortedBy(&CVisibilityLayer::getBase) : CVisibilityLayerList();
        simconnectMetar += visibilitiesToSimConnectMetar(visibilityLayers);

        // PRESENT CONDITIONS
        // todo

        // PARTIAL OBSCURATION
        // todo

        // SKY CONDITIONS
        const CCloudLayerList cloudLayers = useCloudLayers ? gridPoint.getCloudLayers().sortedBy(&CCloudLayer::getBase) : CCloudLayerList();
        simconnectMetar += cloudsToSimConnectMetar(cloudLayers);

        // TEMPERATURE
        const CTemperatureLayerList temperatureLayers = useTempLayers ? gridPoint.getTemperatureLayers().sortedBy(&CTemperatureLayer::getLevel) : CTemperatureLayerList();
        simconnectMetar += temperaturesToSimConnectMetar(temperatureLayers);

        // ALTIMETER
        // Format:
        // QNNNN
        // Q = specifier for altimeter in millibars
        // NNNN = altimeter in millibars
        if (!gridPoint.getPressureAtMsl().isNull())
        {
            static const QString arg1s(" Q%1");
            const int altimeter = gridPoint.getPressureAtMsl().valueInteger(CPressureUnit::mbar());
            simconnectMetar += arg1s.arg(altimeter, 4, 10, QLatin1Char('0'));
        }
        return simconnectMetar.simplified();
    }

    void CSimConnectUtilities::registerMetadata()
    {
        qRegisterMetaType<CSimConnectUtilities::SIMCONNECT_EXCEPTION>();
        qRegisterMetaType<CSimConnectUtilities::SIMCONNECT_SURFACE>();
    }

    QString CSimConnectUtilities::windsToSimConnectMetar(const CWindLayerList &windLayers, bool isFSX)
    {
        static const QString arg1s("%1");

        QString simConnectWinds;
        bool surface = true;
        for (const CWindLayer &windLayer : windLayers)
        {
            simConnectWinds += QLatin1Char(' ');

            // Format:
            // DDDSSSUUU (steady)
            // DDDSSSGXXUUU (gusts)
            if (windLayer.isDirectionVariable())
            {
                // DDD = VRB for variable
                simConnectWinds += QLatin1String("VRB");
            }
            else
            {
                if (!windLayer.getSpeed().isNull() && !windLayer.getDirection().isNull())
                {
                    const int speedKts = windLayer.getSpeed().valueInteger(CSpeedUnit::kts());
                    const int directionDeg = windLayer.getDirection().valueInteger(CAngleUnit::deg());

                    simConnectWinds += arg1s.arg(directionDeg, 3, 10, QLatin1Char('0')) % // DDD = Direction (0-360 degrees)
                                       arg1s.arg(speedKts, 3, 10, QLatin1Char('0')); // SSS = Speed
                }
            }
            // XX = Gust speed
            const int gustSpeedKts = windLayer.getGustSpeed().valueInteger(CSpeedUnit::kts());
            if (gustSpeedKts > 0) { simConnectWinds += u'G' % arg1s.arg(gustSpeedKts, 2, 10, QLatin1Char('0')); }

            // UUU = Speed units
            simConnectWinds += QStringLiteral("KT");

            if (surface)
            {
                // Surface extension:
                // &DNNNNTS
                static const QString surfaceWinds =
                    "&D" // D = specifier for surface layer
                    "305" // Surface default depth is 1000 feet or 305m
                    "NG"; // We don't have turbulence or wind shear information, hence we use the defaults
                simConnectWinds += surfaceWinds;
                surface = false;
            }
            else
            {
                CAltitude altitude = windLayer.getLevel();

                // this seems to crash FSX, P3D works
                // https://www.fsdeveloper.com/forum/threads/setting-winds-aloft.18862/
                if (!altitude.isNull() && !isFSX)
                {
                    altitude.toMeanSeaLevel();
                    int altitudeValueMeters = altitude.valueInteger(CLengthUnit::m());

                    // Winds aloft extension:
                    // &ANNNNTS
                    simConnectWinds +=
                        u"&A" % // A = specifier for altitude above mean sea-level (MSL)
                        arg1s.arg(altitudeValueMeters, 4, 10, QLatin1Char('0')) % // NNNN = depth (height) in meters.
                        u"NG"; // We don't have turbulence or wind shear information, hence we use the defaults
                }
            }
        }

        return QStringLiteral(" ") % simConnectWinds.simplified();
    }

    QString CSimConnectUtilities::visibilitiesToSimConnectMetar(const CVisibilityLayerList &visibilityLayers)
    {
        // There are several format options, we use the meter format:
        // NNNND&BXXXX&DYYYY
        QString simconnectVisibilities;
        for (const auto &visibilityLayer : visibilityLayers)
        {
            simconnectVisibilities += QLatin1Char(' ');

            // NNNN = in meters
            auto visibility = visibilityLayer.getVisibility().valueInteger(CLengthUnit::m());
            visibility = qMin(9999, visibility);
            simconnectVisibilities += QStringLiteral("%1").arg(visibility, 4, 10, QLatin1Char('0'));

            // D = directional variation
            // We set NDV - no directional variation
            simconnectVisibilities += QLatin1String("NDV");

            // XXXX = base of visibility layer in meters
            const auto base = visibilityLayer.getBase().valueInteger(CLengthUnit::m());
            simconnectVisibilities += QStringLiteral("&B%1").arg(base, 4, 10, QLatin1Char('0'));

            // YYYY = depth of visibility layer in meters
            const auto depth = visibilityLayer.getTop().valueInteger(CLengthUnit::m());
            simconnectVisibilities += QStringLiteral("&D%1").arg(depth, 4, 10, QLatin1Char('0'));
        }
        return simconnectVisibilities;
    }

    QString CSimConnectUtilities::cloudsToSimConnectMetar(const CCloudLayerList &cloudLayers)
    {
        // Format:
        // CCCNNN&BXXXX&DYYYY
        QString simconnectClouds;
        static const QString arg1s("%1");
        for (const CCloudLayer &cloudLayer : cloudLayers)
        {
            simconnectClouds += QLatin1Char(' ');

            // CCC = Coverage string
            switch (cloudLayer.getCoverage())
            {
            case CCloudLayer::None: simconnectClouds += QLatin1String("CLR"); break;
            case CCloudLayer::Few: simconnectClouds += QLatin1String("FEW"); break;
            case CCloudLayer::Broken: simconnectClouds += QLatin1String("BKN"); break;
            case CCloudLayer::Overcast: simconnectClouds += QLatin1String("OVC"); break;
            case CCloudLayer::Scattered:
            default:
                simconnectClouds += QLatin1String("SCT");
            }

            // NNN = coded height
            // If NNN is 999 the level is 100,000 feet, otherwise it is 100 x NNN in feet
            auto level = cloudLayer.getTop().valueInteger(CLengthUnit::ft()) / 100;
            // Ignore clouds higher than 99900 feet
            if (level > 999) { continue; }
            simconnectClouds += arg1s.arg(level, 3, 10, QLatin1Char('0')) %
                                u'&';

            // TT = Cloud type
            switch (cloudLayer.getClouds())
            {
            case CCloudLayer::Cirrus: simconnectClouds += QLatin1String("CI"); break;
            case CCloudLayer::Stratus: simconnectClouds += QLatin1String("ST"); break;
            case CCloudLayer::Thunderstorm: simconnectClouds += QLatin1String("CB"); break;
            case CCloudLayer::Cumulus:
            default:
                simconnectClouds += QLatin1String("CU");
            }

            // 000 - Unused.
            simconnectClouds += QLatin1String("000");

            // F = Top of cloud
            // Default to F - flat
            simconnectClouds += QLatin1Char('F');
            // T = Turbulence
            // N - None (default)
            simconnectClouds += QLatin1Char('N');

            // P = precipitation rate
            // http://wiki.sandaysoft.com/a/Rain_measurement#Rain_Rate
            auto precipitationRate = cloudLayer.getPrecipitationRate();
            // Very light rain: precipitation rate is < 0.25 mm/hour
            if (precipitationRate < 0.25) { simconnectClouds += QLatin1Char('V'); }
            // Light rain: precipitation rate is between 0.25mm/hour and 1.0mm/hour
            else if (precipitationRate >= 0.25 && precipitationRate < 1.0) { simconnectClouds += QLatin1Char('L'); }
            // Moderate rain: precipitation rate is between 1.0 mm/hour and 4.0 mm/hour
            else if (precipitationRate >= 1.0 && precipitationRate < 4.0) { simconnectClouds += QLatin1Char('M'); }
            // Heavy rain: recipitation rate is between 4.0 mm/hour and 16.0 mm/hour
            else if (precipitationRate >= 4.0 && precipitationRate < 16.0) { simconnectClouds += QLatin1Char('H'); }
            // Very heavy rain: precipitation rate is > 16.0 mm/hour
            else if (precipitationRate >= 16.0) { simconnectClouds += QLatin1Char('D'); }

            // Q = Type of precipitation
            switch (cloudLayer.getPrecipitation())
            {
            case CCloudLayer::Rain: simconnectClouds += QLatin1Char('R'); break;
            case CCloudLayer::Snow: simconnectClouds += QLatin1Char('S'); break;
            default: simconnectClouds += QLatin1Char('N');
            }

            // BBB = Coded base height
            // the precipitation ends at this height, set to 0 for it to land on the ground
            simconnectClouds += QLatin1String("000");

            // I = icing rate
            // Set to None for now
            simconnectClouds += QLatin1String("N");
        }
        return simconnectClouds;
    }

    QString CSimConnectUtilities::temperaturesToSimConnectMetar(const CTemperatureLayerList &temperatureLayers)
    {
        // Format:
        // TT/DD&ANNNNN
        QString simconnectTemperatures;
        static const QString arg1s("%1");

        for (const CTemperatureLayer &temperatureLayer : temperatureLayers)
        {
            simconnectTemperatures += QLatin1Char(' ');

            const int temperature = temperatureLayer.getTemperature().valueInteger(CTemperatureUnit::C());
            const int dewPoint = temperatureLayer.getDewPoint().valueInteger(CTemperatureUnit::C());
            const int altitude = temperatureLayer.getLevel().valueInteger(CLengthUnit::m());

            simconnectTemperatures += arg1s.arg(temperature, 2, 10, QLatin1Char('0')) % // TT = temperature in Celsius
                                      u'/' %
                                      arg1s.arg(dewPoint, 2, 10, QLatin1Char('0')) % // DD = dewpoint in Celsius
                                      u"&A" %
                                      arg1s.arg(altitude, 5, 10, QLatin1Char('0')); // NNNNN = altitude of the temperatures in meters.
        }
        return simconnectTemperatures;
    }

    CWinDllUtils::DLLInfo CSimConnectUtilities::simConnectDllInfo()
    {
        const QList<CWinDllUtils::ProcessModule> modules = CWinDllUtils::getModules(-1, "simconnect");
        if (modules.isEmpty())
        {
            CWinDllUtils::DLLInfo info;
            info.errorMsg = "No SimConnect.dll loaded";
            return info;
        }
        return CWinDllUtils::getDllInfo(modules.first().executable);
    }
} // namespace
