/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/worker.h"
#include "fileutilities.h"

#include <QFile>
#include <QCoreApplication>

namespace BlackMisc
{
    bool CFileUtils::writeStringToFile(const QString &content, const QString &fileNameAndPath)
    {
        if (fileNameAndPath.isEmpty()) { return false; }
        QFile file(fileNameAndPath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { return false; }
        QTextStream stream(&file);
        stream << content;
        file.close();
        return true;
    }

    QString CFileUtils::readFileToString(const QString &fileNameAndPath)
    {
        QFile file(fileNameAndPath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { return ""; }
        QTextStream stream(&file);
        QString content(stream.readAll());
        file.close();
        return content;
    }

    bool CFileUtils::writeStringToFileInBackground(const QString &content, const QString &fileNameAndPath)
    {
        if (fileNameAndPath.isEmpty()) { return false; }
        CWorker *worker = BlackMisc::CWorker::fromTask(QCoreApplication::instance(), "writeStringToFileInBackground", [content, fileNameAndPath]()
        {
            bool s = CFileUtils::writeStringToFile(content, fileNameAndPath);
            Q_UNUSED(s);
        });
        return worker ? true : false;
    }

    QString CFileUtils::appendFilePaths(const QString &path1, const QString &path2)
    {
        if (path1.isEmpty()) { return QDir::cleanPath(path2); }
        if (path2.isEmpty()) { return QDir::cleanPath(path1); }
        return QDir::cleanPath(path1 + QDir::separator() + path2);
    }

} // ns
