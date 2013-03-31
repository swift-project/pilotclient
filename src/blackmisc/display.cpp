//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include <QTextStream>
#include <QFileInfo>

#include "blackmisc/context.h"
#include "blackmisc/display.h"

#include <errno.h>

namespace BlackMisc
{

    static const char *LogTypeToString[7] =
        { "", "Error", "Warning", "Info", "Debug", "Assert", "Unknown" };
	
    ILogDisplay::ILogDisplay(const QString &displayName)
    {
        m_mutex = new QMutex ();
        DisplayName = displayName;
    }
	
    ILogDisplay::~ILogDisplay()
    {
        delete m_mutex;
    }

    const char *ILogDisplay::logTypeToString (CLog::TLogType logType)
    {
        if (logType < CLog::OFF || logType > CLog::UNKNOWN)
            return "Not defined";

        return LogTypeToString[logType];
    }

    QString ILogDisplay::currentDateToString ()
    {
        QDateTime currentDateTime = QDateTime::currentDateTime();
		QString format("yyyy-MM-dd hh:mm:ss");
        return currentDateTime.toString(format);
    }

    QString ILogDisplay::dateToString (const QDateTime &time)
    {
        QString format("yyyy-MM-dd hh:mm:ss");
		return time.toString(format);
    }

    QString ILogDisplay::headLine ()
    {
        QString header = QString("\n Logging started at %1\n").arg(currentDateToString());
        return header;
    }

    /*
     * Display the string where it does.
     */
    void ILogDisplay::print (const CLog::SLogInformation &logInformation, const QString &message)
    {
        m_mutex->lock();
        doPrint( logInformation, message );
        m_mutex->unlock();
    }

    void CStdDisplay::doPrint (const CLog::SLogInformation &logInformation, const QString &message)
    {
        bool needSpace = false;
        QString line;

        if (logInformation.m_logType != CLog::OFF)
        {
            line += logTypeToString(logInformation.m_logType);
            needSpace = true;
        }

        // Write thread identifier
        if ( logInformation.m_threadId != 0 )
        {
            if (needSpace) { line += " "; needSpace = false; }
            line += QString("%1").arg(logInformation.m_threadId);
        }

        if (logInformation.m_sourceFile != NULL)
        {
            if (needSpace) { line += " "; needSpace = false; }
            line += QFileInfo (logInformation.m_sourceFile).fileName();
            needSpace = true;
        }

        if (logInformation.m_line != -1)
        {
            if (needSpace) { line += " "; needSpace = false; }
            line += QString("%1").arg(logInformation.m_line);
            needSpace = true;
        }

        if (logInformation.m_methodName != NULL)
        {
            if (needSpace) { line += " "; needSpace = false; }
            line += logInformation.m_methodName;
            needSpace = true;
        }

        if (logInformation.m_applicationName != NULL)
        {
            if (needSpace) { line += " "; needSpace = false; }
            line += logInformation.m_applicationName;
            needSpace = true;
        }

        if (needSpace) { line += " : "; needSpace = false; }

        line += message;

        static bool consoleMode = true;

        QTextStream out(stdout);
        out << line;
    }

    CFileDisplay::CFileDisplay (const QString &filename, bool eraseLastLog, const QString &displayName) :
        ILogDisplay (displayName), m_needHeader(true)
    {
        setParam (filename, eraseLastLog);

    }

    CFileDisplay::CFileDisplay () :
        ILogDisplay (""), m_needHeader(true)
    {

    }

    CFileDisplay::~CFileDisplay ()
    {
        if ( m_file->handle() != -1 )
        {
            m_file->close();
        }

        delete m_file;
    }

    void CFileDisplay::setParam (const QString &filename, bool eraseLastLog)
    {
        m_fileName = filename;

        if (filename.isEmpty())
        {
            // Call this first, otherwise the Singleton pointer is NULL
            CLog::setDefaultApplicationName();

            // Read the process name and name the log accordingly
            QString *processName = (QString *)IContext::getInstance().singletonPointer("BlackMisc::CLog::m_applicationName");

            // Just in case
            if ( processName != NULL )
                m_fileName = QFileInfo(*processName).baseName() + ".log";
            else
                printf("Cannot create log file without a filename!");
        }

        m_file = new QFile(m_fileName);

        if (eraseLastLog)
        {
            // Erase all the derived log files
            int i = 0;
            bool fileExist = true;
            while (fileExist)
            {
                QFileInfo info(m_fileName);
                QString fileToDelete = info.absolutePath() + "/" + info.baseName() + QString("%1").arg(i, 3, 10, QChar('0')) + info.completeSuffix();
                if ( QFile::exists(fileToDelete) )
                    QFile(fileToDelete).remove();
                else
                    fileExist = false;
                i++;
            }
        }

        if ( m_file->handle() != -1 )
        {
            m_file->close();
        }
    }

    void CFileDisplay::doPrint (const CLog::SLogInformation &logInformation, const QString &message)
    {
        bool needSpace = false;
        QString line;

        // if the filename is not set, don't log
        if ( m_fileName.isEmpty() ) return;

        if (logInformation.m_dateTime.isValid())
        {
            line += dateToString(logInformation.m_dateTime);
            needSpace = true;
        }

        if (logInformation.m_logType != CLog::OFF)
        {
            if (needSpace) { line += " "; needSpace = false; }
            line += logTypeToString(logInformation.m_logType);
            needSpace = true;
        }

        if (!logInformation.m_applicationName.isEmpty())
        {
            if (needSpace) { line += " "; needSpace = false; }
            line += logInformation.m_applicationName;
            needSpace = true;
        }

        if (logInformation.m_sourceFile != NULL)
        {
            if (needSpace) { line += " "; needSpace = false; }
            line += QFileInfo (logInformation.m_sourceFile).fileName();
            needSpace = true;
        }

        if (logInformation.m_line != -1)
        {
            if (needSpace) { line += " "; needSpace = false; }
            line += QString("%1").arg(logInformation.m_line);
            needSpace = true;
        }

        if (logInformation.m_methodName != NULL)
        {
            if (needSpace) { line += " "; needSpace = false; }
            line += logInformation.m_methodName;
            needSpace = true;
        }

        if (needSpace) { line += " : "; needSpace = false; }

        line += message;

        // Try to open the file if the handle is invalid
        if (m_file->handle() == -1)
        {
            if ( !m_file->open(QIODevice::WriteOnly) )
                printf ("Can't open log file '%s': %s\n", m_fileName.toLatin1().constData(), strerror (errno));
        }

        if (m_file->handle() != -1)
        {
            QTextStream out(m_file);

            if (m_needHeader)
            {
                const QString hs = headLine();
                out << hs;
                m_needHeader = false;
            }

            if(!line.isEmpty())
                out << line;
        }
    }
}
