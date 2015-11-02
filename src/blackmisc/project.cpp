/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "project.h"
#include "blackmisc/fileutilities.h"
#include <QStringList>
#include <QCoreApplication>
#include <QProcessEnvironment>
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/simulation/simulatorinfo.h"

#define BLACK_VERSION_STR_X(v) #v
#define BLACK_VERSION_STR(v) BLACK_VERSION_STR_X(v)

namespace BlackMisc
{
    bool CProject::isCompiledWithBlackCore()
    {
#ifdef WITH_BLACKCORE
        return true;
#else
        return false;
#endif
    }

    bool CProject::isCompiledWithBlackSound()
    {
#ifdef WITH_BLACKSOUND
        return true;
#else
        return false;
#endif
    }

    bool CProject::isCompiledWithBlackInput()
    {
#ifdef WITH_BLACKINPUT
        return true;
#else
        return false;
#endif
    }

    bool CProject::isCompiledWithFs9Support()
    {
#ifdef WITH_FS9
        return true;
#else
        return false;
#endif
    }

    bool CProject::isCompiledWithFsxSupport()
    {
#ifdef WITH_FSX
        return true;
#else
        return false;
#endif
    }

    bool CProject::isCompiledWithP3DSupport()
    {
        return isCompiledWithFsxSupport();
    }

    bool CProject::isCompiledWithMsFlightSimulatorSupport()
    {
        return isCompiledWithFs9Support() || isCompiledWithFsxSupport() || isCompiledWithP3DSupport();
    }

    bool CProject::isCompiledWithXPlaneSupport()
    {
#ifdef WITH_XPLANE
        return true;
#else
        return false;
#endif
    }

    bool CProject::isCompiledWithFlightSimulatorSupport()
    {
        return isCompiledWithFsxSupport() || isCompiledWithXPlaneSupport();
    }

    bool BlackMisc::CProject::isCompiledWithGui()
    {
#ifdef WITH_BLACKGUI
        return true;
#else
        return false;
#endif
    }

    const BlackMisc::Simulation::CSimulatorInfo &CProject::simulators()
    {
        static const BlackMisc::Simulation::CSimulatorInfo simInfo(
            isCompiledWithFsxSupport(),
            isCompiledWithFs9Support(),
            isCompiledWithXPlaneSupport(),
            isCompiledWithP3DSupport()
        );
        return simInfo;
    }

    const char *CProject::simulatorsChar()
    {
        static const QByteArray sims(simulators().toQString().toUtf8());
        return sims.constData();
    }

    const QString &CProject::version()
    {
#ifdef BLACK_VERSION
        const static QString v(BLACK_VERSION_STR(BLACK_VERSION));
#else
        const static QString v("?");
#endif
        return v;
    }

    const QString &CProject::swiftVersionString()
    {
        static const QString s(QString("swift %1").arg(version()));
        return s;
    }

    const QString &CProject::swiftVersionStringDevInfo()
    {
        if (!isRunningInDeveloperEnvironment()) { return swiftVersionString(); }
        static const QString s(swiftVersionString() + " [DEV]");
        return s;
    }

    const char *CProject::swiftVersionChar()
    {
        static const QByteArray a(swiftVersionString().toUtf8());
        return a.constData();
    }

    int CProject::versionMajor()
    {
        return getMajorMinor(0);
    }

    int CProject::versionMinor()
    {
        return getMajorMinor(1);
    }

    bool CProject::isDebugBuild()
    {
#ifdef QT_DEBUG
        return true;
#else
        return false;
#endif
    }

    bool CProject::isReleaseBuild()
    {
#ifdef QT_NO_DEBUG
        return true;
#else
        return false;
#endif
    }

    bool CProject::isBetaTest()
    {
        //! \todo however we do it
        return false;
    }

    bool CProject::isRunningOnWindowsNtPlatform()
    {
#ifdef Q_OS_WIN
        // QSysInfo::WindowsVersion only available on Win platforms
        return (QSysInfo::WindowsVersion & QSysInfo::WV_NT_based) ? true : false;
#else
        return false;
#endif
    }

    bool CProject::isRunningInDeveloperEnvironment()
    {
        static const bool dev = BlackMisc::stringToBool(envVarDevelopmentValue());
        return dev;
    }

    bool CProject::useDevelopmentSetup()
    {
        static const QString v(envVarDevelopmentValue());
        if (v.isEmpty())
        {
            // no explicit value
            return isRunningInBetaOrDeveloperEnvironment();
        }
        else
        {
            return stringToBool(v);
        }
    }

    bool CProject::isRunningInBetaOrDeveloperEnvironment()
    {
        return isBetaTest() || isRunningInDeveloperEnvironment();
    }

    int CProject::getMajorMinor(int index)
    {
        QString v = version();
        if (v.isEmpty() || !v.contains(".")) return -1;
        bool ok;
        int vi = v.split(".")[index].toInt(&ok);
        return ok ? vi : -1;
    }

    const QString &CProject::envVarDevelopment()
    {
        static const QString s("SWIFT_DEV");
        return s;
    }

