#include "blackmisc/logmessage.h"
#include "blackmisc/debug.h"

namespace BlackMisc
{

    CLogMessage::LogStream::LogStream(CLog::TLogType type)
        : output(&buffer, QIODevice::WriteOnly), reference(1),
          type(type), needSpace(true), enableOutput(true)
    {}

    CLogMessage::CLogMessage(CDebug &debug_, CLog::TLogType type)
        : debug(debug_), logStream(new LogStream(type))
    {
    }

    CLogMessage::CLogMessage(const CLogMessage &other)
        : debug(other.debug), logStream(other.logStream)
    {
        ++logStream->reference;
    }

    CLogMessage::~CLogMessage()
    {
        if (!--logStream->reference)
        {
            if (logStream->enableOutput)
            {
                switch (logStream->type)
                {
                case CLog::eWarning:
                    debug.getWarningLog()->printWithNewLine(logStream->buffer);
                    break;

                case CLog::eInfo:
                    debug.getInfoLog()->printWithNewLine(logStream->buffer);
                    break;

                case CLog::eDebug:
                    debug.getDebugLog()->printWithNewLine(logStream->buffer);
                    break;

                case CLog::eError:
                default:
                    debug.getErrorLog()->printWithNewLine(logStream->buffer);
                    break;
                }
            }
            delete logStream;
        }
    }

} // namespace Blackib
