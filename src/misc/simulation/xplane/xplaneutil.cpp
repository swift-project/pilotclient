// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/xplane/xplaneutil.h"

#include <qsystemdetection.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QTextStream>

#include "misc/directoryutils.h"
#include "misc/fileutils.h"
#include "misc/swiftdirectories.h"

#ifdef Q_OS_WIN
#    include <ShlObj.h>
#endif

using namespace swift::misc;

namespace swift::misc::simulation::xplane
{
    const QStringList &CXPlaneUtil::getLogCategories()
    {
        static const QStringList cats { CLogCategories::matching() };
        return cats;
    }

    // Returns the last path from filePath, which does exist on the file system
    QString getLastExistingPathFromFile(const QString &filePath)
    {
        const QFileInfo fileInfo(filePath);
        if (!fileInfo.exists()) { return {}; }
        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly)) { return {}; }

        QString lastLine;
        QTextStream ts(&file);
        while (!ts.atEnd())
        {
            QString pathToCheck = ts.readLine();
            if (QFileInfo::exists(pathToCheck)) { lastLine = pathToCheck; }
        }
        return lastLine;
    }

#ifdef Q_OS_WIN
    QString getWindowsLocalAppDataPath()
    {
        QString result;
        TCHAR szLocalAppDataPath[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szLocalAppDataPath)))
        {
            result = QString::fromWCharArray(szLocalAppDataPath);
        }
        return result;
    }
