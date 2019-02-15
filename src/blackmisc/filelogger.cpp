/* Copyright (C) 2015
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackmisc/filelogger.h"
#include "blackmisc/loghandler.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QVersionNumber>
#include <QDir>
#include <QFileInfo>
#include <QFlags>
#include <QIODevice>
#include <QLatin1String>
#include <QString>
#include <QStringBuilder>
#include <QtGlobal>

using namespace BlackConfig;

namespace BlackMisc
{
    CFileLogger::CFileLogger(QObject *parent) :
        CFileLogger(QCoreApplication::applicationName(), QString(), parent)
    {
        // void
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
        this->close();
    }

    void CFileLogger::close()
    {
        if (m_logFile.isOpen())
        {
            disconnect(this); // disconnect from log handler
            writeContentToFile(QStringLiteral("Logging stops."));
            m_logFile.close();
        }
    }

    void CFileLogger::ps_writeStatusMessageToFile(const BlackMisc::CStatusMessage &statusMessage)
    {
        if (statusMessage.isEmpty()) { return; }
        if (!m_logFile.isOpen()) { return; }
        if (! m_logPattern.match(statusMessage)) { return; }
        const QString categories = statusMessage.getCategoriesAsString();
        if (categories != m_previousCategories)
        {
            writeContentToFile(u"\n[" % categories % u']');
            m_previousCategories = categories;
        }
        const QString finalContent(QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss "))
                                   % statusMessage.getSeverityAsString()
                                   % u": "
                                   % statusMessage.getMessage());

        writeContentToFile(finalContent);
    }

    QString CFileLogger::getFullFileName()
    {
        QString filePath;
        Q_ASSERT(!m_applicationName.isEmpty());
        if (!m_logPath.isEmpty()) filePath += m_logPath;

        m_fileName += m_applicationName;
        m_fileName += QLatin1String("_");
        m_fileName += QDateTime::currentDateTime().toString(QStringLiteral("yyMMddhhmmss"));
        m_fileName += QLatin1String("_");
        m_fileName += QString::number(QCoreApplication::applicationPid());
        m_fileName += QLatin1String(".log");
        return filePath + m_fileName;
    }

    void CFileLogger::removeOldLogFiles()
    {
        QString nameFilter(m_applicationName);
        nameFilter += QLatin1String("*.log");
        QDir dir(m_logPath, nameFilter, QDir::Name, QDir::Files);

        QDateTime now = QDateTime::currentDateTime();
        for (const auto &logFileInfo : dir.entryInfoList())
        {
            if (logFileInfo.lastModified().daysTo(now) > 7)
            {
                dir.remove(logFileInfo.fileName());
            }
        }
    }

    void CFileLogger::writeHeaderToFile()
    {
        m_stream << "This is " << m_applicationName;
        m_stream << " version " << CBuildConfig::getVersionString();
        m_stream << " running on " << QSysInfo::prettyProductName();
        m_stream << " " << QSysInfo::currentCpuArchitecture() << endl;

        m_stream << "Built from revision " << CBuildConfig::gitHeadSha1();
        m_stream << " on " << CBuildConfig::buildDateAndTime() << endl;

        m_stream << "Built with Qt " << QT_VERSION_STR;
        m_stream << " and running with Qt " << qVersion();
        m_stream << " " << QSysInfo::buildAbi() << endl;

        m_stream << "Program is going to expire on " + CBuildConfig::getEol().toString() << "." << endl;

        m_stream << "Application started." << endl;
    }

    void CFileLogger::writeContentToFile(const QString &content)
    {
        m_stream << content << endl;
    }
}
