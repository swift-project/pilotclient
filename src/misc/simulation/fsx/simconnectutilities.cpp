// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/fsx/simconnectutilities.h"

#include <QCoreApplication>
#include <QFile>
#include <QFlags>
#include <QIODevice>
#include <QLatin1String>
#include <QMetaEnum>
#include <QMetaObject>
#include <QStandardPaths>
#include <QStringBuilder>
#include <QTextStream>

#include "misc/stringutils.h"

using namespace swift::misc;
using namespace swift::misc::aviation;

namespace swift::misc::simulation::fsx
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

    void CSimConnectUtilities::registerMetadata()
    {
        qRegisterMetaType<CSimConnectUtilities::SIMCONNECT_EXCEPTION>();
        qRegisterMetaType<CSimConnectUtilities::SIMCONNECT_SURFACE>();
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
} // namespace swift::misc::simulation::fsx
