/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "project.h"
#include <QStringList>
#include <QCoreApplication>
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

    const QString &CProject::compiledInfo()
    {
        static QString info;
        if (info.isEmpty())
        {
            static QStringList sl;
            if (isCompiledWithBlackCore()) { sl << "BlackCore"; }
            if (isCompiledWithBlackSound()) { sl << "BlackSound"; }
            if (isCompiledWithBlackInput()) { sl << "BlackInput"; }
            if (isCompiledWithGui()) { sl << "BlackGui"; }
            if (isCompiledWithFs9Support()) { sl << "FS9"; }
            if (isCompiledWithFsxSupport()) { sl << "FSX"; }
            if (isCompiledWithXPlaneSupport()) { sl << "XPlane"; }
            if (isCompiledWithP3DSupport()) { sl << "P3D"; }
            info = sl.join(", ");
            if (info.isEmpty()) info = "<none>";
        }
        return info;
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
        if (!isDebugBuild()) { return false; }
        QFileInfo executable(QCoreApplication::applicationFilePath());
        QDir p(executable.dir());

        // search for typical developer dirs, feel free to improve the "algortithm"
        if (!p.cdUp()) { return false; }
        bool hasSrc = p.cd("src");
        if (!hasSrc) { return false; }
        p.cdUp();
        return p.cd("samples");
    }

    int CProject::getMajorMinor(int index)
    {
        QString v = version();
        if (v.isEmpty() || !v.contains(".")) return -1;
        bool ok;
        int vi = v.split(".")[index].toInt(&ok);
        return ok ? vi : -1;
    }

    QString CProject::getApplicationDir()
    {
        QFileInfo executable(QCoreApplication::applicationFilePath());
        QDir p(executable.dir());
        return p.absolutePath();
    }

    QString CProject::getSwiftResourceDir()
    {
        QDir dir(getApplicationDir());
        if (!dir.cdUp()) { return ""; }
        if (dir.cd("resources")) { return dir.absolutePath(); }
        return "";
    }

    QString CProject::getSwiftStaticDbFilesDir()
    {
        QString d(getSwiftResourceDir());
        if (d.isEmpty()) { return ""; }
        QDir dir(d);
        if (dir.cd("swiftDB")) { return dir.absolutePath(); }
        return "";
    }
} // ns

#undef BLACK_VERSION_STR
#undef BLACK_VERSION_STR_X
