// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/textmessagetextedit.h"

#include <QAction>
#include <QMenu>
#include <QScopedPointer>
#include <QStringBuilder>
#include <QTextDocument>
#include <QTextOption>
#include <Qt>
#include <QtGlobal>

#include "gui/guiapplication.h"
#include "misc/aviation/callsign.h"
#include "misc/network/textmessage.h"

using namespace swift::misc;
using namespace swift::misc::network;

namespace swift::gui
{
    CTextMessageTextEdit::CTextMessageTextEdit(QWidget *parent) : QTextEdit(parent)
    {
        this->setDocument(&m_textDocument);
        this->setReadOnly(true);
        this->setWordWrap(true);

        // menu
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        m_actionClearTextEdit = new QAction(CIcon::iconByIndex(CIcons::StandardIconDelete16).toQIcon(), "Clear", this);
        m_actionLast10 = new QAction("Keep last 10 messages", this);
        m_actionLast25 = new QAction("Keep last 25 messages", this);
        m_actionAll = new QAction("Keep all messages", this);
        m_actionWithRecipient = new QAction("With recipient", this);
        m_actionWithSender = new QAction("With sender", this);
        m_actionWordWrap = new QAction("Word wrap", this);
        m_actionWithRecipient->setCheckable(true);
        m_actionWithSender->setCheckable(true);
        m_actionWordWrap->setCheckable(true);

        connect(m_actionClearTextEdit, &QAction::triggered, this, &CTextMessageTextEdit::clear);
        connect(m_actionAll, &QAction::triggered, this, &CTextMessageTextEdit::keepLastNMessages);
        connect(m_actionLast10, &QAction::triggered, this, &CTextMessageTextEdit::keepLastNMessages);
        connect(m_actionLast25, &QAction::triggered, this, &CTextMessageTextEdit::keepLastNMessages);
        connect(m_actionWithSender, &QAction::triggered, this, &CTextMessageTextEdit::setVisibleFields);
        connect(m_actionWithRecipient, &QAction::triggered, this, &CTextMessageTextEdit::setVisibleFields);
        connect(m_actionWordWrap, &QAction::triggered, this, &CTextMessageTextEdit::setWordWrap);

        connect(this, &QTextEdit::customContextMenuRequested, this, &CTextMessageTextEdit::showContextMenuForTextEdit);
    }

    CTextMessageTextEdit::~CTextMessageTextEdit()
    {}

    void CTextMessageTextEdit::insertTextMessage(const CTextMessage &textMessage, int maxMessages)
    {
        if (maxMessages < 0 && m_keepMaxMessages >= 0) { maxMessages = m_keepMaxMessages; }
        if (maxMessages >= 0)
        {
            m_messages.push_backMaxElements(textMessage, maxMessages);
        }
        else
        {
            m_messages.push_back(textMessage);
        }
        this->redrawHtml();
    }

    int CTextMessageTextEdit::count() const
    {
        return m_messages.size();
    }

    void CTextMessageTextEdit::clear()
    {
        m_messages.clear();
        QTextEdit::clear();
    }

    void CTextMessageTextEdit::redrawHtml()
    {
        const QString html(
            this->toHtml(
                m_latestFirst ? m_messages.reversed() : m_messages,
                m_withSender,
                m_withRecipient));
        m_textDocument.setHtml(html);
        this->moveCursor(m_latestFirst ? QTextCursor::Start : QTextCursor::End);
    }

    void CTextMessageTextEdit::setStyleSheetForContent(const QString &styleSheet)
    {
        m_textDocument.setDefaultStyleSheet(styleSheet);
        this->redrawHtml();
    }

    QString CTextMessageTextEdit::toHtml(const CTextMessageList &messages, bool withFrom, bool withTo)
    {
        if (messages.isEmpty()) { return {}; }
        QString html("<table>");
        for (const CTextMessage &msg : messages)
        {
            html += CTextMessageTextEdit::toHtml(msg, withFrom, withTo);
        }
        html += "</table>";
        return html;
    }

    QString CTextMessageTextEdit::toHtml(const CTextMessage &message, bool withFrom, bool withTo)
    {
        QString rowClass(message.wasSent() ? "sent" : "received");
        if (message.isSupervisorMessage()) { rowClass += " supervisor"; }

        QString html(u"<tr class=\"" % rowClass.trimmed() %
                     u"\"><td class=\"timestamp\">" % message.getFormattedUtcTimestampHms() % u"</td>");

        if (withFrom)
        {
            html += u"<td class=\"sender\">" % message.getSenderCallsign().asString() % u"</td>";
        }

        if (withTo)
        {
            html += u"<td class=\"recipient\">" % message.getRecipientCallsignOrFrequency() % u"</td>";
        }

        html += u"<td class=\"message\">" % message.getHtmlEncodedMessage() % u"</td></tr>";
        return html;
    }

    void CTextMessageTextEdit::showContextMenuForTextEdit(const QPoint &pt)
    {
        m_actionWithRecipient->setChecked(m_withRecipient);
        m_actionWithSender->setChecked(m_withSender);
        m_actionWordWrap->setChecked(m_wordWrap);

        QScopedPointer<QMenu> menu(this->createStandardContextMenu());
        menu->setObjectName(this->objectName().append("_contextMenu"));
        menu->addSeparator();
        menu->addAction(m_actionWordWrap);
        QMenu *subMenu = menu->addMenu("Max.messages");
        subMenu->addAction(m_actionLast10);
        subMenu->addAction(m_actionLast25);
        subMenu->addAction(m_actionAll);
        subMenu = menu->addMenu("Fields");
        subMenu->addAction(m_actionWithSender);
        subMenu->addAction(m_actionWithRecipient);
        menu->addAction(m_actionClearTextEdit);
        menu->exec(this->mapToGlobal(pt));
    }

    void CTextMessageTextEdit::keepLastNMessages()
    {
        QObject *sender = QObject::sender();
        if (sender == m_actionAll)
        {
            m_keepMaxMessages = -1;
        }
        else if (sender == m_actionLast10)
        {
            m_keepMaxMessages = 10;
        }
        else if (sender == m_actionLast25)
        {
            m_keepMaxMessages = 25;
        }
    }

    void CTextMessageTextEdit::setVisibleFields()
    {
        QObject *sender = QObject::sender();
        if (sender == m_actionWithRecipient)
        {
            m_withRecipient = m_actionWithRecipient->isChecked();
        }
        else if (sender == m_actionWithSender)
        {
            m_withSender = m_actionWithSender->isChecked();
        }
    }

    void CTextMessageTextEdit::setWordWrap(bool wordWrap)
    {
        m_wordWrap = wordWrap;
        if (m_wordWrap)
        {
            this->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        }
        else
        {
            this->setWordWrapMode(QTextOption::NoWrap);
        }
    }
} // namespace swift::gui
