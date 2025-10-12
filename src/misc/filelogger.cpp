// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/filelogger.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QIODevice>
#include <QString>
#include <QStringBuilder>
#include <QtGlobal>

#include "config/buildconfig.h"
#include "misc/swiftdirectories.h"
#include "misc/verify.h"

using namespace swift::config;

namespace swift::misc
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
        static const QString fileName = applicationName() % QLatin1String("_") %
                                        QDateTime::currentDateTimeUtc().toString(QStringLiteral("yyMMddhhmmss")) %
                                        QLatin1String("_") % QString::number(QCoreApplication::applicationPid()) %
                                        QLatin1String(".log");
        return fileName;
    }

    CFileLogger::CFileLogger(QObject *parent) : QObject(parent), m_logFile(this)
    {
        Q_ASSERT(!applicationName().isEmpty());
        QDir::root().mkpath(CSwiftDirectories::logDirectory());
        removeOldLogFiles();
        m_logFile.setFileName(getLogFilePath());
        const bool res = m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
        SWIFT_VERIFY_X(res, Q_FUNC_INFO, "Could not open log file");
        m_stream.setDevice(&m_logFile);
        m_stream.setEncoding(QStringConverter::Utf8);
        writeHeaderToFile();
    }

    CFileLogger::~CFileLogger() { this->close(); }

    void CFileLogger::close()
    {
        if (m_logFile.isOpen())
        {
            disconnect(this); // disconnect from log handler
            writeContentToFile(QStringLiteral("Logging stops."));
            m_logFile.close();
        }
    }

    QString CFileLogger::getLogFileName() { return logFileName(); }

    void CFileLogger::writeStatusMessageToFile(const swift::misc::CStatusMessage &statusMessage)
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
        const QString finalContent(QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss ")) %
                                   statusMessage.getSeverityAsString() % u": " % statusMessage.getMessage());

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
            if (logFileInfo.lastModified().daysTo(now) > 7) { dir.remove(logFileInfo.fileName()); }
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

    void CFileLogger::writeContentToFile(const QString &content) { m_stream << content << Qt::endl; }
} // namespace swift::misc
