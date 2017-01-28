/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "lineedithistory.h"
#include <QKeyEvent>

namespace BlackGui
{
    QString CLineEditHistory::getLastEnteredLine() const
    {
        if (m_history.isEmpty()) return {};
        return m_history.first();
    }

    QString CLineEditHistory::getLastEnteredLineFormatted() const
    {
        return getLastEnteredLine().trimmed().simplified();
    }

    void CLineEditHistory::keyPressEvent(QKeyEvent *event)
    {
        if (event->key() == Qt::Key_Up)
        {
            // move back in history
            if (m_history.size() > m_position)
            {
                setText(m_history.at(m_position++));
            }
        }
        else if (event->key() == Qt::Key_Down)
        {
            // move forward in history
            if (m_position <= 0) { clear(); return; }
            if (m_position == m_history.size()) { --m_position; } // avoid need of 2xKeyDown at end
            if (m_position > 0 && m_history.size() > --m_position)
            {
                setText(m_history.at(m_position));
            }
        }
        else if (event->key() == Qt::Key_Return)
        {
            if (!text().isEmpty())
            {
                m_history.push_front(text());
                m_position = 0;
                clear();
            }
        }
        // default handler for event
        QLineEdit::keyPressEvent(event);
    }
} // ns
