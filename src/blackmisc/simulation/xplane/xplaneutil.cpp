/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/xplane/xplaneutil.h"
#include "blackmisc/fileutils.h"
#include "qsystemdetection.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QTextStream>

#if defined(Q_OS_WIN)
#include <Shlobj.h>
#endif

using namespace BlackMisc;

namespace BlackMisc
{
    namespace Simulation
    {
        namespace XPlane
        {
            // Returns the last path from filePath, which does exist on the file system
            QString getLastExistingPathFromFile(const QString &filePath)
            {
                const QFileInfo fileInfo(filePath);
                if (!fileInfo.exists()) { return {}; }
                QFile file(fileInfo.absoluteFilePath());
                if (!file.open(QIODevice::ReadOnly))
                {
                    return {};
                }

                QString lastLine;
                QTextStream ts(&file);
                while (!ts.atEnd())
                {
                    QString pathToCheck = ts.readLine();
                    if (QFileInfo::exists(pathToCheck)) { lastLine = pathToCheck; }
                }
                return lastLine;
            }

#if defined(Q_OS_WIN)
            QString getWindowsLocalAppDataPath()
            {
                QString result;
                TCHAR szLocalAppDataPath[MAX_PATH];
                if (SUCCEEDED(SHGetFolderPath(NULL,
                                              CSIDL_LOCAL_APPDATA,
                                              NULL,
                                              0,
                                              szLocalAppDataPath)))
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
                //! \fixme KB 8/17 we could also use the runtime CBuildConfig decision here, which looks nicer (I personally always try to avoid ifdef)
#if defined(Q_OS_WIN)
                return CFileUtils::appendFilePaths(getWindowsLocalAppDataPath(), xplaneInstallFile);
#elif defined (Q_OS_LINUX)
                static const QString xp(".x-plane");
                return CFileUtils::appendFilePaths(QDir::homePath(), xp, xplaneInstallFile);
#elif defined (Q_OS_OSX)
                static const QString lib("Library/Preferences");
                return CFileUtils::appendFilePaths(QDir::homePath(), lib, xplaneInstallFile);
#endif
            }

            const QString &CXPlaneUtil::xplaneRootDir()
            {
                static const QString dir = []
                {
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
                static const QString pd(xplaneRootDir().isEmpty() ? "" : CFileUtils::appendFilePaths(xplaneRootDir(), xplanePluginPath()));
                return pd;
            }

            QString CXPlaneUtil::pluginDirFromSimDir(const QString &simulatorDir)
            {
                return CFileUtils::appendFilePaths(simulatorDir, xplanePluginPath());
            }

            QStringList CXPlaneUtil::modelDirectoriesFromSimDir(const QString &simulatorDir)
            {
                return QStringList({ simulatorDir });
            }

            QString CXPlaneUtil::xplanePluginPath()
            {
                static const QString p("/Resources/plugins");
                return p;
            }

            bool CXPlaneUtil::isXplanePluginDirDirExisting()
            {
                static const bool exists = QDir(xplanePluginDir()).exists();
                return exists;
            }

            const QStringList &CXPlaneUtil::xplaneModelDirectories()
            {
                static const QStringList dirs = xplaneRootDir().isEmpty() ? QStringList() : QStringList({xplaneRootDir()});
                return dirs;
            }

            const QStringList &CXPlaneUtil::xplaneModelExcludeDirectoryPatterns()
            {
                static const QStringList empty;
                return empty;
            }

            QString CXPlaneUtil::xswiftbusLegacyDir(const QString &rootDir)
            {
                static const QString legacyPath("/Resources/plugins/xswiftbus/LegacyData");
                // Return the first non empty path, we can find.
                if (!rootDir.isEmpty())
                {
                    const QString xswiftbusLegacy = CFileUtils::appendFilePaths(rootDir, legacyPath);
                    if (QDir(xswiftbusLegacy).exists())
                    {
                        return xswiftbusLegacy;
                    }
                }

                for (auto func : {&CXPlaneUtil::xplane11Dir, &CXPlaneUtil::xplane10Dir, &CXPlaneUtil::xplane9Dir})
                {
                    const QString xswiftbusLegacy = CFileUtils::appendFilePaths(func(), legacyPath);
                    if (QDir(xswiftbusLegacy).exists())
                    {
                        return xswiftbusLegacy;
                    }
                }
                return {};
            }
        } // namespace
    } // namespace
} // namespace
