#include "blackmisc/logmessage.h"
#include "blackmisc/debug.h"

namespace BlackMisc
{

	CLogMessage::LogStream::LogStream(CLog::TLogType type) 
		: output(&buffer, QIODevice::WriteOnly), reference(1), 
		type(type), needSpace(true), enableOutput(true) 
		{}

	CLogMessage::CLogMessage(CLog::TLogType type)
		:	logStream(new LogStream(type))
	{
	}

	CLogMessage::CLogMessage(const CLogMessage &other)
		:	logStream(other.logStream)
	{
		++logStream->reference;
	}

    CLogMessage::~CLogMessage() {
		 if (!--logStream->reference) {
			if(logStream->enableOutput) {
				switch (logStream->type)
				{
				case CLog::WARNING:
					BlackMisc::IContext::getInstance().getDebug()->getWarningLog()->printWithNewLine(logStream->buffer);
					break;

				case CLog::INFO:
					BlackMisc::IContext::getInstance().getDebug()->getInfoLog()->printWithNewLine(logStream->buffer);
					break;

				case CLog::DEBUG:
					BlackMisc::IContext::getInstance().getDebug()->getDebugLog()->printWithNewLine(logStream->buffer);
					break;

				case CLog::ERROR:
					BlackMisc::IContext::getInstance().getDebug()->getErrorLog()->printWithNewLine(logStream->buffer);
					break;

				case CLog::ASSERT:
					BlackMisc::IContext::getInstance().getDebug()->getAssertLog()->printWithNewLine(logStream->buffer);
					break;
				}

			}
			delete logStream;
		 }
    }

} // namespace Blackib
