/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "project.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/simulation/simulatorinfo.h"

#include <QStringList>
#include <QCoreApplication>
#include <QProcessEnvironment>

#if !defined(BLACK_VERSION)
#error Missing version
#endif

#if !defined(BLACK_EOL)
#error Missing EOL
#endif

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
        static const QString v(BLACK_STRINGIFY(BLACK_VERSION));
#else
        static const QString v("?");
#endif
        return v;
    }

    int CProject::versionMajor()
    {
        return getMajorMinor(0);
    }

    int CProject::versionMinor()
    {
        return getMajorMinor(1);
    }

    bool CProject::isNewerVersion(const QString &versionString)
    {
        if (versionString.isEmpty()) { return false; }
        if (CProject::version() == versionString) { return false; }

        QList<int> newer(getVersionParts(versionString));
        QList<int> current(getVersionParts(version()));
        for (int i = 0; i < current.length(); i++)
        {
            if (newer.length() <= i) { return false; }
            if (current.at(i) > newer.at(i)) { return false; }
            if (current.at(i) < newer.at(i)) { return true; }
        }
        return false;
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
#ifdef SWIFT_BETA
        return true;
#else
        return false;
#endif
    }

    bool CProject::canRunInDeveloperEnvironment()
    {
        if (isBetaTest()) { return true; }
        return !isShippedVersion();
    }

    bool CProject::isShippedVersion()
    {
#ifdef SWIFT_SHIPPED
        return true;
#else
        return false;
#endif
    }

    bool CProject::isRunningOnWindowsNtPlatform()
    {
#ifdef Q_OS_WIN
        // QSysInfo::WindowsVersion only available on Win platforms
        return (QSysInfo::WindowsVersion & QSysInfo::WV_NT_based);
#else
        return false;
#endif
    }

    QList<int> CProject::getVersionParts(const QString &versionString)
    {
        QStringList parts = versionString.split('.');
        QList<int> partsInt;
        for (const QString &p : parts)
        {
            bool ok = false;
            int pInt = p.toInt(&ok);
            partsInt.append(ok ? pInt : -1);
        }
        return partsInt;
    }

    int CProject::getMajorMinor(int index)
    {
        QList<int> partsInt(getVersionParts(version()));
        if (index >= partsInt.length()) { return -1; }
        return partsInt[index];
    }

    const QString &CProject::swiftGuiExecutableName()
    {
        static const QString s("swiftguistd");
        return s;
    }

    const QString &CProject::swiftCoreExecutableName()
    {
        static const QString s("swiftcore");
        return s;
    }

    const QString &CProject::swiftDataExecutableName()
    {
        static const QString s("swiftdata");
        return s;
    }

    const QStringList &CProject::swiftTeamDefaultServers()
    {
        static const QStringList s({ "https://vatsim-germany.org:50443/mapping/public/shared", "http://ubuntu12/public/bootstrap/shared"});
        return s;
    }

    const QDateTime &CProject::getEol()
    {
        static const QString eol(BLACK_STRINGIFY(BLACK_EOL));
        static const QDateTime dt(eol.isEmpty() ? QDateTime() : QDateTime::fromString(eol, "yyyyMMdd"));
        return dt;
    }

    bool CProject::isLifetimeExpired()
    {
        if (getEol().isValid())
        {
            return QDateTime::currentDateTime() > getEol();
        }
        else
        {
            return true;
        }
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
        if (dir.cdUp())
        {
            Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
            return dir.absolutePath();
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "missing dir");
        return "";
    }

    const QString &CProject::getSwiftResourceDir()
    {
        static const QString s(getSwiftResourceDirImpl());
        return s;
    }

    QString getSwiftStaticDbFilesDirImpl()
    {
        QString d(CProject::getSwiftResourceDir());
        if (d.isEmpty()) { return ""; }
        QDir dir(CFileUtils::appendFilePaths(d, "shared/dbdata"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CProject::getSwiftStaticDbFilesDir()
    {
        static QString s(getSwiftStaticDbFilesDirImpl());
        return s;
    }

    QString getImagesDirImpl()
    {
        QString d(CProject::getSwiftResourceDir());
        QDir dir(CFileUtils::appendFilePaths(d, "data/images"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CProject::getImagesDir()
    {
        static const QString s(getImagesDirImpl());
        return s;
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
} // ns

//! \endcond
