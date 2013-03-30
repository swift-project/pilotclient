#include <QDir>
#include <QCoreApplication>
#include "blackmisc/log.h"
#include "blackmisc/debug.h"
#include "blackmisc/display.h"

namespace BlackMisc
{

    CDebug::CDebug()
        :   m_isInitialized(false), m_errorLog(NULL), m_warningLog(NULL),
          m_infoLog(NULL), m_debugLog(NULL), m_assertLog(NULL)
    {

    }

    void CDebug::create (const char *logPath, bool logInFile, bool eraseLastLog)
	{
		if (!m_isInitialized)
		{
			setErrorLog(new CLog (CLog::ERROR));
			setWarningLog(new CLog (CLog::WARNING));
			setInfoLog(new CLog (CLog::INFO));
			setDebugLog(new CLog (CLog::DEBUG));
			setAssertLog(new CLog (CLog::ASSERT));

			stdDisplayer = new CStdDisplay ("DEFAULT_SD");

			if (logInFile)
			{
                QDir fileinfo(m_logPath);
				QString fn;
				if ( !m_logPath.isEmpty() )
				{
					m_logPath = fileinfo.absolutePath();
					fn += m_logPath;
				}
				else
				{
				}

				fileDisplayer = new CFileDisplay ("", true, "DEFAULT_FD");
				fileDisplayer = new CFileDisplay ("", eraseLastLog, "DEFAULT_FD");
			}

			init(true);
			m_isInitialized = true;
		}
	}

	void CDebug::init (bool logInFile)
	{
		m_debugLog->attachDisplay(stdDisplayer);
		m_infoLog->attachDisplay (stdDisplayer);
		m_warningLog->attachDisplay (stdDisplayer);
		m_assertLog->attachDisplay (stdDisplayer);
		m_errorLog->attachDisplay (stdDisplayer);

		if (logInFile)
		{
			m_debugLog->attachDisplay (fileDisplayer);
			m_infoLog->attachDisplay (fileDisplayer);
			m_warningLog->attachDisplay (fileDisplayer);
			m_assertLog->attachDisplay (fileDisplayer);
			m_errorLog->attachDisplay (fileDisplayer);
		}
	}

	QString CDebug::getLogDirectory()
	{
		return m_logPath;
	}

	void CDebug::assertFailed(int line, const char *file, const char* function, const char *exp)
	{
		m_assertLog->setLogInformation (line, file, function);
		BlackMisc::CLogMessage::getAssertMsgObj() << "ASSERT FAILED! STOP!";

    #ifdef BB_GUI
        QMessageBox::critical(0, "ASSERT FAILED",
					  QString("%1 %2 %3 () - failed assert: %4").
							  arg(QString(file)).arg(line).arg(QString(function)).arg(QString(exp)));
	#endif
	
		qApp->quit();
		exit(1);

	}

    CLog *CDebug::getErrorLog()
    {
        return m_errorLog;
    }

    void CDebug::setErrorLog(CLog *errorLog)
    {
        m_errorLog = errorLog;
    }

    CLog *CDebug::getWarningLog()
    {
        return m_warningLog;
    }

    void CDebug::setWarningLog(CLog *warningLog)
    {
        m_warningLog = warningLog;
    }

    CLog *CDebug::getInfoLog()
    {
        return m_infoLog;
    }

    void CDebug::setInfoLog(CLog *infoLog)
    {
        m_infoLog = infoLog;
    }

    CLog *CDebug::getDebugLog()
    {
        return m_debugLog;
    }

    void CDebug::setDebugLog(CLog *debugLog)
    {
        m_debugLog = debugLog;
    }

    CLog *CDebug::getAssertLog()
    {
        return m_assertLog;
    }

    void CDebug::setAssertLog(CLog *assertLog)
    {
        m_assertLog = assertLog;
    }

    void CDebug::assertFailedString(int line, const char *fileName, const char *methodName, const char* exp, const char* string)
    {
        m_assertLog->setLogInformation (line, fileName, methodName);
        BlackMisc::CLogMessage::getAssertMsgObj() << "ASSERT FAILED: " << string;
    #ifdef BB_GUI
        QMessageBox::critical(0, "ASSERT FAILED", string);
    #endif
    }

	CLogMessage CDebug::blackInfo(int line, const char *fileName, const char *methodName)
	{
		create();
		m_infoLog->setLogInformation( line, fileName, methodName);
        return BlackMisc::CLogMessage::getInfoMsgObj();
	}
	
	CLogMessage CDebug::blackWarning(int line, const char *fileName, const char *methodName)
	{
		create();
		m_warningLog->setLogInformation( line, fileName, methodName);
        return BlackMisc::CLogMessage::getWarningMsgObj();
	}
	
	CLogMessage CDebug::blackDebug(int line, const char *fileName, const char *methodName)
	{
		create();
		m_debugLog->setLogInformation( line, fileName, methodName);
        return BlackMisc::CLogMessage::getDebugMsgObj();
	}
	
	CLogMessage CDebug::blackError(int line, const char *fileName, const char *methodName)
	{
		create();
		m_errorLog->setLogInformation( line, fileName, methodName);
        return BlackMisc::CLogMessage::getErrorMsgObj();
	}

	CLogMessage CDebug::blackAssert(int line, const char *fileName, const char *methodName)
	{
		create();
		m_assertLog->setLogInformation( line, fileName, methodName);
        return BlackMisc::CLogMessage::getAssertMsgObj();
	}
	
	CLogMessage CDebug::blackAssertqstr(int line, const char *fileName, const char *methodName)
	{
		create();
		m_assertLog->setLogInformation( line, fileName, methodName);
        return BlackMisc::CLogMessage::getAssertMsgObj();
	}

} // namespace BlackMisc
