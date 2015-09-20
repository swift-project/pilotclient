/* Copyright (C) 2015
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "filelogger.h"
#include <QString>
#include <QDateTime>
#include <QCoreApplication>

namespace BlackMisc
{

    CFileLogger::CFileLogger(QObject *parent) : CFileLogger(QCoreApplication::applicationName(), QString(), parent)
    {
    }

    CFileLogger::CFileLogger(const QString &applicationName, const QString &logPath, QObject *parent) :
        QObject(parent),
        m_logFile(this),
        m_applicationName(applicationName),
        m_logPath(logPath)
    {
        if (!m_logPath.isEmpty()) { QDir::root().mkpath(m_logPath); }
        removeOldLogFiles();
        if (!m_logPath.isEmpty() && !m_logPath.endsWith('/')) { m_logPath += '/'; }
        m_logFile.setFileName(getFullFileName());
        m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
        m_stream.setDevice(&m_logFile);
        m_stream.setCodec("UTF-8");
        writeHeaderToFile();

        connect(CLogHandler::instance(), &CLogHandler::localMessageLogged, this, &CFileLogger::ps_writeStatusMessageToFile);
        connect(CLogHandler::instance(), &CLogHandler::remoteMessageLogged, this, &CFileLogger::ps_writeStatusMessageToFile);
    }

    CFileLogger::~CFileLogger()
    {
        if (m_logFile.isOpen())
        {
            writeContentToFile(QStringLiteral("Application stopped."));
            m_logFile.close();
        }
    }

    void CFileLogger::ps_writeStatusMessageToFile(const BlackMisc::CStatusMessage &statusMessage)
    {
        if (! m_logPattern.match(statusMessage)) { return; }

        QString finalContent = QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss "));
        finalContent += statusMessage.getHumanReadableCategory();
        finalContent += " ";
        finalContent += statusMessage.getSeverityAsString();
        finalContent += ": ";
        finalContent += statusMessage.getMessage();
        writeContentToFile(finalContent);
    }

    QString CFileLogger::getFullFileName()
    {
        QString filename;
        Q_ASSERT(!m_applicationName.isEmpty());
        if (!m_logPath.isEmpty()) filename += m_logPath;
        filename += m_applicationName;
        filename += QLatin1String("_");
        filename += QDateTime::currentDateTime().toString(QStringLiteral("yyMMddhhmmss"));
        filename += QLatin1String(".log");
        return filename;
    }

    void CFileLogger::removeOldLogFiles()
    {
        QString nameFilter(m_applicationName);
        nameFilter += QLatin1String("*.log");
        QDir dir(m_logPath, nameFilter, QDir::Name, QDir::Files);

        for (const auto &logFile : dir.entryList())
        {
            if (QFileInfo(logFile).lastModified().daysTo(QDateTime::currentDateTime()) > 7 )
            {
                dir.remove(logFile);
            }
        }
    }

    void CFileLogger::writeHeaderToFile()
    {
        QString header(QStringLiteral("Application started."));
        writeContentToFile(header);
    }

    void CFileLogger::writeContentToFile(const QString &content)
    {
        m_stream << content << endl;
    }
}
