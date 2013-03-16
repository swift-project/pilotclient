//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef QT_DISPLAYER_H
#define QT_DISPLAYER_H

#include "blackmisc/display.h"

// Qt includes
#include <QPlainTextEdit>

class CQtDisplayer : virtual public BlackMisc::ILogDisplay
{
public:
    CQtDisplayer(QPlainTextEdit *debugWindow,
                 const char *displayerName = "");
    CQtDisplayer();
    ~CQtDisplayer ();
    void setParam (QPlainTextEdit *debugWindow);

protected:
    virtual void doPrint(const BlackMisc::CLog::SLogInformation &logInformation, const QString &message);

private:
    QPlainTextEdit  *m_DebugWindow;
    bool            m_needHeader;
};

#endif // QT_DISPLAYER_H
