// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
