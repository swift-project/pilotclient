/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/statusmessagefilter.h"

using namespace BlackMisc;

namespace BlackGui
{
    namespace Models
    {
        CStatusMessageFilter::CStatusMessageFilter(CStatusMessage::StatusSeverity severity, const QString &text, const QString &category) :
            m_severity(severity), m_msgText(text.trimmed()), m_category(category.trimmed())
        {
            // info or debug are treated like no filter
            this->m_valid =  !((severity == CStatusMessage::SeverityInfo || severity == CStatusMessage::SeverityDebug) && m_msgText.isEmpty() && m_category.isEmpty());
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
                    if (!this->stringMatchesFilterExpression(msg.getHumanOrTechnicalCategoriesAsString(), this->m_category)) { continue; }
                }

                outContainer.push_back(msg);
            }
            return outContainer;
        }
    } // namespace
} // namespace
