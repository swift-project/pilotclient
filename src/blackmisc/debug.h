//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef BLACKMISC_DEBUG_H
#define BLACKMISC_DEBUG_H

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
    class CDebug : public QObject
    {
        Q_OBJECT

    public:
        BLACK_INTERFACE(BlackMisc::CDebug)

        CDebug();

        // internal use only
        void create(const char *logPath = "", bool logInFile = true, bool eraseLastLog = false);

        // init Debug
        void init(bool logInFile);

        /// Do not call this, unless you know what you're trying to do (it kills debug)!
        void destroy() {}

        void changeLogDirectory(const QString &dir) { Q_UNUSED(dir); }

        QString getLogDirectory();

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

        CLogMessage blackInfo(int line, const char *fileName, const char *methodName);
        CLogMessage blackWarning(int line, const char *fileName, const char *methodName);
        CLogMessage blackDebug(int line, const char *fileName, const char *methodName);
        CLogMessage blackError(int line, const char *fileName, const char *methodName);

    private:
        bool m_isInitialized;

        CStdDisplay *stdDisplayer;
        CFileDisplay *fileDisplayer;

        QString m_logPath;

        CLog *m_errorLog;
        CLog *m_warningLog;
        CLog *m_infoLog;
        CLog *m_debugLog;
    };

#define bInfo(CONTEXT)      ( (CONTEXT).getDebug()->blackInfo(__LINE__, __FILE__, __FUNCTION__ ) )
#define bWarning(CONTEXT)   ( (CONTEXT).getDebug()->blackWarning(__LINE__, __FILE__, __FUNCTION__ ) )
#define bDebug(CONTEXT)     ( (CONTEXT).getDebug()->blackDebug(__LINE__, __FILE__, __FUNCTION__ ) )
#define bError(CONTEXT)     ( (CONTEXT).getDebug()->blackError(__LINE__, __FILE__, __FUNCTION__ ) )

#define bAppInfo    bInfo(BlackMisc::IContext::getInstance())
#define bAppWarning bWarning(BlackMisc::IContext::getInstance())
#define bAppDebug   bDebug(BlackMisc::IContext::getInstance())
#define bAppError   bError(BlackMisc::IContext::getInstance())

} // namespace BlackMisc

#endif //BLACKMISC_DEBUG_H
