//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef UNICODE
#define UNICODE
#endif

#include <QTextStream>
#include <QFileInfo>
#include <QtGlobal>

#include "blackmisc/log.h"
#include "blackmisc/display.h"
#include "blackmisc/debug.h"

#ifdef Q_OS_WIN
#	define NOMINMAX
#	include <process.h>
#	include <windows.h>
#else
#	include <unistd.h>
#endif

namespace BlackMisc
{
    QString *CLog::m_applicationName = NULL;

    CLog::CLog(TLogType logType) : m_logType (logType)
	{
	}

	CLog::~CLog(void)
	{
	}

    void CLog::setDefaultApplicationName()
    {
		//! Get the object from the singleton context and create a new one if not already done
        if (m_applicationName == NULL)
        {
            m_applicationName = static_cast<QString*>(IContext::getInstance().singletonPointer("BlackMisc::CLog::m_applicationName"));
            if (m_applicationName == NULL)
            {
                m_applicationName = new QString;
                IContext::getInstance().setSingletonPointer("BlackMisc::CLog::m_applicationName", m_applicationName);
            }
        }

#ifdef Q_OS_WIN
		//! By default, we use the executables name.
        if ((*m_applicationName).isEmpty())
        {
            WCHAR name[1024];
            int size = GetModuleFileName (NULL, name, 1023);
            QString applicationPath = QString::fromWCharArray(name,size);
            (*m_applicationName) = QFileInfo ( applicationPath ).fileName();
        }
#else
		//! Todo: Check if there a corresponding API in Linux and Mac
		//! For the time being, set it to unknown.
        if ((*m_applicationName).isEmpty())
        {
            *m_applicationName = "<Unknown>";
        }
#endif
    }

    void CLog::printWithNewLine( QString &message )
    {
		//! If we have no displays, we have nothing to do.
        if ( hasNoDisplays() )
        {
            return;
        }
		
        QChar newLine = '\n';
		
		//! We should print the message with a new line. So check
		//! if one is already there. If not append it.
        if (!message.endsWith(newLine, Qt::CaseInsensitive))
            message.append(newLine);

        printString (message);
    }

    void CLog::print( QString &message )
    {
		//! If we have no displays, we have nothing to do.
        if ( hasNoDisplays() )
        {
            return;
        }

        printString (message);
    }

    void CLog::printString( QString &message )
    {
        QString logDisplay;

		//! Just in case, lets set the default name
        setDefaultApplicationName ();

		//! If the current line is empty, then put some information as 
		//! the prefix.
		//! Be aware: This information must be set by the \sa setLogInformation()
		//! before.
		if ( m_logLine.isEmpty() )
		{
			m_logInformation.m_dateTime 		= QDateTime::currentDateTime();
			m_logInformation.m_logType 			= m_logType;
			m_logInformation.m_applicationName 	= *m_applicationName;
            m_logInformation.m_threadId 		= 0; //getThreadId();
			m_logInformation.m_sourceFile 		= m_sourceFile;
			m_logInformation.m_line 			= m_line;
			m_logInformation.m_methodName 		= m_methodName;

			m_logLine = message;
		}
		else
		{
			m_logLine += message;
		}
		
		//! If this is not the end of the line, we are done for now.
		if ( ! message.contains('\n') )
		{
			return;
		}

        for ( TLogDisplayList::iterator it = m_logDisplays.begin(); it != m_logDisplays.end(); ++it)
        {
            (*it)->print(m_logInformation, m_logLine);
        }
		
		//! Reset everything for the next line
        m_logLine.clear();
        resetLogInformation();
    }

    void CLog::attachDisplay(ILogDisplay *display)
	{
		//! Display must be a valid pointer
        if (display == NULL)
		{
            printf ("Trying to add a NULL pointer\n");
			return;
		}
		
		//! Check if it is already attached
        if ( !m_logDisplays.contains(display) )
        {
            m_logDisplays.push_back (display);
        }
        else
        {
            bWarning << "Couldn't attach the display - already in the list!";
        }
	}

    ILogDisplay* CLog::getDisplay(const QString &displayName)
	{
		//! Must be a valid name
        if (displayName.isEmpty())
        {
            bWarning << "Cannot return a display with empty name!";
            return NULL;
        }

        TLogDisplayList::const_iterator it;

		//! Loop through the list and find the candidate
        for ( it = m_logDisplays.constBegin(); it != m_logDisplays.constEnd(); ++it)
        {
			//! Does it have the desired name?
            if ( (*it)->DisplayName == displayName )
            {
                return *it;
            }
        }
	}

    void CLog::dettachDisplay (ILogDisplay *logDisplay)
    {
		//! Must be a valid pointer
        if (logDisplay == NULL)
        {
            bWarning << "Cannot remove a NULL displayer!";
            return;
        }
		
		//! We should have only	 one, but just in case.
        m_logDisplays.removeAll(logDisplay);

    }

    void CLog::dettachDisplay (const QString &displayName)
    {
		//! Must be a valid name
        if (displayName.isEmpty())
        {
            bWarning << "Cannot remove displayer with empty name!";
            return;
        }

        TLogDisplayList::iterator it;

        for ( it = m_logDisplays.begin(); it != m_logDisplays.end(); )
        {
            if ( (*it)->DisplayName == displayName )
            {
                it = m_logDisplays.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    bool CLog::isAttached (ILogDisplay *logDisplay) const
    {
        return  m_logDisplays.contains(logDisplay);
    }

    void CLog::setLogInformation (int line, const char *sourceFile, const char *methodName)
    {
		//! We have to make sure, we at least one display.
        if ( !hasNoDisplays() )
        {
            m_mutex.lock();
            m_posSet++;
            m_sourceFile = sourceFile;
            m_line = line;
            m_methodName = methodName;
        }
    }

    void CLog::resetLogInformation()
    {
		//! It should be impossible that this gets called, withoud 
		//! having a attached display.
        bAssert( !hasNoDisplays() );

        if ( m_posSet > 0 )
        {
            m_sourceFile = NULL;
            m_line = -1;
            m_methodName = NULL;
            m_posSet--;
            m_mutex.unlock();
        }
    }

} // namespace BlackMisc
