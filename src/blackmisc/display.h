//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef DISPLAY_H
#define DISPLAY_H

#include "blackmisc/log.h"
#include <QFile>
#include <QMutex>

namespace BlackMisc
{
    class ILogDisplay
    {
    public:
        /// Constructor
        ILogDisplay(const QString &displayName );

        /// Destructor
        virtual ~ILogDisplay();

        /// Display the string where it does.
        void print( const CLog::SLogInformation &logInformation, const QString &message );

        /// This is the identifier for a displayer, it is used to find or remove a displayer
        QString DisplayName;

    protected:
        /// Method to implement in the derived class
        virtual void doPrint(const CLog::SLogInformation &logInformation, const QString &message) = 0;

        // Return the header string with date (for the first line of the log)
        static QString headLine ();

    public:

        /// Convert log type to string
        static const char *logTypeToString (CLog::TLogType logType);

        /// Convert the current date to human string
        static QString currentDateToString ();

        /// Convert date to "2000/01/14 10:05:17" string
        static QString dateToString (const QDateTime &time);

    private:

        QMutex *m_mutex;

    };

    class CStdDisplay : virtual public ILogDisplay
    {
    public:
        CStdDisplay ( const QString &displayerName = QString("")) : ILogDisplay (displayerName) {}

    protected:

        /// Display the string to stdout and OutputDebugString on Windows
        virtual void doPrint (const CLog::SLogInformation &logInformation, const QString &message);

    };

    class CFileDisplay : virtual public ILogDisplay
    {
    public:

        /// Constructor
        CFileDisplay (const QString &filename, bool eraseLastLog = false, const QString &displayName = "");

        CFileDisplay ();

        ~CFileDisplay ();

        /// Set Parameter of the displayer if not set at the ctor time
        void setParam (const QString &filename, bool eraseLastLog = false);

    protected:
        /// Put the string into the file.
        virtual void doPrint (const CLog::SLogInformation &logInformation, const QString &message);

    private:
        QString     m_fileName;

        QFile      *m_file;

        bool		m_needHeader;
    };
} // BlackMisc


#endif // DISPLAY_H
