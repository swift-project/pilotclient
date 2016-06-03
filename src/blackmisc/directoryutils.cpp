/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "blackmisc/directoryutils.h"

#include <QCoreApplication>
#include <QDir>
#include <QRegularExpression>

namespace BlackMisc
{

    QString applicationDirectoryPathImpl()
    {
        QString appDirectoryString(qApp->applicationDirPath());
        if (appDirectoryString.endsWith("Contents/MacOS")) { appDirectoryString += "/../../.."; }
        QDir appDirectory(appDirectoryString);
        return appDirectory.absolutePath();
    }

    QString CDirectoryUtils::applicationDirectoryPath()
    {
        static const QString appDirectory(applicationDirectoryPathImpl());
        return appDirectory;
    }

    QString normalizedApplicationDirectoryImpl()
    {
        QString appDir = CDirectoryUtils::applicationDirectoryPath().toLower();
        Q_ASSERT(appDir.size() > 0);
        // Remove leading '/' on Unix
        if (appDir.at(0) == '/') { appDir.remove(0, 1); }
        const QRegularExpression re("[:\\\\\\/]");
        appDir = appDir.replace(re, "_");
        return appDir;
    }

    const QString &CDirectoryUtils::normalizedApplicationDirectory()
    {
        static const QString appDir(normalizedApplicationDirectoryImpl());
        return appDir;
    }

} // ns

//! \endcond
