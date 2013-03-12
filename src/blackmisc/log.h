//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef LOG_H
#define LOG_H

#include <QMutex>
#include <QList>
#include <QString>
#include <QDateTime>

namespace BlackMisc {

    class ILogDisplay;

	//! Logging class
	/*! This class implements the logging of the library. It holds a list of displays 
		takes care of any additional information needed, e.g. timestamps, filename etc.
		To use the logging use the default displayer or implement your own one and
		register it with this class.
	*/
	class CLog
	{
	public:
        //! Logging type.
        /*! This enum holds all different available log types. */
        typedef enum { OFF = 0,
                       ERROR,
                       WARNING,
                       INFO,
                       DEBUG,
                       ASSERT,
                       UNKNOWN } TLogType;

        //! SLogParameter
        /*!
          This structs capsulates all information needed to output a log line.
        */
        struct SLogInformation
        {
            SLogInformation() : m_logType(CLog::OFF), m_threadId(0), m_line(-1), m_sourceFile(NULL), m_methodName(NULL) {}

            QDateTime       	m_dateTime;
            TLogType			m_logType;
            QString 			m_applicationName;
            quint32				m_threadId;
            const char 		   *m_sourceFile;
            qint32          	m_line;
            const char         *m_methodName;
        };

        CLog (TLogType logType = OFF);

        //! CLog destructor.
		virtual ~CLog(void);

        /*!
         * Display
         */

		//! This method adds a Displayer to the Log object
		/*! CLog does not own the pointer. It is the callers responsibility
			to allocate the displayer and remove it after using. */
		/*!
		  \param display A pointer to a display.
		  \sa ILogDisplay
		*/
        void attachDisplay (BlackMisc::ILogDisplay *display);

		//! This method returns the pointer to a registered display.
		/*!
		  \return display A pointer to a display.
		*/
        ILogDisplay *getDisplay (const QString &displayName);

		//! Removes a display
		/*!
		  \param logDisplay Pointer to the display.
		*/
        void dettachDisplay (ILogDisplay *logDisplay);

		//! Removes a display by its name.
		/*!
		  \param displayName Name of the display.
		*/
        void dettachDisplay (const QString& displayName);

		//! Checks if the displayer is added
		/*!
		  \param logDisplay Pointer to a display.
		  \return Returns true if display is attached, otherwise false.
		*/
        bool isAttached (ILogDisplay *logDisplay) const;

		//! Checks if the object has any attached displays
		/*!
		  \return Returns true if no display is attached, otherwise false.
		*/
        bool hasNoDisplays () const { return m_logDisplays.empty(); }

        //! Sets the name of the application process
		/*!
		  \param displayName Name of the application.
		*/
        static void setApplicationName (const QString &displayName);

        //! Sets the default application name. This is the name
		//! of the executable.
        static void setDefaultApplicationName ();

        //! Sets any additional information as prefix to the log message
		/*!
		  \param fileName This is the name of the corresponding source file.
		  \param methodName This is the name of calling method.
		*/
        void setLogInformation (int line, const char *fileName, const char *methodName = NULL);

		//! Prints the message and adds an new line character at the end
		/*!
		  \param message Message string, which has to be logged
		*/
        void printWithNewLine( QString &message );

		//! Prints the message as is and appends no additional characters
		/*!
		  \param message Message string, which has to be logged
		*/
        void print( QString &message );

        void printString ( QString &message );

	protected:
	
		/// Display a string in decorated form to all attached displayers.
        void displayString (const char *str);

        /// Symetric to setPosition(). Automatically called by display...(). Do not call if noDisplayer().
        void resetLogInformation();

		//! Logging Type.
		/*!
		  Specifies which logging type should be used.
		  Possible are: Error, Warning, Info, Debug and Assert.
		*/
        TLogType							  m_logType;
		
		//! Application name.
		/*!
		  Specifies the name of the application to put it into the log line
		*/
        static QString   					 *m_applicationName;

		//! Source file name.
		/*!
		  Specifies the name of the source file, log message was called from.
		*/
        const char 							 *m_sourceFile;
		
		//! Code line.
		/*!
		  Specifies the line in the source file, log message was called from.
		*/
        qint32                                m_line;
		
		//! Method name.
		/*!
		  Specifies the method in the source file, log message was called from.
		*/
        const char						     *m_methodName;
		
		/*!
		  \typedef TLogDisplayerMap
		  QList container holding pointers to ILogDisplay objects
		  \sa ArrayList::GetEnumerator, \sa List::GetEnumerator
		*/
        typedef QList<ILogDisplay *>		  TLogDisplayList;
		
		//! List of all attached displays.
        TLogDisplayList                       m_logDisplays;
		
		//! Mutex object
		/*!
		  This makes our class thread safe.
		*/
        QMutex                                m_mutex;

		//! Position
        quint32								  m_posSet;

		//! Log message string
		/*!
		  This variable is used, if a log line consists of two or more
		  calls. The first ones are then stored temporary in this variable
		  until one message ends with a line feed.
		*/
        QString                              m_logLine;
        SLogInformation						 m_logInformation;
	};

} // namespace BlackMisc

#endif LOG_H
