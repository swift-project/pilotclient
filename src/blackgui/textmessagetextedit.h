/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_TEXTMESSAGETEXTEDIT_H
#define BLACKGUI_TEXTMESSAGETEXTEDIT_H

#include "blackmisc/nwtextmessagelist.h"
#include <QTextEdit>
#include <QAction>

namespace BlackGui
{
    //! Specialized text edit for displaying text messages
    class CTextMessageTextEdit : public QTextEdit
    {
    public:
        //! Constructor
        CTextMessageTextEdit(QWidget *parent = nullptr);

        //! Destructor
        ~CTextMessageTextEdit();

        //! Insert a message
        void insertTextMessage(const BlackMisc::Network::CTextMessage &textMessage, int maxMessages = -1);

        //! The displayed text messaged
        const BlackMisc::Network::CTextMessageList &getDisplayedTextMessages() const;

        //! Number of displayed messages
        int count() const;

    public slots:
        //! Clear
        void clear();

    private slots:
        //! Context menu
        void ps_showContextMenuForTextEdit(const QPoint &pt);

        //! Stylesheet has been changed
        void ps_onStyleSheetChanged();

        //! Keep last n messages
        void ps_keepLastNMessages();

        //! Visible fields
        void ps_setVisibleFields();

        //! Toggle word wrap
        void ps_setWordWrap();

    private:
        //! Convert to HTML
        static QString toHtml(const BlackMisc::Network::CTextMessageList &messages, bool withFrom, bool withTo);

        //! Convert to HTML
        static QString toHtml(const BlackMisc::Network::CTextMessage &message, bool withFrom, bool withTo);

        //! Stylesheet for content
        void setStyleSheetForContent(const QString &styleSheet);

        //! Word wrap
        void setWordWrap(bool wordWrap);

        BlackMisc::Network::CTextMessageList m_messages;
        QTextDocument *m_textDocument = nullptr;
        int m_keepMaxMessages = 10;
        bool m_withSender = true;
        bool m_withRecipient = false;
        bool m_wordWrap = true;

        QAction *m_actionClearTextEdit = nullptr;
        QAction *m_actionLast10 = nullptr;
        QAction *m_actionLast25 = nullptr;
        QAction *m_actionAll = nullptr;
        QAction *m_actionWithSender = nullptr;
        QAction *m_actionWithRecipient = nullptr;
        QAction *m_actionWordWrap = nullptr;

    };

} // namespace

#endif // guard
