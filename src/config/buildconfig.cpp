// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE

#include "buildconfig.h"

#include <QCoreApplication>
#include <QLocale>
#include <QOperatingSystemVersion>
#include <QStringList>
#include <QSysInfo>
#include <QtGlobal>

namespace swift::config
{
    const QString &CBuildConfig::swiftGuiExecutableName()
    {
        static const QString s("swiftguistd");
        return s;
    }

    const QString &CBuildConfig::swiftCoreExecutableName()
    {
        static const QString s("swiftcore");
        return s;
    }

    const QString &CBuildConfig::swiftDataExecutableName()
    {
        static const QString s("swiftdata");
        return s;
    }

    const QString &CBuildConfig::getPlatformString()
    {
        static const QString p([] {
            if constexpr (CBuildConfig::isRunningOnLinuxPlatform()) return QString("Linux");
            if constexpr (CBuildConfig::isRunningOnMacOSPlatform()) return QString("MacOS");
            if constexpr (CBuildConfig::isRunningOnWindowsNtPlatform())
            {
                if (CBuildConfig::buildWordSize() == 32) return QString("Win32");
                if (CBuildConfig::buildWordSize() == 64) return QString("Win64");
            }
            return QString("unknown");
        }());
        return p;
    }

    namespace Private
    {
        bool isLocalDeveloperBuildImpl()
        {
            if constexpr (!CBuildConfig::isDebugBuild()) { return false; }
            const QString p = QCoreApplication::applicationDirPath().toLower();

            // guessing, feel free to add path checks
            if (p.contains("build")) { return true; }
            if (p.contains("msvc")) { return true; }
            return false;
        }
    } // namespace Private

    bool CBuildConfig::isLocalDeveloperDebugBuild()
    {
        static const bool devBuild = Private::isLocalDeveloperBuildImpl();
        return devBuild;
    }

    const QString &CBuildConfig::compiledWithInfo()
    {
        static QString infoShort;
        QStringList sl;
        if constexpr (CBuildConfig::isCompiledWithFs9Support()) { sl << "FS9"; }
        if constexpr (CBuildConfig::isCompiledWithFsxSupport()) { sl << "FSX"; }
        if constexpr (CBuildConfig::isCompiledWithMSFSSupport()) { sl << "MSFS2020"; }
        if constexpr (CBuildConfig::isCompiledWithMSFS2024Support()) { sl << "MSFS2024"; }
        if constexpr (CBuildConfig::isCompiledWithXPlaneSupport()) { sl << "XPlane"; }
        if constexpr (CBuildConfig::isCompiledWithP3DSupport()) { sl << "P3D"; }
        if constexpr (CBuildConfig::isCompiledWithFGSupport()) { sl << "FG"; }
        infoShort = sl.join(", ");
        if (infoShort.isEmpty()) { infoShort = "<none>"; }
        return infoShort;
    }

    const QString &CBuildConfig::gitHubRepoUrl()
    {
        static const QString url = "https://github.com/swift-project/pilotclient/";
        return url;
    }

    const QString &CBuildConfig::gitHubRepoApiUrl()
    {
        static const QString url = "https://api.github.com/repos/swift-project/pilotclient/";
        return url;
    }

    const QString &CBuildConfig::buildDateAndTime()
    {
        // http://en.cppreference.com/w/cpp/preprocessor/replace#Predefined_macros
        static const QString buildDateAndTime = QString(__DATE__ " " __TIME__).simplified();
        return buildDateAndTime;
    }

    const QVersionNumber &CBuildConfig::getVersion()
    {
        static const QVersionNumber v { versionMajor(), versionMinor(), versionRevision() };
        return v;
    }

    const QString &CBuildConfig::getVersionString()
    {
        static const QString s(getVersion().toString());
        return s;
    }

    const QString &CBuildConfig::getShortVersionString()
    {
        static const QVersionNumber v { versionMajor(), versionMinor() };
        static const QString s(v.toString());
        return s;
    }

    const QString &CBuildConfig::getVersionStringPlatform()
    {
        static const QString s = getPlatformString() % u' ' % getVersionString();
        return s;
    }

    const QStringList &CBuildConfig::getBuildAbiParts()
    {
        static const QStringList parts = QSysInfo::buildAbi().split('-');
        return parts;
    }

    namespace Private
    {
        int buildWordSizeImpl()
        {
            if (CBuildConfig::getBuildAbiParts().length() < 3) { return -1; }
            const QString abiWs = CBuildConfig::getBuildAbiParts()[2];
            if (abiWs.contains("32")) { return 32; }
            if (abiWs.contains("64")) { return 64; }
            return -1;
        }
    } // namespace Private

    int CBuildConfig::buildWordSize()
    {
        static const int bws = Private::buildWordSizeImpl();
        return bws;
    }
} // namespace swift::config

//! \endcond
