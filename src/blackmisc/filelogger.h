/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_FILELOGGER_H
#define BLACKMISC_FILELOGGER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/logpattern.h"
#include "blackmisc/statusmessage.h"

#include <QFile>
#include <QObject>
#include <QString>
#include <QTextStream>

namespace BlackMisc
{
    //! Class to write log messages to file
    class BLACKMISC_EXPORT CFileLogger : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor.
        //! Filename defaults to QCoreApplication::applicationName() and path to "."
        CFileLogger(QObject *parent = nullptr);

        /*!
         * Constructor
         * \param applicationName Use the applications name without any extension.
         *                        A timestamp and extension will be added automatically.
         * \param logPath Path the log files is written to. If you leave this empty, the
         *                file will be written in the working directory of the binary.
         * \param parent QObject parent
         */
        CFileLogger(const QString &applicationName, const QString &logPath, QObject *parent = nullptr);

        //! Destructor.
        virtual ~CFileLogger();

        //! Change the log pattern. Default is to log all messages.
        void changeLogPattern(const CLogPattern &pattern) { m_logPattern = pattern; }

        //! Close file
        void close();

        //! Get the current log file path
        QString getLogFilePath() const { return m_logFile.fileName(); }

        //! Get the current log file name
        QString getLogFileName() const { return m_fileName; }

    private slots:
        //! Write single status message to file
        void ps_writeStatusMessageToFile(const BlackMisc::CStatusMessage &statusMessage);

    private:
        QString getFullFileName();
        void removeOldLogFiles();

        void writeHeaderToFile();
        void writeContentToFile(const QString &content);

        CLogPattern m_logPattern;
        QFile m_logFile;
        QString m_fileName;
        QTextStream m_stream;
        QString m_applicationName;
        QString m_logPath; //!< Empty by default. Hence the working directory "." is used
        QString m_previousCategories;
    };
}

#endif
