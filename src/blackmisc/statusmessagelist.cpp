/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "blackmisc/compare.h"
#include "blackmisc/range.h"
#include "blackmisc/statusmessagelist.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QStringList>
#include <tuple>

namespace BlackMisc
{
    CStatusMessageList::CStatusMessageList(const CSequence<CStatusMessage> &other) :
        CSequence<CStatusMessage>(other)
    { }

    CStatusMessageList::CStatusMessageList(const CStatusMessage &statusMessage)
    {
        this->push_back(statusMessage);
    }

    CStatusMessageList CStatusMessageList::findByCategory(const CLogCategory &category) const
    {
        return this->findBy([ & ](const CStatusMessage & msg) { return msg.getCategories().contains(category); });
    }

    CStatusMessageList CStatusMessageList::findBySeverity(CStatusMessage::StatusSeverity severity) const
    {
        return this->findBy(&CStatusMessage::getSeverity, severity);
    }

    bool CStatusMessageList::hasErrorMessages() const
    {
        return this->contains(&CStatusMessage::getSeverity, CStatusMessage::SeverityError);
    }

    bool CStatusMessageList::hasWarningMessages() const
    {
        return this->contains(&CStatusMessage::getSeverity, CStatusMessage::SeverityWarning);
    }

    bool CStatusMessageList::hasWarningOrErrorMessages() const
    {
        return this->containsBy
        ([ = ](const CStatusMessage & m) { return m.getSeverity() == CStatusMessage::SeverityWarning || m.getSeverity() == CStatusMessage::SeverityError; });
    }

    bool CStatusMessageList::isSuccess() const
    {
        return !this->isFailure();
    }

    bool CStatusMessageList::isFailure() const
    {
        return this->contains(&CStatusMessage::isFailure, true);
    }

    CStatusMessageList CStatusMessageList::getErrorMessages() const
    {
        return findBySeverity(SeverityError);
    }

    CStatusMessageList CStatusMessageList::getWarningAndErrorMessages() const
    {
        return this->findBy([ & ](const CStatusMessage & msg)
        {
            return msg.getSeverity() >= CStatusMessage::SeverityWarning;
        });
    }

    void CStatusMessageList::addCategory(const CLogCategory &category)
    {
        for (auto &msg : *this)
        {
            msg.addCategory(category);
        }
    }

    void CStatusMessageList::addCategories(const CLogCategoryList &categories)
    {
        for (auto &msg : *this)
        {
            msg.addCategories(categories);
        }
    }

    void CStatusMessageList::setCategory(const CLogCategory &category)
    {
        for (auto &msg : *this)
        {
            msg.setCategory(category);
        }
    }

    void CStatusMessageList::setCategories(const CLogCategoryList &categories)
    {
        for (auto &msg : *this)
        {
            msg.setCategories(categories);
        }
    }

    void CStatusMessageList::clampSeverity(CStatusMessage::StatusSeverity severity)
    {
        for (auto &msg : *this)
        {
            msg.clampSeverity(severity);
        }
    }

    void CStatusMessageList::removeWarningsAndBelow()
    {
        if (this->isEmpty()) { return; }
        this->removeIf(&CStatusMessage::getSeverity, CStatusMessage::SeverityWarning);
        this->removeInfoAndBelow();
    }

    void CStatusMessageList::removeInfoAndBelow()
    {
        if (this->isEmpty()) { return; }
        this->removeIf(&CStatusMessage::getSeverity, CStatusMessage::SeverityDebug);
        this->removeIf(&CStatusMessage::getSeverity, CStatusMessage::SeverityInfo);
    }

    CStatusMessage::StatusSeverity CStatusMessageList::worstSeverity() const
    {
        CStatusMessage::StatusSeverity s = CStatusMessage::SeverityDebug;
        for (const CStatusMessage &msg : *this)
        {
            CStatusMessage::StatusSeverity ms = msg.getSeverity();
            if (ms == CStatusMessage::SeverityError) { return CStatusMessage::SeverityError; }
            if (ms <= s) { continue; }
            s = ms;
        }
        return s;
    }

    CStatusMessage CStatusMessageList::toSingleMessage() const
    {
        if (this->isEmpty()) { return CStatusMessage(); }
        if (this->size() == 1) { return this->front(); }
        QStringList newMsgs;
        CStatusMessage::StatusSeverity s = CStatusMessage::SeverityDebug;
        CLogCategoryList cats;
        for (const CStatusMessage &msg : *this)
        {
            if (msg.isEmpty()) { continue; }
            newMsgs.append(msg.getMessage());
            CStatusMessage::StatusSeverity ms = msg.getSeverity();
            if (s < ms) { s = ms; }
            cats = cats.join(msg.getCategories());
        }
        const CStatusMessage newMsg(cats, s, newMsgs.join(", "));
        return newMsg;
    }

    QString CStatusMessageList::toHtml(const CPropertyIndexList &indexes) const
    {
        if (indexes.isEmpty() || this->isEmpty()) { return ""; }
        QString html;
        int line = 1;
        const bool withLineNumbers = indexes.contains(CPropertyIndex::GlobalIndexLineNumber);
        CPropertyIndexList usedIndexes(indexes);
        if (withLineNumbers) { usedIndexes.remove(CPropertyIndex::GlobalIndexLineNumber); }
        for (const CStatusMessage &statusMessage : *this)
        {
            QString rowHtml;
            if (withLineNumbers)
            {
                rowHtml += "<td>" + QString::number(line++) + "</td>";
            }

            for (const CPropertyIndex &index : usedIndexes)
            {
                rowHtml += "<td>" + statusMessage.propertyByIndex(index).toQString(true) + "</td>";
            }

            rowHtml = "<tr class=\"%1\">" + rowHtml + "</tr>";
            const QString severityClass = statusMessage.getSeverityAsString();
            html += rowHtml.arg(severityClass);
        }
        return "<table>" + html + "</table>";
    }

    const QString htmlStyleSheetImpl()
    {
        QString style;
        style += "." + CStatusMessage::severityToString(CStatusMessage::SeverityDebug) + " { color: lightgreen; } ";
        style += "." + CStatusMessage::severityToString(CStatusMessage::SeverityInfo) + " { color: lightgreen; } ";
        style += "." + CStatusMessage::severityToString(CStatusMessage::SeverityWarning) + " { color: yellow; } ";
        style += "." + CStatusMessage::severityToString(CStatusMessage::SeverityError) + " { color: red; }";
        return style; // "<style type=\"text/css\">" + style + "</style>";
    }

    const QString &CStatusMessageList::htmlStyleSheet()
    {
        static const QString style(htmlStyleSheetImpl());
        return style;
    }

    CStatusMessageList CStatusMessageList::fromDatabaseJson(const QJsonArray &array)
    {
        CStatusMessageList messages;
        for (const QJsonValue &value : array)
        {
            messages.push_back(CStatusMessage::fromDatabaseJson(value.toObject()));
        }
        return messages;
    }
} // ns

//! \endcond
