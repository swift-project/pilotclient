/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/xplane/xplaneutil.h"
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
                QFileInfo fileInfo(filePath);
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
                    if (QFileInfo(pathToCheck).exists()) { lastLine = pathToCheck; }
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
                QString xplaneInstallFilePath;
                QString xplaneInstallFile = QStringLiteral("/x-plane_install.txt");
#if defined(Q_OS_WIN)
                xplaneInstallFilePath = getWindowsLocalAppDataPath() + xplaneInstallFile;
#elif defined (Q_OS_LINUX)
                xplaneInstallFilePath = QDir::homePath() + QStringLiteral("/.x-plane") + xplaneInstallFile;
#elif defined (Q_OS_OSX)
                xplaneInstallFilePath = QDir::homePath() + QStringLiteral("/Library/Preferences") + xplaneInstallFile;
#endif
                return getLastExistingPathFromFile(xplaneInstallFilePath);
            }

            QString CXPlaneUtil::xplane10Dir()
            {
                QString xplaneInstallFilePath;
                QString xplaneInstallFile = QStringLiteral("/x-plane_install_10.txt");
#if defined(Q_OS_WIN)
                xplaneInstallFilePath = getWindowsLocalAppDataPath() + xplaneInstallFile;
#elif defined (Q_OS_UNIX)
                xplaneInstallFilePath = QDir::homePath() + QStringLiteral("/.x-plane") + xplaneInstallFile;
#elif defined (Q_OS_OSX)
                xplaneInstallFilePath = QDir::homePath() + QStringLiteral("/Library/Preferences") + xplaneInstallFile;
#endif
                return getLastExistingPathFromFile(xplaneInstallFilePath);
            }

            QString CXPlaneUtil::xplaneRootDir()
            {
                if (!xplane10Dir().isEmpty()) { return xplane10Dir(); }
                else if (!xplane9Dir().isEmpty()) { return xplane9Dir(); }
                else { return {}; }
            }

            QString CXPlaneUtil::xplaneModelDirectory()
            {
                return xplaneRootDir();
            }

            QStringList CXPlaneUtil::xplaneModelExcludeDirectoryPatterns()
            {
                return QStringList();
            }

            QString CXPlaneUtil::xbusLegacyDir()
            {
                QString legacyPath("/Resources/plugins/xbus/LegacyData");
                // Return the first non empty path, we can find.
                if (!xplane10Dir().isEmpty())
                {
                    QString xbusLegacy = xplane10Dir() + legacyPath;
                    if (QDir(xbusLegacy).exists())
                    {
                        return xbusLegacy;
                    }
                }

                if (!xplane9Dir().isEmpty())
                {
                    QString xbusLegacy = xplane9Dir() + legacyPath;
                    if (QDir(xbusLegacy).exists())
                    {
                        return xbusLegacy;
                    }
                }
                return {};
            }

        } // namespace
    } // namespace
} // namespace
