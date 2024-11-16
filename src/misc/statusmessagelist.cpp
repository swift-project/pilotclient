// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE

#include "misc/statusmessagelist.h"

#include <QJsonValue>
#include <QString>
#include <QStringBuilder>
#include <QStringList>

#include "misc/propertyindexlist.h"

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc, CStatusMessage, CStatusMessageList)

namespace swift::misc
{
    CStatusMessageList::CStatusMessageList(const CSequence<CStatusMessage> &other) : CSequence<CStatusMessage>(other)
    {}

    CStatusMessageList::CStatusMessageList(const CStatusMessage &statusMessage)
    {
        this->push_back(statusMessage);
    }

    CStatusMessageList CStatusMessageList::findByCategory(const CLogCategory &category) const
    {
        return this->findBy([&](const CStatusMessage &msg) { return msg.getCategories().contains(category); });
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
        return this->containsBy([=](const CStatusMessage &m) { return m.getSeverity() == CStatusMessage::SeverityWarning || m.getSeverity() == CStatusMessage::SeverityError; });
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
        return this->findBy([&](const CStatusMessage &msg) {
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

    void CStatusMessageList::addValidationCategory()
    {
        this->addCategory(CLogCategories::validation());
    }

    void CStatusMessageList::addCategories(const CLogCategoryList &categories)
    {
        for (auto &msg : *this)
        {
            msg.addCategories(categories);
        }
    }

    void CStatusMessageList::addValidationMessage(const QString &validationText, CStatusMessage::StatusSeverity severity)
    {
        static const CLogCategoryList cats({ CLogCategories::validation() });
        const CStatusMessage msg(cats, severity, validationText);
        this->push_back(msg);
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

    void CStatusMessageList::warningToError()
    {
        for (CStatusMessage &msg : *this)
        {
            if (msg.getSeverity() != CStatusMessage::SeverityWarning) { continue; }
            msg.setSeverity(CStatusMessage::SeverityError);
        }
    }

    void CStatusMessageList::sortBySeverity()
    {
        this->sortBy(&CStatusMessage::getSeverity);
    }

    void CStatusMessageList::sortBySeverityHighestFirst()
    {
        this->sortBy(&CStatusMessage::getSeverity);
        this->reverse();
    }

    void CStatusMessageList::removeWarningsAndBelow()
    {
        if (this->isEmpty()) { return; }
        this->removeIf(&CStatusMessage::getSeverity, CStatusMessage::SeverityWarning);
        this->removeInfoAndBelow();
    }

    void CStatusMessageList::removeSeverity(CStatusMessage::StatusSeverity severity)
    {
        if (this->isEmpty()) { return; }
        this->removeIf(&CStatusMessage::getSeverity, severity);
    }

    void CStatusMessageList::removeInfoAndBelow()
    {
        if (this->isEmpty()) { return; }
        this->removeIf(&CStatusMessage::getSeverity, CStatusMessage::SeverityDebug);
        this->removeIf(&CStatusMessage::getSeverity, CStatusMessage::SeverityInfo);
    }

    int CStatusMessageList::keepLatest(int estimtatedNumber)
    {
        const int oldSize = this->size();
        if (estimtatedNumber >= oldSize) { return 0; }
        if (estimtatedNumber < 1)
        {
            this->clear();
            return oldSize;
        }

        CStatusMessageList copy(*this);
        copy.sortLatestFirst();
        const QDateTime ts = copy[estimtatedNumber - 1].getUtcTimestamp();
        copy = *this; // keep order
        copy.removeBefore(ts);
        *this = copy;
        return oldSize - this->size();
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

    QMap<int, int> CStatusMessageList::countSeverities() const
    {
        QMap<int, int> counts;
        counts.insert(SeverityDebug, 0);
        counts.insert(SeverityInfo, 0);
        counts.insert(SeverityWarning, 0);
        counts.insert(SeverityError, 0);
        for (const CStatusMessage &m : *this)
        {
            counts[m.getSeverity()]++;
        }
        return counts;
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
            cats.push_back(msg.getCategories());
        }
        const CStatusMessage newMsg(cats, s, newMsgs.join(", "));
        return newMsg;
    }

    QString CStatusMessageList::toHtml(const CPropertyIndexList &indexes) const
    {
        if (indexes.isEmpty() || this->isEmpty()) { return {}; }
        QString html;
        int line = 1;
        const bool withLineNumbers = indexes.contains(CPropertyIndexRef::GlobalIndexLineNumber);
        CPropertyIndexList usedIndexes(indexes);
        if (withLineNumbers) { usedIndexes.remove(CPropertyIndexRef::GlobalIndexLineNumber); }
        for (const CStatusMessage &statusMessage : *this)
        {
            QString rowHtml;
            if (withLineNumbers)
            {
                rowHtml = u"<td>" % QString::number(line++) % u"</td>";
            }

            for (const CPropertyIndex &index : usedIndexes)
            {
                rowHtml += u"<td>" %
                           statusMessage.propertyByIndex(index).toString().toHtmlEscaped().replace('\n', "<br>") %
                           u"</td>";
            }

            const QString severityClass = statusMessage.getSeverityAsString();
            html += QStringLiteral("<tr class=\"%1\">%2</tr>").arg(severityClass, rowHtml);
        }
        return u"<table>" % html % u"</table>";
    }

    const CPropertyIndexList &CStatusMessageList::simpleHtmlOutput()
    {
        static const CPropertyIndexList properties({ CPropertyIndexRef::GlobalIndexLineNumber, CStatusMessage::IndexMessage });
        return properties;
    }

    const CPropertyIndexList &CStatusMessageList::timestampHtmlOutput()
    {
        static const CPropertyIndexList properties({ CStatusMessage::IndexUtcTimestampFormattedHms, CStatusMessage::IndexMessage });
        return properties;
    }

    QString htmlStyleSheetImpl()
    {
        const QString style = u'.' % CStatusMessage::severityToString(CStatusMessage::SeverityDebug) % u" { color: lightgreen; } " %
                              u'.' % CStatusMessage::severityToString(CStatusMessage::SeverityInfo) % u" { color: lightgreen; } " %
                              u'.' % CStatusMessage::severityToString(CStatusMessage::SeverityWarning) % u" { color: yellow; } " %
                              u'.' % CStatusMessage::severityToString(CStatusMessage::SeverityError) % u" { color: red; }";
        return style;
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
} // namespace swift::misc

//! \endcond
