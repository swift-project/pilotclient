/* Copyright (C) 2013
 * swift Project Community/Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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
#include <QtGlobal>

namespace BlackConfig
{
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

    QString getSwiftShareDirImpl()
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

    const QString &CBuildConfig::getSwiftShareDir()
    {
        static const QString s(getSwiftShareDirImpl());
        return s;
    }

    const QString getBootstrapResourceFileImpl()
    {
        const QString d(CBuildConfig::getSwiftShareDir());
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
        const QString d(CBuildConfig::getSwiftShareDir());
        if (d.isEmpty()) { return ""; }
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "shared/dbdata"));
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
        const QString d(CBuildConfig::getSwiftShareDir());
        if (d.isEmpty()) { return ""; }
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "sounds"));
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
        const QString d(CBuildConfig::getSwiftShareDir());
        if (d.isEmpty()) { return ""; }
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "qss"));
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
        const QString d(CBuildConfig::getSwiftShareDir());
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "images"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CBuildConfig::getImagesDir()
    {
        static const QString s(getImagesDirImpl());
        return s;
    }

    QString getHtmlDirImpl()
    {
        const QString d(CBuildConfig::getSwiftShareDir());
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "html"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CBuildConfig::getHtmlDir()
    {
        static const QString s(getHtmlDirImpl());
        return s;
    }

    QString getLegalDirImpl()
    {
        const QString d(CBuildConfig::getSwiftShareDir());
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "legal"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CBuildConfig::getLegalDir()
    {
        static const QString s(getLegalDirImpl());
        return s;
    }

    const QString &CBuildConfig::getAboutFileLocation()
    {
        static const QString about = QDir::cleanPath(CBuildConfig::getLegalDir() + QDir::separator() + "about.html");
        return about;
    }

    QString getTestFilesDirImpl()
    {
        const QString d(CBuildConfig::getSwiftShareDir());
        if (d.isEmpty()) { return ""; }
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "test"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CBuildConfig::getTestFilesDir()
    {
        static QString s(getTestFilesDirImpl());
        return s;
    }

    const QString &CBuildConfig::getHtmlTemplateFileName()
    {
        static const QString s(getHtmlDir() + QDir::separator() + "swifttemplate.html");
        return s;
    }

    bool CBuildConfig::canRunInDeveloperEnvironment()
    {
        if (CBuildConfig::isDevBranch()) { return true; }
        return !CBuildConfig::isStableBranch();
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

    const QString &CBuildConfig::buildDateAndTime()
    {
        // http://en.cppreference.com/w/cpp/preprocessor/replace#Predefined_macros
        static const QString buildDateAndTime(__DATE__ " "  __TIME__);
        return buildDateAndTime;
    }

    namespace Private
    {
        const QDateTime buildTimestampImpl()
        {
            // Mar 27 2017 20:17:06 (needs to be on english locale, otherwise fails - e.g.
            QDateTime dt = QLocale(QLocale::English).toDateTime(CBuildConfig::buildDateAndTime(), "MMM dd yyyy hh:mm:ss");
            dt.setUtcOffset(0);
            return dt;
        }
    }

    const QDateTime &CBuildConfig::buildTimestamp()
    {
        // Mar 27 2017 20:17:06
        static const QDateTime dt = Private::buildTimestampImpl();
        return dt;
    }

    const QString &CVersion::version()
    {
        static const QString v(versionMajorMinorPatch() + "." + QString::number(buildTimestampAsVersionSegment(CBuildConfig::buildTimestamp())));
        return v;
    }

    const QList<int> &CVersion::getVersionParts()
    {
        static const QList<int> parts(splitIntoVersionParts(version()));
        return parts;
    }

    bool CVersion::isNewerVersion(const QString &versionString)
    {
        return isNewerVersion(version(), versionString);
    }

    bool CVersion::isNewerVersion(const QString &aVersion, const QString &bVersion)
    {
        if (aVersion.isEmpty() || bVersion.isEmpty()) { return false; }
        if (aVersion == bVersion) { return true; }

        const QList<int> aParts(splitIntoVersionParts(aVersion));
        const QList<int> bParts(splitIntoVersionParts(bVersion));
        for (int i = 0; i < bParts.length(); i++)
        {
            if (aParts.length() <= i) { return true; }
            if (bParts.at(i) > aParts.at(i)) { return true; }
            if (bParts.at(i) < aParts.at(i)) { return false; }
            // same, try next part
        }
        return false;
    }

    int CVersion::buildTimestampAsVersionSegment(const QDateTime &buildTimestamp)
    {
        if (buildTimestamp.isValid())
        {
            static const qint64 dt2017 = QDateTime::fromString("20170101000000", "yyyyMMddHHmmss").toMSecsSinceEpoch();
            const qint64 msSinceEpoch = buildTimestamp.toMSecsSinceEpoch();
            const qint64 msSinceSwiftEpoch = msSinceEpoch - dt2017;
            return msSinceSwiftEpoch / 1000; // accuraccy second should be enough, and is shorter
        }
        return 0; // intentionally 0 and not zero => 0.7.3.0 <-
    }

    QList<int> CVersion::splitIntoVersionParts(const QString &versionString)
    {
        const QStringList parts = versionString.split('.');
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
