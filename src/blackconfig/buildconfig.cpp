/* Copyright (C) 2013
 * swift Project Community/Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "blackconfig/buildconfig.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QStringList>
#include <QtGlobal>

namespace BlackConfig
{
    bool CBuildConfig::isCompiledWithP3DSupport()
    {
        return CBuildConfig::isCompiledWithFsxSupport();
    }

    bool CBuildConfig::isCompiledWithMsFlightSimulatorSupport()
    {
        return CBuildConfig::isCompiledWithFs9Support() || CBuildConfig::isCompiledWithFsxSupport() || CBuildConfig::isCompiledWithP3DSupport();
    }

    bool CBuildConfig::isCompiledWithFlightSimulatorSupport()
    {
        return CBuildConfig::isCompiledWithFsxSupport() || CBuildConfig::isCompiledWithXPlaneSupport();
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

    bool CBuildConfig::isRunningOnWindowsNtPlatform()
    {
#ifdef Q_OS_WIN
        // QSysInfo::WindowsVersion only available on Win platforms
        return (QSysInfo::WindowsVersion & QSysInfo::WV_NT_based);
#else
        return false;
#endif
    }

    bool CBuildConfig::isRunningOnWindows10()
    {
#ifdef Q_OS_WIN
        // QSysInfo::WindowsVersion only available on Win platforms
        if (!isRunningOnWindowsNtPlatform()) { return false; }
        return (QSysInfo::WindowsVersion == QSysInfo::WV_10_0);
#else
        return false;
#endif
    }

    bool CBuildConfig::isRunningOnMacOSXPlatform()
    {
#ifdef Q_OS_OSX
        return true;
#else
        return false;
#endif
    }

    bool CBuildConfig::isRunningOnLinuxPlatform()
    {
#ifdef Q_OS_LINUX
        return true;
#else
        return false;
#endif
    }

    bool CBuildConfig::isRunningOnUnixPlatform()
    {
        return isRunningOnMacOSXPlatform() || isRunningOnLinuxPlatform();
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

    const QStringList &CBuildConfig::swiftTeamDefaultServers()
    {
        static const QStringList s({ "https://vatsim-germany.org:50443/mapping/public/shared",
            "http://ubuntu12/public/bootstrap/shared"
        });
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

    bool isAppBundle()
    {
        QDir bundleDir(CBuildConfig::getApplicationDir());
        bundleDir.cd("../..");
        static const bool isBundled = QFileInfo(bundleDir.absolutePath()).isBundle();
        return isBundled;
    }

    QString getSwiftResourceDirImpl()
    {
        QDir dir(CBuildConfig::getApplicationDir());
        bool success = true;

        static const bool appBundle = isAppBundle();
        if (CBuildConfig::isRunningOnMacOSXPlatform() && appBundle)
        {
            success = dir.cd("../../../../share");
        }
        else { success = dir.cd("../share"); }

        if (success)
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
        const QFile file(QDir::cleanPath(d + QDir::separator() + "shared/boostrap/boostrap.json"));
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
        QDir dir(QDir::cleanPath(d + QDir::separator() + "shared/dbdata"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CBuildConfig::getSwiftStaticDbFilesDir()
    {
        static QString s(getSwiftStaticDbFilesDirImpl());
        return s;
    }

    QString getSoundFilesDirImpl()
    {
        const QString d(CBuildConfig::getSwiftResourceDir());
        if (d.isEmpty()) { return ""; }
        QDir dir(QDir::cleanPath(d + QDir::separator() + "sounds"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CBuildConfig::getSoundFilesDir()
    {
        static QString s(getSoundFilesDirImpl());
        return s;
    }

    QString getStylesheetsDirImpl()
    {
        const QString d(CBuildConfig::getSwiftResourceDir());
        if (d.isEmpty()) { return ""; }
        QDir dir(QDir::cleanPath(d + QDir::separator() + "qss"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CBuildConfig::getStylesheetsDir()
    {
        static QString s(getStylesheetsDirImpl());
        return s;
    }

    QString getImagesDirImpl()
    {
        const QString d(CBuildConfig::getSwiftResourceDir());
        QDir dir(QDir::cleanPath(d + QDir::separator() + "images"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CBuildConfig::getImagesDir()
    {
        static const QString s(getImagesDirImpl());
        return s;
    }

    bool CBuildConfig::canRunInDeveloperEnvironment()
    {
        if (CBuildConfig::isBetaTest()) { return true; }
        return !CBuildConfig::isShippedVersion();
    }

    QString getDocumentationDirectoryImpl()
    {
        QStringList pathes(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation));
        QString d = pathes.first();
        d = QDir::cleanPath(d + QDir::separator() + "swift");
        QDir dir(d);
        if (dir.exists()) { return dir.absolutePath(); }
        return pathes.first();
    }

    const QString &CBuildConfig::getDocumentationDirectory()
    {
        static const QString d(getDocumentationDirectoryImpl());
        return d;
    }

    QString boolToYesNo(bool v)
    {
        return v ? "yes" : "no";
    }

    const QString &CBuildConfig::compiledWithInfo(bool shortVersion)
    {
        if (shortVersion)
        {
            static QString infoShort;
            if (infoShort.isEmpty())
            {
                QStringList sl;
                if (CBuildConfig::isCompiledWithBlackCore())     { sl << "BlackCore"; }
                if (CBuildConfig::isCompiledWithBlackSound())    { sl << "BlackSound"; }
                if (CBuildConfig::isCompiledWithBlackInput())    { sl << "BlackInput"; }
                if (CBuildConfig::isCompiledWithGui())           { sl << "BlackGui"; }
                if (CBuildConfig::isCompiledWithFs9Support())    { sl << "FS9"; }
                if (CBuildConfig::isCompiledWithFsxSupport())    { sl << "FSX"; }
                if (CBuildConfig::isCompiledWithXPlaneSupport()) { sl << "XPlane"; }
                if (CBuildConfig::isCompiledWithP3DSupport())    { sl << "P3D"; }
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
                infoLong = infoLong.append("BlackCore: ").append(boolToYesNo(isCompiledWithBlackCore()));
                infoLong = infoLong.append(" BlackInput: ").append(boolToYesNo(isCompiledWithBlackInput()));
                infoLong = infoLong.append(" BlackSound: ").append(boolToYesNo(isCompiledWithBlackSound()));
                infoLong = infoLong.append(" GUI: ").append(boolToYesNo(isCompiledWithGui()));

                infoLong = infoLong.append(" FS9: ").append(boolToYesNo(isCompiledWithFs9Support()));
                infoLong = infoLong.append(" FSX: ").append(boolToYesNo(isCompiledWithFsxSupport()));
                infoLong = infoLong.append(" P3D: ").append(boolToYesNo(isCompiledWithP3DSupport()));
                infoLong = infoLong.append(" XPlane: ").append(boolToYesNo(isCompiledWithXPlaneSupport()));
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
