// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/statusmessagefilter.h"
#include "blackmisc/logpattern.h"

using namespace BlackMisc;

namespace BlackGui::Models
{
    CStatusMessageFilter::CStatusMessageFilter(CStatusMessage::StatusSeverity severity, const QString &text, const QString &category) : m_severity(severity), m_msgText(text.trimmed()), m_category(category.trimmed())
    {
        // info or debug are treated like no filter
        this->m_valid = !((severity == CStatusMessage::SeverityInfo || severity == CStatusMessage::SeverityDebug) && m_msgText.isEmpty() && m_category.isEmpty());
    }

    CStatusMessageList CStatusMessageFilter::filter(const CStatusMessageList &inContainer) const
    {
        if (!this->isValid()) { return inContainer; }
        CStatusMessageList outContainer;
        for (const CStatusMessage &msg : inContainer)
        {
            if (!(m_severity == CStatusMessage::SeverityInfo || m_severity == CStatusMessage::SeverityDebug))
            {
                if (!msg.isSeverityHigherOrEqual(this->m_severity)) { continue; }
            }

            if (!this->m_msgText.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(msg.getMessage(), this->m_msgText)) { continue; }
            }

            if (!this->m_category.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(CLogPattern::humanOrTechnicalCategoriesFrom(msg).join(", "), this->m_category)) { continue; }
            }

            outContainer.push_back(msg);
        }
        return outContainer;
    }

    CVariant CStatusMessageFilter::getAsValueObject() const
    {
        if (!m_category.contains('*'))
        {
            if (CLogPattern::allHumanReadableNames().contains(m_category))
            {
                return CVariant::from(CLogPattern::fromHumanReadableName(m_category).withSeverityAtOrAbove(m_severity));
            }
            return CVariant::from(CLogPattern::exactMatch(m_category).withSeverityAtOrAbove(m_severity));
        }

        CLogCategoryList categories = CLogCategoryList::fromQStringList(CLogCategories::allSpecialCategories());
        categories.removeIf([this](const CLogCategory &cat) { return this->stringMatchesFilterExpression(cat.toQString(), this->m_category); });
        CSequence<QString> humanNames = CLogPattern::allHumanReadableNames();
        humanNames.removeIf([this](const QString &name) { return this->stringMatchesFilterExpression(name, this->m_category); });
        auto humanCats = humanNames.transform([](const QString &name) {
            const auto strings = CLogPattern::fromHumanReadableName(name).getCategoryStrings();
            return strings.isEmpty() ? QString {} : *strings.begin();
        });

        return CVariant::from(CLogPattern::anyOf(categories.join(humanCats)).withSeverityAtOrAbove(m_severity));
    }
} // namespace
