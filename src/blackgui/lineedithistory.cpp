/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "lineedithistory.h"
#include <QKeyEvent>
#include <QMenu>

namespace BlackGui
{
    QString CLineEditHistory::getLastEnteredLine() const
    {
        if (m_history.isEmpty()) return {};
        return m_history.first();
    }

    QString CLineEditHistory::getLastEnteredLineFormatted() const
    {
        return this->getLastEnteredLine().trimmed().simplified();
    }

    void CLineEditHistory::clearHistory()
    {
        m_history.clear();
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
            if (m_position <= 0) { this->clear(); return; }
            if (m_position == m_history.size()) { --m_position; } // avoid need of 2xKeyDown at end
            if (m_position > 0 && m_history.size() > --m_position)
            {
                this->setText(m_history.at(m_position));
            }
        }
        else if (event->key() == Qt::Key_Return)
        {
            if (!text().isEmpty())
            {
                m_history.push_front(text());
                m_position = 0;
                this->clear();
            }
        }
        // default handler for event
        QLineEdit::keyPressEvent(event);
    }

    void CLineEditHistory::contextMenuEvent(QContextMenuEvent *event)
    {
        if (!event) { return; }
        QMenu *menu = this->createStandardContextMenu();
        menu->addSeparator();
        menu->addAction("Clear history");
        connect(menu->actions().last(), &QAction::triggered, this, &CLineEditHistory::clearHistory);
        menu->exec(event->globalPos());
        delete menu;
    }
} // ns