    QString CProject::envVarDevelopmentValue()
    {
        return QProcessEnvironment::systemEnvironment().value(envVarDevelopment());
    }

    const QString &CProject::envVarPrivateSetupDir()
    {
        static const QString s("SWIFT_SETUP_DIR");
        return s;
    }

    QString CProject::envVarPrivateSetupDirValue()
    {
        return QProcessEnvironment::systemEnvironment().value(envVarPrivateSetupDir());
    }

    QString getApplicationDirImpl()
    {
        QFileInfo executable(QCoreApplication::applicationFilePath());
        QDir p(executable.dir());
        return p.absolutePath();
    }

    const QString &CProject::getApplicationDir()
    {
        static const QString s(getApplicationDirImpl());
        return s;
    }

    QString getSwiftResourceDirImpl()
    {
        QDir dir(CProject::getApplicationDir());
        if (dir.cdUp()) { return dir.absolutePath(); }
        return "";
    }

    const QString &CProject::getSwiftResourceDir()
    {
        static const QString s(getSwiftResourceDirImpl());
        return s;
    }

    QString CProject::getSwiftPrivateResourceDir()
    {
        static const QString dir(envVarPrivateSetupDirValue());
        return dir;
    }

    QString getSwiftStaticDbFilesDirImpl()
    {
        QString d(CProject::getSwiftResourceDir());
        if (d.isEmpty()) { return ""; }
        QDir dir(d);
        if (dir.cd("swiftDB")) { return dir.absolutePath(); }
        return "";
    }

    const QString &CProject::getSwiftStaticDbFilesDir()
    {
        static QString s(getSwiftResourceDirImpl());
        return s;
    }

    QString getImagesDirImpl()
    {
        QString d(CProject::getSwiftResourceDir());
        if (d.isEmpty()) return "";
        return CFileUtils::appendFilePaths(d, "data/images");
    }

    const QString &CProject::getImagesDir()
    {
        static const QString s(getImagesDirImpl());
        return s;
    }

    QString CProject::getEnvironmentVariables(const QString &separator)
    {
        QString e(envVarDevelopment());
        e = e.append(": ").append(envVarDevelopmentValue());
        e = e.append(separator);

        e = e.append(envVarPrivateSetupDir());
        e = e.append(": ").append(envVarPrivateSetupDirValue());
        return e;
    }

    const QString &CProject::compiledWithInfo(bool shortVersion)
    {
        if (shortVersion)
        {
            static QString infoShort;
            if (infoShort.isEmpty())
            {
                QStringList sl;
                if (isCompiledWithBlackCore())     { sl << "BlackCore"; }
                if (isCompiledWithBlackSound())    { sl << "BlackSound"; }
                if (isCompiledWithBlackInput())    { sl << "BlackInput"; }
                if (isCompiledWithGui())           { sl << "BlackGui"; }
                if (isCompiledWithFs9Support())    { sl << "FS9"; }
                if (isCompiledWithFsxSupport())    { sl << "FSX"; }
                if (isCompiledWithXPlaneSupport()) { sl << "XPlane"; }
                if (isCompiledWithP3DSupport())    { sl << "P3D"; }
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
                infoLong = infoLong.append("BlackCore: ").append(BlackMisc::boolToYesNo(isCompiledWithBlackCore()));
                infoLong = infoLong.append(" BlackInput: ").append(BlackMisc::boolToYesNo(isCompiledWithBlackInput()));
                infoLong = infoLong.append(" BlackSound: ").append(BlackMisc::boolToYesNo(isCompiledWithBlackSound()));
                infoLong = infoLong.append(" GUI: ").append(BlackMisc::boolToYesNo(isCompiledWithGui()));

                infoLong = infoLong.append(" FS9: ").append(BlackMisc::boolToYesNo(isCompiledWithFs9Support()));
                infoLong = infoLong.append(" FSX: ").append(BlackMisc::boolToYesNo(isCompiledWithFsxSupport()));
                infoLong = infoLong.append(" P3D: ").append(BlackMisc::boolToYesNo(isCompiledWithP3DSupport()));
                infoLong = infoLong.append(" XPlane: ").append(BlackMisc::boolToYesNo(isCompiledWithXPlaneSupport()));
            }
            return infoLong;
        }
    }

    QString CProject::environmentInfo(const QString &separator)
    {
        QString env("Beta: ");
        env.append(boolToYesNo(isBetaTest()));
        env = env.append(" dev.env,: ").append(boolToYesNo(isRunningInDeveloperEnvironment()));
        env = env.append(separator);
        env.append("Windows: ").append(boolToYesNo(isRunningOnWindowsNtPlatform()));
        return env;
    }

    QString CProject::convertToQString(const QString &separator)
    {
        QString str(version());
        str = str.append(" ").append(isReleaseBuild() ? "Release build" : "Debug build");
        str = str.append(separator);
        str = str.append(environmentInfo(separator));
        str = str.append(separator);
        str.append(compiledWithInfo(false));
        return str;
    }

} // ns

#undef BLACK_VERSION_STR
#undef BLACK_VERSION_STR_X
