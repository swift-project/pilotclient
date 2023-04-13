/* Copyright (C) 2015
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/filelogger.h"
#include "blackmisc/swiftdirectories.h"
#include "blackconfig/buildconfig.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QIODevice>
#include <QString>
#include <QStringBuilder>
#include <QtGlobal>

using namespace BlackConfig;

namespace BlackMisc
{
    //! Get application name
    QString applicationName()
    {
        static const QString applicationName = QFileInfo(QCoreApplication::applicationFilePath()).completeBaseName();
        return applicationName;
    }

    //! Get log file name
    QString logFileName()
    {
        static const QString fileName = applicationName() %
                                        QLatin1String("_") %
                                        QDateTime::currentDateTimeUtc().toString(QStringLiteral("yyMMddhhmmss")) %
                                        QLatin1String("_") %
                                        QString::number(QCoreApplication::applicationPid()) %
                                        QLatin1String(".log");
        return fileName;
    }

    CFileLogger::CFileLogger(QObject *parent) : QObject(parent),
                                                m_logFile(this)
    {
        Q_ASSERT(!applicationName().isEmpty());
        QDir::root().mkpath(CSwiftDirectories::logDirectory());
        removeOldLogFiles();
        m_logFile.setFileName(getLogFilePath());
        m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
        m_stream.setDevice(&m_logFile);
        m_stream.setCodec("UTF-8");
        writeHeaderToFile();
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

    QString CFileLogger::getLogFileName()
    {
        return logFileName();
    }

    void CFileLogger::writeStatusMessageToFile(const BlackMisc::CStatusMessage &statusMessage)
    {
        if (statusMessage.isEmpty()) { return; }
        if (!m_logFile.isOpen()) { return; }
        if (!m_logPattern.match(statusMessage)) { return; }
        const QString categories = statusMessage.getCategoriesAsString();
        if (categories != m_previousCategories)
        {
            writeContentToFile(u"\n[" % categories % u']');
            m_previousCategories = categories;
        }
        const QString finalContent(QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss ")) % statusMessage.getSeverityAsString() % u": " % statusMessage.getMessage());

        writeContentToFile(finalContent);
    }

    QString CFileLogger::getLogFilePath()
    {
        QString filePath = CSwiftDirectories::logDirectory() % '/' % logFileName();
        return filePath;
    }

    void CFileLogger::removeOldLogFiles()
    {
        QString nameFilter(applicationName());
        nameFilter += QLatin1String("*.log");
        QDir dir(CSwiftDirectories::logDirectory(), nameFilter, QDir::Name, QDir::Files);

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
        m_stream << "This is " << applicationName();
        m_stream << " version " << CBuildConfig::getVersionString();
        m_stream << " running on " << QSysInfo::prettyProductName();
        m_stream << " " << QSysInfo::currentCpuArchitecture() << Qt::endl;

        m_stream << "Built from revision " << CBuildConfig::gitHeadSha1();
        m_stream << " on " << CBuildConfig::buildDateAndTime() << Qt::endl;

        m_stream << "Built with Qt " << QT_VERSION_STR;
        m_stream << " and running with Qt " << qVersion();
        m_stream << " " << QSysInfo::buildAbi() << Qt::endl;

        m_stream << "Application started." << Qt::endl;
    }

    void CFileLogger::writeContentToFile(const QString &content)
    {
        m_stream << content << Qt::endl;
    }
}