#endif

    QString CXPlaneUtil::xplane9Dir()
    {
        static const QString xplaneInstallFile("x-plane_install.txt");
        const QString xplaneInstallFilePath = xplaneDir(xplaneInstallFile);
        return getLastExistingPathFromFile(xplaneInstallFilePath);
    }

    QString CXPlaneUtil::xplane10Dir()
    {
        static const QString xplaneInstallFile("x-plane_install_10.txt");
        const QString xplaneInstallFilePath = xplaneDir(xplaneInstallFile);
        return getLastExistingPathFromFile(xplaneInstallFilePath);
    }

    QString CXPlaneUtil::xplane11Dir()
    {
        static const QString xplaneInstallFile("x-plane_install_11.txt");
        const QString xplaneInstallFilePath = xplaneDir(xplaneInstallFile);
        return getLastExistingPathFromFile(xplaneInstallFilePath);
    }

    QString CXPlaneUtil::xplaneDir(const QString &xplaneInstallFile)
    {
        //! \fixme KB 8/17 we could also use the runtime CBuildConfig decision here, which looks nicer (I personally
        //! always try to avoid ifdef)
#ifdef Q_OS_WIN
        return CFileUtils::appendFilePathsAndFixUnc(getWindowsLocalAppDataPath(), xplaneInstallFile);
#elif defined(Q_OS_LINUX)
        static const QString xp(".x-plane");
        return CFileUtils::appendFilePaths(QDir::homePath(), xp, xplaneInstallFile);
#elif defined(Q_OS_OSX)
        static const QString lib("Library/Preferences");
        return CFileUtils::appendFilePaths(QDir::homePath(), lib, xplaneInstallFile);
#endif
    }

    const QString &CXPlaneUtil::xplaneRootDir()
    {
        static const QString dir = [] {
            if (!xplane11Dir().isEmpty()) { return xplane11Dir(); }
            else if (!xplane10Dir().isEmpty()) { return xplane10Dir(); }
            else if (!xplane9Dir().isEmpty()) { return xplane9Dir(); }
            else { return QString(); }
        }();
        return dir;
    }

    bool CXPlaneUtil::isXplaneRootDirExisting()
    {
        static const bool exists = QDir(xplaneRootDir()).exists();
        return exists;
    }

    const QString &CXPlaneUtil::xplanePluginDir()
    {
        static const QString pd(
            xplaneRootDir().isEmpty() ? "" : CFileUtils::appendFilePaths(xplaneRootDir(), xplanePluginPathName()));
        return pd;
    }

    QString CXPlaneUtil::pluginDirFromRootDir(const QString &rootDir)
    {
        return CFileUtils::appendFilePathsAndFixUnc(rootDir, xplanePluginPathName());
    }

    QStringList CXPlaneUtil::modelDirectoriesFromSimDir(const QString &simulatorDir)
    {
        if (simulatorDir.isEmpty()) { return {}; }
        return QStringList({ simulatorDir });
    }

    const QString &CXPlaneUtil::xplanePluginPathName()
    {
        static const QString p("Resources/plugins");
        return p;
    }

    const QString &CXPlaneUtil::xswiftbusPathName()
    {
        static const QString p("xswiftbus");
        return p;
    }

    bool CXPlaneUtil::isXplanePluginDirDirExisting()
    {
        static const bool exists = QDir(xplanePluginDir()).exists();
        return exists;
    }

    QStringList CXPlaneUtil::pluginSubdirectories(const QString &pluginDir)
    {
        const QString dirName = pluginDir.isEmpty() ? xplaneRootDir() : pluginDir;
        if (!CDirectoryUtils::isDirExisting(dirName)) { return {}; }
        const QDir dir(dirName);
        return dir.entryList(QDir::Dirs, QDir::Name | QDir::IgnoreCase);
    }

    const QStringList &CXPlaneUtil::xplaneModelDirectories()
    {
        static const QStringList dirs = xplaneRootDir().isEmpty() ? QStringList() : QStringList({ xplaneRootDir() });
        return dirs;
    }

    const QStringList &CXPlaneUtil::xplaneModelExcludeDirectoryPatterns()
    {
        static const QStringList empty;
        return empty;
    }

    QString CXPlaneUtil::xswiftbusPluginDir(const QString &xplaneRootDir)
    {
        const QString rootDir = xplaneRootDir.isEmpty() ? CXPlaneUtil::xplaneRootDir() : xplaneRootDir;
        if (!rootDir.isEmpty())
        {
            const QString xswiftbusDir = CFileUtils::appendFilePathsAndFixUnc(
                CXPlaneUtil::pluginDirFromRootDir(xplaneRootDir), CXPlaneUtil::xswiftbusPathName());
            if (CDirectoryUtils::isDirExisting(xswiftbusDir)) { return xswiftbusDir; }
        }
        return {};
    }

    QString CXPlaneUtil::xswiftbusLegacyDir(const QString &xplaneRootDir)
    {
        static const QString legacyPath("/Resources/plugins/xswiftbus/LegacyData");
        // Return the first non empty path, we can find.

        const QString rootDir = xplaneRootDir.isEmpty() ? CXPlaneUtil::xplaneRootDir() : xplaneRootDir;
        if (!rootDir.isEmpty())
        {
            const QString xswiftbusLegacy = CFileUtils::appendFilePathsAndFixUnc(xplaneRootDir, legacyPath);
            if (CDirectoryUtils::isDirExisting(xswiftbusLegacy)) { return xswiftbusLegacy; }
        }
        return {};
    }

    bool CXPlaneUtil::hasXSwiftBusBuildAndPluginDir(const QString &xplaneRootDir)
    {
        if (CSwiftDirectories::getXSwiftBusBuildDirectory().isEmpty()) { return false; }
        const QString xswiftBusPluginDir = CXPlaneUtil::xswiftbusPluginDir(xplaneRootDir);
        return (!xswiftBusPluginDir.isEmpty());
    }

    QStringList CXPlaneUtil::findConflictingPlugins(const QString &pluginDir)
    {
        const QStringList files = findAllXplFiles(pluginDir);
        QStringList conflicts;
        for (const QString &file : files)
        {
            if (file.contains("swift", Qt::CaseInsensitive)) { continue; }
            if (file.contains("ivap", Qt::CaseInsensitive))
            {
                conflicts.push_back(file);
                continue;
            }
            if (file.contains("XSquawkBox", Qt::CaseInsensitive))
            {
                conflicts.push_back(file);
                continue;
            }
        }
        return conflicts;
    }

    QStringList CXPlaneUtil::findAllXplFiles(const QString &pluginDir)
    {
        const QString dirName = CFileUtils::fixWindowsUncPath(pluginDir.isEmpty() ? xplaneRootDir() : pluginDir);
        const QDir directory(dirName);
        if (!directory.exists()) { return {}; }

        // this finds the current levels XPLs
        QStringList files = directory.entryList(xplFileFilter(), QDir::Files, QDir::Name | QDir::IgnoreCase);
        const QStringList dirs = directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase);
        for (const QString &dir : dirs)
        {
            const QString subDir = CFileUtils::appendFilePaths(dirName, dir);
            const QStringList subDirFiles = CXPlaneUtil::findAllXplFiles(subDir);
            if (subDirFiles.isEmpty()) { continue; }
            for (const QString &file : subDirFiles) { files.push_back(CFileUtils::appendFilePaths(dir, file)); }
        }
        return files;
    }

    bool CXPlaneUtil::hasNewerXSwiftBusBuild(const QString &xplaneRootDir)
    {
        if (CSwiftDirectories::getXSwiftBusBuildDirectory().isEmpty()) { return false; }
        const QString xswiftBusPluginDir = CXPlaneUtil::xswiftbusPluginDir(xplaneRootDir);
        if (xswiftBusPluginDir.isEmpty()) { return false; }

        const QFileInfo fiLatestBuild =
            CFileUtils::findLastModified(CSwiftDirectories::getXSwiftBusBuildDirectory(), true, xplFileFilter());
        if (!fiLatestBuild.lastModified().isValid()) { return false; }

        const QFileInfo fiLatestDeployed = CFileUtils::findLastModified(xswiftBusPluginDir, true, xplFileFilter());
        if (!fiLatestDeployed.lastModified().isValid()) { return true; } // not yet existing

        // newer?
        return fiLatestBuild.lastModified() > fiLatestDeployed.lastModified();
    }

    int CXPlaneUtil::copyXSwiftBusBuildFiles(const QString &xplaneRootDir)
    {
        if (CSwiftDirectories::getXSwiftBusBuildDirectory().isEmpty()) { return -1; }
        const QString xswiftBusPluginDir = CXPlaneUtil::xswiftbusPluginDir(xplaneRootDir);
        if (xswiftBusPluginDir.isEmpty()) { return -1; }

        return CDirectoryUtils::copyDirectoryRecursively(CSwiftDirectories::getXSwiftBusBuildDirectory(),
                                                         xswiftBusPluginDir, true);
    }

    const QStringList &CXPlaneUtil::xplFileFilter()
    {
        static const QStringList filter({ "*.xpl" });
        return filter;
    }

    CStatusMessageList CXPlaneUtil::validateModelDirectories(const QString &simDir, const QStringList &modelDirectories)
    {
        if (simDir.isEmpty())
        {
            return CStatusMessage(static_cast<CXPlaneUtil *>(nullptr), CStatusMessage::SeverityWarning,
                                  u"no simulator directory", true);
        }

        CStatusMessageList msgs;
        if (modelDirectories.isEmpty()) { return msgs; }
        const QDir sd(simDir);
        const bool simDirExists = sd.exists();
        for (const QString &modelDir : modelDirectories)
        {
            const bool exists = simDirExists ?
                                    CDirectoryUtils::isSameOrSubDirectoryOf(modelDir, sd) :
                                    CDirectoryUtils::isSameOrSubDirectoryOfStringBased(modelDir, sd.absolutePath());
            const CStatusMessage m =
                exists ?
                    CStatusMessage(static_cast<CXPlaneUtil *>(nullptr)).info(u"Model directory '%1' inside '%2'")
                        << modelDir << sd.absolutePath() :
                    CStatusMessage(static_cast<CXPlaneUtil *>(nullptr)).error(u"Model directory '%1' NOT inside '%2'")
                        << modelDir << sd.absolutePath();
            msgs.push_back(m);
        }
        msgs.addValidationCategory();
        return msgs;
    }
} // namespace swift::misc::simulation::xplane
