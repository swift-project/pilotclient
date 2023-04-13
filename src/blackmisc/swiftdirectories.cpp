/* Copyright (C) 2020
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/range.h"
#include "blackconfig/buildconfig.h"
#include <QCoreApplication>
#include <QDir>
#include <QUrl>
#include <QSet>
#include <QRegularExpression>
#include <QStandardPaths>

using namespace BlackConfig;

namespace BlackMisc
{
    QString binDirectoryImpl()
    {
        QString appDirectoryString(qApp->applicationDirPath());
        if (appDirectoryString.endsWith("Contents/MacOS")) { appDirectoryString += "/../../.."; }
        QDir appDirectory(appDirectoryString);
        return appDirectory.absolutePath();
    }

    const QString &CSwiftDirectories::binDirectory()
    {
        static const QString binDir(binDirectoryImpl());
        return binDir;
    }

    const QString &CSwiftDirectories::pluginsDirectory()
    {
        static const QString pDir(CFileUtils::appendFilePaths(binDirectory(), "plugins"));
        return pDir;
    }

    const QString &CSwiftDirectories::audioPluginDirectory()
    {
        static const QString pDir(CFileUtils::appendFilePaths(binDirectory(), "audio"));
        return pDir;
    }

    const QString &CSwiftDirectories::getXSwiftBusBuildDirectory()
    {
        if (!CBuildConfig::isLocalDeveloperDebugBuild())
        {
            static const QString e;
            return e;
        }

        // the xswiftbus directory in out, not in dist
        static const QString bd = [] {
            QDir dir(binDirectory());
            if (!dir.cdUp()) { return QString(); }
            if (!dir.cd("xswiftbus")) { return QString(); }
            return dir.absolutePath();
        }();
        return bd;
    }

    QString CSwiftDirectories::executableFilePath(const QString &executable)
    {
        Q_ASSERT_X(!executable.isEmpty(), Q_FUNC_INFO, "Missing executable file path");
        Q_ASSERT_X(CBuildConfig::isKnownExecutableName(executable), Q_FUNC_INFO, "Unknown exectuable");

        QString s = CFileUtils::appendFilePaths(binDirectory(), executable);
        if (CBuildConfig::isRunningOnMacOSPlatform())
        {
            // MacOS bundle may or may not be a bundle
            const QDir dir(s + QLatin1String(".app/Contents/MacOS"));
            if (dir.exists())
            {
                s += QLatin1String(".app/Contents/MacOS/") + executable;
            }
        }
        else if (CBuildConfig::isRunningOnWindowsNtPlatform())
        {
            s += QLatin1String(".exe");
        }
        return s;
    }

    QString normalizedApplicationDirectoryImpl()
    {
        QString appDir = CSwiftDirectories::binDirectory();
        Q_ASSERT(appDir.size() > 0);
        // Remove leading '/' on Unix
        if (appDir.at(0) == '/') { appDir.remove(0, 1); }
        return QUrl::toPercentEncoding(appDir);
    }

    const QString &CSwiftDirectories::normalizedApplicationDirectory()
    {
        static const QString appDir(normalizedApplicationDirectoryImpl());
        return appDir;
    }

    const QString &CSwiftDirectories::applicationDataDirectory()
    {
        static const QString p = CFileUtils::appendFilePaths(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation), "/org.swift-project/");
        return p;
    }

    const QFileInfoList &CSwiftDirectories::applicationDataDirectories()
    {
        static QFileInfoList fileInfoList = currentApplicationDataDirectories();
        return fileInfoList;
    }

    QFileInfoList CSwiftDirectories::currentApplicationDataDirectories()
    {
        const QDir swiftAppData(applicationDataDirectory()); // contains 1..n subdirs
        if (!swiftAppData.isReadable()) { return QFileInfoList(); }
        return swiftAppData.entryInfoList({}, QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time);
    }

    int CSwiftDirectories::applicationDataDirectoriesCount()
    {
        return applicationDataDirectories().size();
    }

    QStringList CSwiftDirectories::applicationDataDirectoryList(bool withoutCurrent, bool decodedDirName)
    {
        QStringList dirs;
        for (const QFileInfo &info : applicationDataDirectories())
        {
            if (withoutCurrent && info.filePath().contains(normalizedApplicationDirectory(), Qt::CaseInsensitive)) continue;
            dirs.append(decodedDirName ?
                            CDirectoryUtils::decodeNormalizedDirectory(info.filePath()) :
                            info.filePath());
        }
        return dirs;
    }

    const QString &CSwiftDirectories::normalizedApplicationDataDirectory()
    {
        static const QString p = CFileUtils::appendFilePaths(applicationDataDirectory(), normalizedApplicationDirectory());
        return p;
    }

    const QString &CSwiftDirectories::logDirectory()
    {
        static const QString p = CFileUtils::appendFilePaths(normalizedApplicationDataDirectory(), "/logs");
        return p;
    }

    QString getSwiftShareDirImpl()
    {
        QDir dir(CSwiftDirectories::binDirectory());
        const bool success = dir.cd("../share");
        if (success)
        {
            Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
            return dir.absolutePath();
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "missing dir");
        return {};
    }

    const QString &CSwiftDirectories::shareDirectory()
    {
        static const QString s(getSwiftShareDirImpl());
        return s;
    }

    const QString &CSwiftDirectories::shareTestDirectory()
    {
        static const QString test(CFileUtils::appendFilePaths(shareDirectory(), "test"));
        return test;
    }

    const QString &CSwiftDirectories::shareTerrainProbeDirectory()
    {
        static const QString tpd(CFileUtils::appendFilePaths(shareDirectory(), "simulator/swiftTerrainProbe"));
        return tpd;
    }

    const QString &CSwiftDirectories::shareMatchingScriptDirectory()
    {
        static const QString ms(CFileUtils::appendFilePaths(shareDirectory(), "matchingscript"));
        return ms;
    }

    const QString &CSwiftDirectories::bootstrapFileName()
    {
        static const QString n("bootstrap.json");
        return n;
    }

    QString getBootstrapResourceFileImpl()
    {
        const QString d(CSwiftDirectories::shareDirectory());
        if (d.isEmpty()) { return {}; }
        const QFile file(QDir::cleanPath(d + QDir::separator() + "shared/bootstrap/" + CSwiftDirectories::bootstrapFileName()));
        Q_ASSERT_X(file.exists(), Q_FUNC_INFO, "missing bootstrap file");
        return QFileInfo(file).absoluteFilePath();
    }

    const QString &CSwiftDirectories::bootstrapResourceFilePath()
    {
        static const QString s(getBootstrapResourceFileImpl());
        return s;
    }

    QString getSwiftStaticDbFilesDirImpl()
    {
        const QString d(CSwiftDirectories::shareDirectory());
        if (d.isEmpty()) { return {}; }
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "shared/dbdata"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CSwiftDirectories::staticDbFilesDirectory()
    {
        static const QString s(getSwiftStaticDbFilesDirImpl());
        return s;
    }

    QString getSoundFilesDirImpl()
    {
        const QString d(CSwiftDirectories::shareDirectory());
        if (d.isEmpty()) { return {}; }
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "sounds"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CSwiftDirectories::soundFilesDirectory()
    {
        static const QString s(getSoundFilesDirImpl());
        return s;
    }

    QString getStylesheetsDirImpl()
    {
        const QString d(CSwiftDirectories::shareDirectory());
        if (d.isEmpty()) { return {}; }
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "qss"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CSwiftDirectories::stylesheetsDirectory()
    {
        static const QString s(getStylesheetsDirImpl());
        return s;
    }

    QString getImagesDirImpl()
    {
        const QString d(CSwiftDirectories::shareDirectory());
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "images"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CSwiftDirectories::imagesDirectory()
    {
        static const QString s(getImagesDirImpl());
        return s;
    }

    const QString &CSwiftDirectories::imagesAirlinesDirectory()
    {
        static const QString s(QDir::cleanPath(imagesDirectory() + QDir::separator() + "airlines"));
        return s;
    }

    const QString &CSwiftDirectories::imagesFlagsDirectory()
    {
        static const QString s(QDir::cleanPath(imagesDirectory() + QDir::separator() + "flags"));
        return s;
    }

    QString getHtmlDirImpl()
    {
        const QString d(CSwiftDirectories::shareDirectory());
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "html"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CSwiftDirectories::htmlDirectory()
    {
        static const QString s(getHtmlDirImpl());
        return s;
    }

    QString getLegalDirImpl()
    {
        const QString d(CSwiftDirectories::shareDirectory());
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "legal"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CSwiftDirectories::legalDirectory()
    {
        static const QString s(getLegalDirImpl());
        return s;
    }

    const QString &CSwiftDirectories::aboutFilePath()
    {
        static const QString about = QDir::cleanPath(legalDirectory() + QDir::separator() + "about.html");
        return about;
    }

    QString testFilesDirImpl()
    {
        const QString d(CSwiftDirectories::shareDirectory());
        if (d.isEmpty()) { return {}; }
        const QDir dir(QDir::cleanPath(d + QDir::separator() + "test"));
        Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "missing dir");
        return dir.absolutePath();
    }

    const QString &CSwiftDirectories::testFilesDirectory()
    {
        static QString s(testFilesDirImpl());
        return s;
    }

    const QString &CSwiftDirectories::htmlTemplateFilePath()
    {
        static const QString s(htmlDirectory() + QDir::separator() + "swifttemplate.html");
        return s;
    }

    QString getDocumentationDirectoryImpl()
    {
        const QStringList pathes(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation));
        QString d = pathes.first();
        d = QDir::cleanPath(CFileUtils::appendFilePaths(d, "swift"));
        QDir dir(d);
        if (dir.exists()) { return dir.absolutePath(); }
        return pathes.first();
    }

    const QString &CSwiftDirectories::documentationDirectory()
    {
        static const QString d(getDocumentationDirectoryImpl());
        return d;
    }

    const QString &CSwiftDirectories::crashpadDirectory()
    {
        static const QString p = CFileUtils::appendFilePaths(normalizedApplicationDataDirectory(), "/crashpad");
        return p;
    }

    const QString &CSwiftDirectories::crashpadDatabaseDirectory()
    {
        static const QString p = CFileUtils::appendFilePaths(crashpadDirectory(), "/database");
        return p;
    }

    const QString &CSwiftDirectories::crashpadMetricsDirectory()
    {
        static const QString p = CFileUtils::appendFilePaths(crashpadDirectory(), "/metrics");
        return p;
    }

    QStringList CSwiftDirectories::verifyRuntimeDirectoriesAndFiles()
    {
        QStringList failed;
        QDir d(binDirectory());
        if (!d.isReadable()) { failed.append(d.absolutePath()); }

        d = QDir(imagesDirectory());
        if (!d.isReadable()) { failed.append(d.absolutePath()); }

        d = QDir(stylesheetsDirectory());
        if (!d.isReadable()) { failed.append(d.absolutePath()); }

        d = QDir(applicationDataDirectory());
        if (!d.isReadable()) { failed.append(d.absolutePath()); }

        // check if the executables are avialable
        QString fn = executableFilePath(CBuildConfig::swiftCoreExecutableName());
        if (!QFile::exists(fn)) { failed.append(fn); }

        fn = executableFilePath(CBuildConfig::swiftDataExecutableName());
        if (!QFile::exists(fn)) { failed.append(fn); }

        fn = executableFilePath(CBuildConfig::swiftGuiExecutableName());
        if (!QFile::exists(fn)) { failed.append(fn); }

        return failed;
    }

    QString CSwiftDirectories::soundFilePathOrDefaultPath(const QString &directory, const QString &fileName)
    {
        if (!directory.isEmpty())
        {
            const QString fp = CFileUtils::appendFilePathsAndFixUnc(directory, fileName);
            const QFileInfo fi(fp);
            if (fi.exists()) { return fi.absoluteFilePath(); }
        }
        const QString fp = CFileUtils::appendFilePathsAndFixUnc(CSwiftDirectories::soundFilesDirectory(), fileName);
        const QFileInfo fi(fp);
        return (fi.exists()) ? fi.absoluteFilePath() : QString();
    }

} // ns

//! \endcond
