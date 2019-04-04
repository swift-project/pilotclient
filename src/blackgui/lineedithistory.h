/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_LINEEDITHISTORY_H
#define BLACKGUI_LINEEDITHISTORY_H

#include "blackgui/blackguiexport.h"
#include <QLineEdit>
#include <QStringList>

namespace BlackGui
{
    /*!
     * Line edit with history
     * \details lines are trimmed and empty lines are ignored
     */
    class BLACKGUI_EXPORT CLineEditHistory : public QLineEdit
    {
        Q_OBJECT

    public:
        //! Constructors
        using QLineEdit::QLineEdit;

        //! Get the last entered line
        QString getLastEnteredLine() const;

        //! Get the last entered line but simplified and trimmed
        QString getLastEnteredLineFormatted() const;

        //! Clear the history
        void clearHistory();

    signals:
        //! Return has been pressed, line is NOT empty (spaces are trimmed)
        //! \details returnPressed alsofires on empty lines, but those are not in the history
        void returnPressedUnemptyLine();

    protected:
        //! \copydoc QLineEdit::keyPressEvent
        virtual void keyPressEvent(QKeyEvent *event) override;

        //! \copydoc QLineEdit::keyPressEvent
        virtual void contextMenuEvent(QContextMenuEvent *event) override;

    private:
        QStringList m_history;
        int m_position = 0;
    };
} // ns

#endif // guard
