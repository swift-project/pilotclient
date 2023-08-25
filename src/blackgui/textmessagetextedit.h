// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_TEXTMESSAGETEXTEDIT_H
#define BLACKGUI_TEXTMESSAGETEXTEDIT_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/network/textmessagelist.h"

#include <QObject>
#include <QString>
#include <QTextEdit>
#include <QTextDocument>

class QAction;
class QPoint;

namespace BlackMisc::Network
{
    class CTextMessage;
}
namespace BlackGui
{
    //! Specialized text edit for displaying text messages
    class BLACKGUI_EXPORT CTextMessageTextEdit : public QTextEdit
    {
        Q_OBJECT

    public:
        //! Constructor
        CTextMessageTextEdit(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CTextMessageTextEdit() override;

        //! Insert a message
        void insertTextMessage(const BlackMisc::Network::CTextMessage &textMessage, int maxMessages = -1);

        //! The displayed text messaged
        const BlackMisc::Network::CTextMessageList &getDisplayedTextMessages() const;

        //! Number of displayed messages
        int count() const;

        //! Stylesheet for content
        void setStyleSheetForContent(const QString &styleSheet);

        //! Clear
        void clear();

        //! Redraw HTML
        void redrawHtml();

        //! Order latest first/latest last
        void setLatestFirst(bool latestFirst) { m_latestFirst = latestFirst; }

        //! Lastest first
        bool isLatestFirst() const { return m_latestFirst; }

    private:
        //! Context menu
        void showContextMenuForTextEdit(const QPoint &pt);

        //! Keep last n messages
        void keepLastNMessages();

        //! Visible fields
        void setVisibleFields();

        //! Convert to HTML
        static QString toHtml(const BlackMisc::Network::CTextMessageList &messages, bool withFrom, bool withTo);

        //! Convert to HTML
        static QString toHtml(const BlackMisc::Network::CTextMessage &message, bool withFrom, bool withTo);

        //! Word wrap
        void setWordWrap(bool wordWrap);

        BlackMisc::Network::CTextMessageList m_messages;
        QTextDocument m_textDocument;
        int m_keepMaxMessages = -1; //!< max number of messages to keep, or -1 to keep all messages
        bool m_latestFirst = false;
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
