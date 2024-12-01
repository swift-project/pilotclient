// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_FILELOGGER_H
#define SWIFT_MISC_FILELOGGER_H

#include <QFile>
#include <QObject>
#include <QString>
#include <QTextStream>

#include "misc/logpattern.h"
#include "misc/statusmessage.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    //! Class to write log messages to file
    class SWIFT_MISC_EXPORT CFileLogger : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor.
        //! Filename defaults to QCoreApplication::applicationName() and path to "."
        CFileLogger(QObject *parent = nullptr);

        //! Destructor.
        virtual ~CFileLogger();

        //! Change the log pattern. Default is to log all messages.
        void changeLogPattern(const CLogPattern &pattern) { m_logPattern = pattern; }

        //! Close file
        void close();

        //! Get the log file name
        static QString getLogFileName();

        //! Get the log file path (including its name)
        static QString getLogFilePath();

    public slots:
        //! Write single status message to file
        void writeStatusMessageToFile(const swift::misc::CStatusMessage &statusMessage);

    private:
        void removeOldLogFiles();
        void writeHeaderToFile();
        void writeContentToFile(const QString &content);

        CLogPattern m_logPattern;
        QFile m_logFile;
        QString m_fileName;
        QTextStream m_stream;
        QString m_previousCategories;
    };
} // namespace swift::misc

#endif // SWIFT_MISC_FILELOGGER_H
