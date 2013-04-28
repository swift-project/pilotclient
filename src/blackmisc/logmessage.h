//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H

#include "blackmisc/log.h"
#include "blackmisc/context.h"
#include <QTextStream>

namespace BlackMisc
{
    class CDebug;

    class CLogMessage
    {
        struct LogStream
        {
            //! Constructor
            LogStream(CLog::TLogType type);

            QTextStream output;

            //! Message Buffer
            QString buffer;

            //! Logging type
            CLog::TLogType type;
            bool needSpace;
            bool enableOutput;

            //! Reference count
            quint32 reference;
        } *logStream;

        CDebug &debug;

    public:
        CLogMessage(CDebug &debug_, CLog::TLogType type);
        CLogMessage(const CLogMessage &other);

        ~CLogMessage();

        inline CLogMessage &maybeSpace() { if (logStream->needSpace) logStream->output << ' '; return *this; }

        inline CLogMessage &operator<<(QChar t) { logStream->output << '\'' << t << '\''; return maybeSpace(); }
        inline CLogMessage &operator<<(QBool t) { logStream->output << (bool(t != 0) ? "true" : "false"); return maybeSpace(); }
        inline CLogMessage &operator<<(bool t) { logStream->output << (t ? "true" : "false"); return maybeSpace(); }
        inline CLogMessage &operator<<(char t) { logStream->output << t; return maybeSpace(); }
        inline CLogMessage &operator<<(signed short t) { logStream->output << t; return maybeSpace(); }
        inline CLogMessage &operator<<(unsigned short t) { logStream->output << t; return maybeSpace(); }
        inline CLogMessage &operator<<(signed int t) { logStream->output << t; return maybeSpace(); }
        inline CLogMessage &operator<<(unsigned int t) { logStream->output << t; return maybeSpace(); }
        inline CLogMessage &operator<<(signed long t) { logStream->output << t; return maybeSpace(); }
        inline CLogMessage &operator<<(unsigned long t) { logStream->output << t; return maybeSpace(); }
        inline CLogMessage &operator<<(qint64 t)
        { logStream->output << QString::number(t); return maybeSpace(); }
        inline CLogMessage &operator<<(quint64 t)
        { logStream->output << QString::number(t); return maybeSpace(); }
        inline CLogMessage &operator<<(float t) { logStream->output << t; return maybeSpace(); }
        inline CLogMessage &operator<<(double t) { logStream->output << t; return maybeSpace(); }
        inline CLogMessage &operator<<(const char *t) { logStream->output << QString::fromAscii(t); return maybeSpace(); }
        inline CLogMessage &operator<<(const QString &t) { logStream->output << '\"' << t  << '\"'; return maybeSpace(); }
        inline CLogMessage &operator<<(const QByteArray &t) { logStream->output  << '\"' << t << '\"'; return maybeSpace(); }

        inline CLogMessage &operator<<(QTextStreamManipulator m)
        { logStream->output << m; return *this; }


    };

} // BlackMisc

#endif // guard
