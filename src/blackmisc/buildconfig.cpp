/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "buildconfig.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/simulation/simulatorinfo.h"

#include <QStringList>
#include <QCoreApplication>
#include <QProcessEnvironment>
#include <QStandardPaths>

namespace BlackMisc
{
    bool CBuildConfig::isCompiledWithBlackCore()
    {
#ifdef WITH_BLACKCORE
        return true;
#else
        return false;
#endif
    }

    bool CBuildConfig::isCompiledWithBlackSound()
    {
#ifdef WITH_BLACKSOUND
        return true;
#else
        return false;
#endif
    }

    bool CBuildConfig::isCompiledWithBlackInput()
    {
#ifdef WITH_BLACKINPUT
        return true;
#else
        return false;
#endif
    }

    bool CBuildConfig::isCompiledWithFs9Support()
    {
#ifdef WITH_FS9
        return true;
#else
        return false;
#endif
    }

    bool CBuildConfig::isCompiledWithFsxSupport()
    {
#ifdef WITH_FSX
        return true;
#else
        return false;
#endif
    }

    bool CBuildConfig::isCompiledWithP3DSupport()
    {
        return isCompiledWithFsxSupport();
    }

    bool CBuildConfig::isCompiledWithMsFlightSimulatorSupport()
    {
        return isCompiledWithFs9Support() || isCompiledWithFsxSupport() || isCompiledWithP3DSupport();
    }

    bool CBuildConfig::isCompiledWithXPlaneSupport()
    {
#ifdef WITH_XPLANE
        return true;
#else
        return false;
#endif
    }

    bool CBuildConfig::isCompiledWithFlightSimulatorSupport()
    {
        return isCompiledWithFsxSupport() || isCompiledWithXPlaneSupport();
    }

    bool CBuildConfig::isCompiledWithGui()
    {
#ifdef WITH_BLACKGUI
        return true;
#else
        return false;
#endif
    }

    bool CBuildConfig::isDebugBuild()
    {
#ifdef QT_DEBUG
        return true;
#else
        return false;
#endif
    }

    bool CBuildConfig::isReleaseBuild()
    {
#ifdef QT_NO_DEBUG
        return true;
#else
        return false;
#endif
    }

    bool CBuildConfig::isBetaTest()
    {
#ifdef SWIFT_BETA
        return true;
#else
        return false;
#endif
    }

    bool CBuildConfig::canRunInDeveloperEnvironment()
    {
        if (isBetaTest()) { return true; }
        return !isShippedVersion();
    }

    bool CBuildConfig::isShippedVersion()
    {
#ifdef SWIFT_SHIPPED
        return true;
#else
        return false;
#endif
    }

    bool CBuildConfig::isRunningOnWindowsNtPlatform()
    {
#ifdef Q_OS_WIN
        // QSysInfo::WindowsVersion only available on Win platforms
        return (QSysInfo::WindowsVersion & QSysInfo::WV_NT_based);
#else
        return false;
#endif
    }

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

    const QStringList &CBuildConfig::swiftTeamDefaultServers()
    {
        static const QStringList s({ "https://vatsim-germany.org:50443/mapping/public/shared",
                                     "http://ubuntu12/public/bootstrap/shared"});
        return s;
    }

    bool CBuildConfig::isLifetimeExpired()
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

    const QString &CBuildConfig::getApplicationDir()
    {
        static const QString s(getApplicationDirImpl());
        return s;
    }

    QString getSwiftResourceDirImpl()
    {
        QDir dir(CBuildConfig::getApplicationDir());
        if (dir.cdUp())
        {
            Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
            return dir.absolutePath();
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "missing dir");
        return "";
    }

    const QString &CBuildConfig::getSwiftResourceDir()
    {
        static const QString s(getSwiftResourceDirImpl());
        return s;
    }

    const QString getBootstrapResourceFileImpl()
    {
        const QString d(CBuildConfig::getSwiftResourceDir());
        if (d.isEmpty()) { return ""; }
        const QFile file(CFileUtils::appendFilePaths(d, "shared/boostrap/boostrap.json"));
        Q_ASSERT_X(file.exists(), Q_FUNC_INFO, "missing dir");
        return QFileInfo(file).absoluteFilePath();
    }

    const QString &CBuildConfig::getBootstrapResourceFile()
    {
        static const QString s(getBootstrapResourceFileImpl());
        return s;
    }

    QString getSwiftStaticDbFilesDirImpl()
    {
        const QString d(CBuildConfig::getSwiftResourceDir());
        if (d.isEmpty()) { return ""; }
        QDir dir(CFileUtils::appendFilePaths(d, "shared/dbdata"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CBuildConfig::getSwiftStaticDbFilesDir()
    {
        static QString s(getSwiftStaticDbFilesDirImpl());
        return s;
    }

    QString getImagesDirImpl()
    {
        const QString d(CBuildConfig::getSwiftResourceDir());
        QDir dir(CFileUtils::appendFilePaths(d, "data/images"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CBuildConfig::getImagesDir()
    {
        static const QString s(getImagesDirImpl());
        return s;
    }

    QString getDocumentationDirectoryImpl()
    {
        QStringList pathes(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation));
        QString d = pathes.first();
        d = CFileUtils::appendFilePaths(d, "/swift");
        QDir dir(d);
        if (dir.exists()) { return dir.absolutePath(); }
        return pathes.first();
    }

    const QString &CBuildConfig::getDocumentationDirectory()
    {
        static const QString d(getDocumentationDirectoryImpl());
        return d;
    }

    const QString &CBuildConfig::compiledWithInfo(bool shortVersion)
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

    bool CVersion::isNewerVersion(const QString &versionString)
    {
        if (versionString.isEmpty()) { return false; }
        if (CVersion::version() == versionString) { return false; }

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



    QList<int> CVersion::getVersionParts(const QString &versionString)
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

} // ns

//! \endcond
