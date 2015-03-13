/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "textmessagetextedit.h"
#include "blackmisc/iconlist.h"
#include "stylesheetutility.h"
#include <QMenu>

using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Network;

namespace BlackGui
{

    CTextMessageTextEdit::CTextMessageTextEdit(QWidget *parent) : QTextEdit(parent)
    {
        this->m_textDocument = new QTextDocument(this);
        this->setDocument(m_textDocument);
        this->setReadOnly(true);
        this->setWordWrapMode(QTextOption::NoWrap);

        // menu
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        m_actionClearTextEdit = new QAction(CIconList::iconByIndex(CIcons::StandardIconDelete16).toQIcon(), "Clear", this);
        m_actionLast10 = new QAction("Keep last 10 messages", this);
        m_actionLast25 = new QAction("Keep last 25 messages", this);
        m_actionAll = new QAction("Keep all messages", this);
        m_actionWithRecipient = new QAction("With recipient", this);
        m_actionWithSender = new QAction("With sender", this);
        m_actionWithRecipient->setCheckable(true);
        m_actionWithSender->setCheckable(true);
        m_actionWithRecipient->setChecked(this->m_withRecipient);
        m_actionWithSender->setCheckable(true);
        m_actionWithSender->setChecked(this->m_withSender);

        connect(this->m_actionClearTextEdit, &QAction::triggered, this, &CTextMessageTextEdit::clear);
        connect(this->m_actionAll, &QAction::triggered, this, &CTextMessageTextEdit::ps_keepLastNMessages);
        connect(this->m_actionLast10, &QAction::triggered, this, &CTextMessageTextEdit::ps_keepLastNMessages);
        connect(this->m_actionLast25, &QAction::triggered, this, &CTextMessageTextEdit::ps_keepLastNMessages);
        connect(this->m_actionWithSender, &QAction::triggered, this, &CTextMessageTextEdit::ps_setVisibleFields);
        connect(this->m_actionWithRecipient, &QAction::triggered, this, &CTextMessageTextEdit::ps_setVisibleFields);

        connect(this, &QTextEdit::customContextMenuRequested, this, &CTextMessageTextEdit::ps_showContextMenuForTextEdit);

        // style sheet
        connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &CTextMessageTextEdit::ps_onStyleSheetChanged);
        ps_onStyleSheetChanged();
    }

    CTextMessageTextEdit::~CTextMessageTextEdit()
    { }

    void CTextMessageTextEdit::insertTextMessage(const BlackMisc::Network::CTextMessage &textMessage, int maxMessages)
    {
        if (maxMessages < 0 && m_keepMaxMessages >= 0) { maxMessages = m_keepMaxMessages; }
        if (maxMessages >= 0)
        {
            this->m_messages.push_frontMaxElements(textMessage, maxMessages);
        }
        else
        {
            this->m_messages.push_front(textMessage);
        }
        QString html(toHtml(this->m_messages, this->m_withSender, this->m_withRecipient));
        m_textDocument->setHtml(html);
    }

    int CTextMessageTextEdit::count() const
    {
        return m_messages.size();
    }

    void CTextMessageTextEdit::clear()
    {
        this->m_messages.clear();
        QTextEdit::clear();
    }

    void CTextMessageTextEdit::setStyleSheetForContent(const QString &styleSheet)
    {
        this->m_textDocument->setDefaultStyleSheet(styleSheet);
    }

    QString CTextMessageTextEdit::toHtml(const CTextMessageList &messages, bool withFrom, bool withTo)
    {
        if (messages.isEmpty()) { return ""; }
        QString html("<table>");
        for (CTextMessage msg : messages)
        {
            html += toHtml(msg, withFrom, withTo);
        }
        html += "</table>";
        return html;
    }

    QString CTextMessageTextEdit::toHtml(const CTextMessage &message, bool withFrom, bool withTo)
    {
        QString html;
        QString rowClass;
        if (message.isSupervisorMessage()) { rowClass += " supervisor"; }
        if (message.wasSent()) { rowClass += " sent"; }
        else { rowClass += " received"; }

        if (rowClass.isEmpty())
        {
            html += "<tr>";
        }
        else
        {
            html += "<tr class=\"";
            html += rowClass.trimmed();
            html += "\">";
        }
        html += "<td class=\"timestamp\">";
        html += message.getFormattedUtcTimestampHms();
        html += "</td>";

        if (withFrom)
        {
            html += "<td class=\"sender\">";
            html += message.getSenderCallsign().asString();
            html += "</td>";
        }

        if (withTo)
        {
            html += "<td class=\"recipient\">";
            html += message.getRecipientCallsignOrFrequency();
            html += "</td>";
        }

        html += "<td class=\"message\">";
        html += message.getMessage();
        html += "</td>";

        html += "</tr>";
        return html;
    }

    void CTextMessageTextEdit::ps_showContextMenuForTextEdit(const QPoint &pt)
    {
        QScopedPointer<QMenu> menu(this->createStandardContextMenu());
        menu->setObjectName(this->objectName().append("_contextMenu"));
        menu->addSeparator();
        QMenu *subMenu = menu->addMenu("Max.messages");
        subMenu->addAction(this->m_actionLast10);
        subMenu->addAction(this->m_actionLast25);
        subMenu->addAction(this->m_actionAll);
        subMenu = menu->addMenu("Fields");
        subMenu->addAction(this->m_actionWithSender);
        subMenu->addAction(this->m_actionWithRecipient);
        menu->addAction(this->m_actionClearTextEdit);
        menu->exec(this->mapToGlobal(pt));
    }

    void CTextMessageTextEdit::ps_onStyleSheetChanged()
    {
        Q_ASSERT(this->m_textDocument);
        QString style(CStyleSheetUtility::instance().style(CStyleSheetUtility::fileNameTextMessage()));
        this->m_textDocument->setDefaultStyleSheet(style);
    }

    void CTextMessageTextEdit::ps_keepLastNMessages()
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

    void CTextMessageTextEdit::ps_setVisibleFields()
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

} // namespace
