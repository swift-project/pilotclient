// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE

#include "misc/logmessage.h"

namespace swift::misc
{

    CLogMessage::CLogMessage() = default;

    CLogMessage::CLogMessage(const char *file, int line, const char *function) : m_logger(file, line, function) {}

    CLogMessage::operator CStatusMessage()
    {
        return { m_categories, m_severity, message() };
    }

    CLogMessage::~CLogMessage()
    {
        ostream(qtCategory()).noquote() << message();
    }

    QByteArray CLogMessage::qtCategory() const
    {
        return m_categories.toQString().toLatin1();
    }

    QDebug CLogMessage::ostream(const QByteArray &category) const
    {
        if (m_categories.isEmpty())
        {
            switch (m_severity)
            {
            default:
            case CStatusMessage::SeverityDebug: return m_logger.debug();
            case CStatusMessage::SeverityInfo: return m_logger.info();
            case CStatusMessage::SeverityWarning: return m_logger.warning();
            case CStatusMessage::SeverityError: return m_logger.critical();
            }
        }
        else
        {
            switch (m_severity)
            {
            default:
            case CStatusMessage::SeverityDebug: return m_logger.debug(QLoggingCategory(category.constData()));
            case CStatusMessage::SeverityInfo: return m_logger.info(QLoggingCategory(category.constData()));
            case CStatusMessage::SeverityWarning: return m_logger.warning(QLoggingCategory(category.constData()));
            case CStatusMessage::SeverityError: return m_logger.critical(QLoggingCategory(category.constData()));
            }
        }
    }

    void CLogMessage::preformatted(const CStatusMessage &statusMessage)
    {
        if (statusMessage.isEmpty()) { return; } // just skip empty messages
        CLogMessage(statusMessage.getCategories()).log(statusMessage.getSeverity(), u"%1") << statusMessage.getMessage();
    }

    void CLogMessage::preformatted(const CStatusMessageList &statusMessages)
    {
        for (const auto &msg : statusMessages)
        {
            preformatted(msg);
        }
    }
} // namespace swift::misc

//! \endcond
