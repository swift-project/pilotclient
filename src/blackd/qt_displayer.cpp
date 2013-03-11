//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "qt_displayer.h"

using namespace BlackMisc;

CQtDisplayer::CQtDisplayer(QPlainTextEdit *debugWindow, const char *displayerName)
    :	ILogDisplay (displayerName), m_needHeader(true)
{
    setParam(debugWindow);
}

CQtDisplayer::CQtDisplayer()
    : ILogDisplay (""), m_needHeader(true)
{
    
}

CQtDisplayer::~CQtDisplayer() {

}

void CQtDisplayer::setParam (QPlainTextEdit *debugWindow)
{
    m_DebugWindow=debugWindow;
}

void CQtDisplayer::doPrint ( const BlackMisc::CLog::SLogInformation &logInformation, const QString &message)
{
	if (!m_DebugWindow)
		return;

    bool needSpace = false;
    QString line;

    if(m_DebugWindow==NULL)
        return;

    QTextCharFormat format;

    if (logInformation.m_dateTime.isValid()) {
        line += dateToString(logInformation.m_dateTime);
        needSpace = true;
    }

    if (logInformation.m_logType != CLog::OFF)
    {
        if (needSpace) { line += " "; needSpace = false; }
        line += logTypeToString(logInformation.m_logType);
        if (logInformation.m_logType == BlackMisc::CLog::WARNING)
            format.setForeground(QBrush(QColor("red")));
        else
            format.setForeground(QBrush(QColor("black")));
        needSpace = true;
    }

    if (logInformation.m_methodName != NULL)
    {
        if (needSpace)
        {
            line += " "; needSpace = false;
        }
        line += logInformation.m_methodName;
        needSpace = true;
    }

    if (needSpace)
    {
        line += " : "; needSpace = false;
    }
    line += message;

    m_DebugWindow->textCursor().insertText(line, format);
    m_DebugWindow->centerCursor();
}
