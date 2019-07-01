/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "eventfilter.h"
#include "blackmisc/stringutils.h"

#include <QKeyEvent>
#include <QPlainTextEdit>

namespace BlackGui
{
    bool CUpperCaseEventFilter::eventFilter(QObject *object, QEvent *event)
    {
        if (event->type() == QEvent::KeyPress)
        {
            if (QKeyEvent *e = dynamic_cast<QKeyEvent *>(event))
            {
                // If QKeyEvent::text() returns an empty QString then let normal
                // processing proceed as it may be a control (e.g. cursor movement)
                // key.  Otherwise convert the text to upper case and insert it at
                // the current cursor position.
                QPlainTextEdit *pte = qobject_cast<QPlainTextEdit *>(object);
                if (!pte) { return false; }
                if (pte->isReadOnly()) { return false; }

                if (e->text().length() == 1)
                {
                    const QChar c = e->text().front();
                    if (m_illegalChars.contains(c)) { return true; }
                    if (c.isLetter())
                    {
                        const ushort unicode = c.unicode();
                        if (m_onlyAscii && unicode > 127) { return true; }

                        pte->insertPlainText(e->text().toUpper());
                        // return true to prevent further processing
                        return true;
                    }
                    else
                    {
                        // all codes like backspace etc.
                        return false;
                    }
                } // length
            } // key event
        }
        return false;
    }
} // namespace
