// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE

#include "buildconfig.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLocale>
#include <QStandardPaths>
#include <QStringList>
#include <QStringBuilder>
#include <QtGlobal>
#include <QSysInfo>
#include <QOperatingSystemVersion>

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

    bool CBuildConfig::isKnownExecutableName(const QString &executable)
    {
        return executable == CBuildConfig::swiftCoreExecutableName() ||
               executable == CBuildConfig::swiftDataExecutableName() ||
               executable == CBuildConfig::swiftGuiExecutableName();
    }

    bool CBuildConfig::isRunningOnWindows10()
    {
        if (!CBuildConfig::isRunningOnWindowsNtPlatform()) { return false; }
        return (QOperatingSystemVersion::current() >= QOperatingSystemVersion::Windows10);
    }

    const QString &CBuildConfig::getPlatformString()
    {
        static const QString p([] {
            if (CBuildConfig::isRunningOnLinuxPlatform()) return QString("Linux");
            if (CBuildConfig::isRunningOnMacOSPlatform()) return QString("MacOS");
            if (CBuildConfig::isRunningOnWindowsNtPlatform())
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
            if (!CBuildConfig::isDebugBuild()) { return false; }
            const QString p = QCoreApplication::applicationDirPath().toLower();

            // guessing, feel free to add path checks
            if (p.contains("build")) { return true; }
            if (p.contains("msvc")) { return true; }
            return false;
        }
    }

    bool CBuildConfig::isLocalDeveloperDebugBuild()
    {
        static const bool devBuild = Private::isLocalDeveloperBuildImpl();
        return devBuild;
    }

    static QString boolToYesNo(bool v)
    {
        return v ? QStringLiteral("yes") : QStringLiteral("no");
    }

    const QString &CBuildConfig::compiledWithInfo(bool shortVersion)
    {
        if (shortVersion)
        {
            static QString infoShort;
            if (infoShort.isEmpty())
            {
                QStringList sl;
                if (CBuildConfig::isCompiledWithCore()) { sl << "Core"; }
                if (CBuildConfig::isCompiledWithSound()) { sl << "Sound"; }
                if (CBuildConfig::isCompiledWithInput()) { sl << "Input"; }
                if (CBuildConfig::isCompiledWithGui()) { sl << "Gui"; }
                if (CBuildConfig::isCompiledWithFs9Support()) { sl << "FS9"; }
                if (CBuildConfig::isCompiledWithFsxSupport()) { sl << "FSX"; }
                if (CBuildConfig::isCompiledWithXPlaneSupport()) { sl << "XPlane"; }
                if (CBuildConfig::isCompiledWithP3DSupport()) { sl << "P3D"; }
                if (CBuildConfig::isCompiledWithFGSupport()) { sl << "FG"; }
                infoShort = sl.join(", ");
                if (infoShort.isEmpty()) { infoShort = "<none>"; }
            }
            return infoShort;
        }
        else
        {
            static QString infoLong;
            if (infoLong.isEmpty())
            {
                infoLong = infoLong.append("Core: ").append(boolToYesNo(isCompiledWithCore()));
                infoLong = infoLong.append(" Input: ").append(boolToYesNo(isCompiledWithInput()));
                infoLong = infoLong.append(" Sound: ").append(boolToYesNo(isCompiledWithSound()));
                infoLong = infoLong.append(" GUI: ").append(boolToYesNo(isCompiledWithGui()));

                infoLong = infoLong.append(" FS9: ").append(boolToYesNo(isCompiledWithFs9Support()));
                infoLong = infoLong.append(" FSX: ").append(boolToYesNo(isCompiledWithFsxSupport()));
                infoLong = infoLong.append(" P3D: ").append(boolToYesNo(isCompiledWithP3DSupport()));
                infoLong = infoLong.append(" XPlane: ").append(boolToYesNo(isCompiledWithXPlaneSupport()));
                infoLong = infoLong.append(" FG: ").append(boolToYesNo(isCompiledWithFGSupport()));
            }
            return infoLong;
        }
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
    }

    int CBuildConfig::buildWordSize()
    {
        static const int bws = Private::buildWordSizeImpl();
        return bws;
    }
} // ns

//! \endcond
