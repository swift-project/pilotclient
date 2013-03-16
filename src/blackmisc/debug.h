//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef DEBUG_H
#define DEBUG_H

#include "blackmisc/context.h"
#include "blackmisc/log.h"
#include "blackmisc/display.h"
#include "blackmisc/logmessage.h"

// This header contains debug macros

namespace BlackMisc
{
	//! class CDebug 
	/*! This class implements the logging of the library. It holds a list of displays 
		takes care of any additional information needed, e.g. timestamps, filename etc.
		To use the logging use the default displayer or implement your own one and
		register it with this class.
	*/
	class CDebug
	{
	public:

        CDebug();


		// internal use only
		void create (QString &logPath = QString(""), bool logInFile = true, bool eraseLastLog = false);

		// init Debug
		void init(bool logInFile);

		/// Do not call this, unless you know what you're trying to do (it kills debug)!
		void destroy() {}

        void changeLogDirectory(const QString &dir) { Q_UNUSED(dir); }

		QString getLogDirectory();

		// Assert function
		
		//! Method returns the pointer to the global assert log object
		/*!
		  \return Pointer to CLog object
		*/
		CLog *getAssertLog();
		
		//! Pure virtual function to set the global assert log object
		/*!
		  \param Pointer to CLog object
		*/
        void setAssertLog(CLog *assertLog);
		
		//! Pure virtual method returns the pointer to the global error log object
		/*!
		  \return Pointer to CLog object
		*/
        CLog *getErrorLog();
		
		//! Pure virtual function to set the global error log object
		/*!
		  \param Pointer to CLog object
		*/
        void setErrorLog(CLog *errorLog);
		
		//! Pure virtual method returns the pointer to the global warning log object
		/*!
		  \return Pointer to CLog object
		*/
        CLog *getWarningLog();
		
		//! Pure virtual function to set the global warning log object
		/*!
		  \param Pointer to CLog object
		*/
        void setWarningLog(CLog *warningLog);
		
		//! Pure virtual method returns the pointer to the global info log object
		/*!
		  \return Pointer to CLog object
		*/
        CLog *getInfoLog();
		
		//! Pure virtual function to set the global info log object
		/*!
		  \param Pointer to CLog object
		*/
        void setInfoLog(CLog *infoLog);
		
		//! Pure virtual method returns the pointer to the global debug log object
		/*!
		  \return Pointer to CLog object
		*/
        CLog *getDebugLog();
		
		//! Pure virtual function to set the global debug log object
		/*!
		  \param Pointer to CLog object
		*/
        void setDebugLog(CLog *debugLog);

		void assertFailed(int line, const char *file, const char* function, const char *exp);
		void assertFailedString(int line, const char *fileName, const char *methodName, const char* exp, const char* string);
		
		CLogMessage blackInfo(int line, const char *fileName, const char *methodName);
		CLogMessage blackWarning(int line, const char *fileName, const char *methodName);
		CLogMessage blackDebug(int line, const char *fileName, const char *methodName);
		CLogMessage blackError(int line, const char *fileName, const char *methodName);
		CLogMessage blackAssert(int line, const char *fileName, const char *methodName);
		CLogMessage blackAssertqstr(int line, const char *fileName, const char *methodName);
		
	private:
		bool m_isInitialized;
			
        CStdDisplay *stdDisplayer;
        CFileDisplay *fileDisplayer;
		
		QString m_logPath;
			
		CLog *m_errorLog;
		CLog *m_warningLog;
        CLog *m_infoLog;
		CLog *m_debugLog;
		CLog *m_assertLog;
	};

   /*!
	  Macro is not defined in VC6
	*/
    #if _MSC_VER <= 1200
    #	define __FUNCTION__ NULL
    #endif
	
    #define bInfo ( BlackMisc::IContext::getInstance().getDebug()->blackInfo(__LINE__, __FILE__, __FUNCTION__ ) )
    #define bWarning ( BlackMisc::IContext::getInstance().getDebug()->blackWarning(__LINE__, __FILE__, __FUNCTION__ ) )
    #define bDebug ( BlackMisc::IContext::getInstance().getDebug()->blackDebug(__LINE__, __FILE__, __FUNCTION__ ) )
    #define bError ( BlackMisc::IContext::getInstance().getDebug()->blackError(__LINE__, __FILE__, __FUNCTION__ ) )
    #define bAssert(exp) if (!(exp)) BlackMisc::IContext::getInstance().getDebug()->assertFailed(__LINE__, __FILE__, __FUNCTION__, #exp)
	#define bAssertstr(exp, str) if (!(exp)) BlackMisc::IContext::getInstance().getDebug()->assertFailedString(__LINE__, __FILE__, __FUNCTION__, #exp, #str)

} // namespace BlackMisc

#endif DEBUG_H // DEBUG_H
