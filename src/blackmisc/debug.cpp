#include "blackmisc/debug.h"
#include "blackmisc/log.h"
#include "blackmisc/display.h"
#include <QDir>
#include <QCoreApplication>

namespace BlackMisc
{

CDebug::CDebug()
    :   m_isInitialized(false), m_errorLog(NULL), m_warningLog(NULL),
        m_infoLog(NULL), m_debugLog(NULL)
{

}

void CDebug::create(const char * /** logPath **/, bool logInFile, bool eraseLastLog)
{
    if (!m_isInitialized)
    {
        setErrorLog(new CLog(IContext::getInstance(), CLog::eError));
        setWarningLog(new CLog(IContext::getInstance(), CLog::eWarning));
        setInfoLog(new CLog(IContext::getInstance(), CLog::eInfo));
        setDebugLog(new CLog(IContext::getInstance(), CLog::eDebug));

        stdDisplayer = new CStdDisplay("DEFAULT_SD");

        if (logInFile)
        {
            QDir fileinfo(m_logPath);
            QString fn;
            if (!m_logPath.isEmpty())
            {
                m_logPath = fileinfo.absolutePath();
                fn += m_logPath;
            }
            else
            {
            }

            fileDisplayer = new CFileDisplay("", true, "DEFAULT_FD");
            fileDisplayer = new CFileDisplay("", eraseLastLog, "DEFAULT_FD");
        }

        init(true);
        m_isInitialized = true;
    }
}

void CDebug::init(bool logInFile)
{
    m_debugLog->attachDisplay(stdDisplayer);
    m_infoLog->attachDisplay(stdDisplayer);
    m_warningLog->attachDisplay(stdDisplayer);
    m_errorLog->attachDisplay(stdDisplayer);

    if (logInFile)
    {
        m_debugLog->attachDisplay(fileDisplayer);
        m_infoLog->attachDisplay(fileDisplayer);
        m_warningLog->attachDisplay(fileDisplayer);
        m_errorLog->attachDisplay(fileDisplayer);
    }
}

QString CDebug::getLogDirectory()
{
    return m_logPath;
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

CLogMessage CDebug::blackInfo(int line, const char *fileName, const char *methodName)
{
    create();
    m_infoLog->setLogInformation(line, fileName, methodName);
    return CLogMessage(*this, CLog::eInfo);
}

CLogMessage CDebug::blackWarning(int line, const char *fileName, const char *methodName)
{
    create();
    m_warningLog->setLogInformation(line, fileName, methodName);
    return CLogMessage(*this, CLog::eWarning);
}

CLogMessage CDebug::blackDebug(int line, const char *fileName, const char *methodName)
{
    create();
    m_debugLog->setLogInformation(line, fileName, methodName);
    return CLogMessage(*this, CLog::eDebug);
}

CLogMessage CDebug::blackError(int line, const char *fileName, const char *methodName)
{
    create();
    m_errorLog->setLogInformation(line, fileName, methodName);
    return CLogMessage(*this, CLog::eError);
}

} // namespace BlackMisc
