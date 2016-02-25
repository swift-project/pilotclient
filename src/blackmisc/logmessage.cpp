/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "logmessage.h"
#include "blackmiscfreefunctions.h"
#include "indexsequence.h"

namespace BlackMisc
{

    CLogMessage &CLogMessage::debug()
    {
        m_severity = CStatusMessage::SeverityDebug;
        return *this;
    }

    CLogMessage &CLogMessage::info(QString format)
    {
        m_severity = CStatusMessage::SeverityInfo;
        m_message = format;
        return *this;
    }

    CLogMessage &CLogMessage::warning(QString format)
    {
        m_severity = CStatusMessage::SeverityWarning;
        m_message = format;
        return *this;
    }

    CLogMessage &CLogMessage::error(QString format)
    {
        m_severity = CStatusMessage::SeverityError;
        m_message = format;
        return *this;
    }

    CLogMessage &CLogMessage::validationInfo(QString format)
    {
        m_categories.remove(CLogCategory::uncategorized());
        m_categories.push_back(CLogCategory::validation());
        return info(format);
    }

    CLogMessage &CLogMessage::validationWarning(QString format)
    {
        m_categories.remove(CLogCategory::uncategorized());
        m_categories.push_back(CLogCategory::validation());
        return warning(format);
    }

    CLogMessage &CLogMessage::validationError(QString format)
    {
        m_categories.remove(CLogCategory::uncategorized());
        m_categories.push_back(CLogCategory::validation());
        return error(format);
    }

    CLogMessage::operator CStatusMessage()
    {
        return { m_categories, m_severity, message() };
    }

    CLogMessage::operator CVariant()
    {
        return CVariant::from(static_cast<CStatusMessage>(*this));
    }

    CLogMessage::~CLogMessage()
    {
        // ostream(encodedCategory()) << message(); // QDebug::operator<<(QString) puts quote characters around the message

        // hack to avoid putting quote characters around the message
        // should be safe, we could directly call qt_message_output instead, but it's undocumented
        QByteArray category = qtCategory();
        QDebug debug = ostream(category);
        auto &stream = **reinterpret_cast<QTextStream **>(&debug); // should be safe because it is relying on Qt's guarantee of ABI compatibility
        stream << message();
    }

    QByteArray CLogMessage::qtCategory() const
    {
        if (m_categories.isEmpty())
        {
            return {};
        }
        else
        {
            QString category = m_categories.toQString();
            if (m_severity == CStatusMessage::SeverityDebug) { category = CLogMessageHelper::addDebugFlag(category); }
            return category.toLatin1();
        }
    }

    QDebug CLogMessage::ostream(const QByteArray &category) const
    {
        if (m_categories.isEmpty())
        {
            switch (m_severity)
            {
            default:
            case CStatusMessage::SeverityDebug: return m_logger.debug();
            case CStatusMessage::SeverityInfo: return m_logger.debug();
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
            case CStatusMessage::SeverityInfo: return m_logger.debug(QLoggingCategory(category.constData()));
            case CStatusMessage::SeverityWarning: return m_logger.warning(QLoggingCategory(category.constData()));
            case CStatusMessage::SeverityError: return m_logger.critical(QLoggingCategory(category.constData()));
            }
        }
    }

    namespace Private
    {
        template <size_t... Is> QString arg(index_sequence<Is...>, const QString &format, const QStringList &args) { return format.arg(args[Is]...); }
        QString arg(index_sequence<>, const QString &format, const QStringList &) { return format; }
    }

    QString CLogMessage::message() const
    {
        if (m_message.isEmpty())
        {
            return m_args.join(" ");
        }
        else
        {
            switch (m_args.size())
            {
            case 0: return Private::arg(Private::make_index_sequence<0>(), m_message, m_args);
            case 1: return Private::arg(Private::make_index_sequence<1>(), m_message, m_args);
            case 2: return Private::arg(Private::make_index_sequence<2>(), m_message, m_args);
            case 3: return Private::arg(Private::make_index_sequence<3>(), m_message, m_args);
            case 4: return Private::arg(Private::make_index_sequence<4>(), m_message, m_args);
            case 5: return Private::arg(Private::make_index_sequence<5>(), m_message, m_args);
            case 6: return Private::arg(Private::make_index_sequence<6>(), m_message, m_args);
            case 7: return Private::arg(Private::make_index_sequence<7>(), m_message, m_args);
            case 8: return Private::arg(Private::make_index_sequence<8>(), m_message, m_args);
            default: qWarning("Too many arguments"); // intentional fall-through
            case 9: return Private::arg(Private::make_index_sequence<9>(), m_message, m_args);
            }
        }
    }

    //! Does category contain flag?
    bool hasFlag(const QString &category, const QString &flag)
    {
        return category.section("/", 1, -1).split("/").contains(flag);
    }

    //! Add flag to category
    QString addFlag(QString category, const QString &flag)
    {
        if (category.isEmpty() || hasFlag(category, flag)) return category;
        return category + "/" + flag;
    }
    QString CLogMessageHelper::addDebugFlag(const QString &category) { return addFlag(category, "debug"); }
    QString CLogMessageHelper::stripFlags(const QString &category) { return category.section("/", 0, 1); }
    bool CLogMessageHelper::hasDebugFlag(const QString &category)
    {
        return hasFlag(category, "debug") || category.isEmpty()
               || (QLoggingCategory::defaultCategory() && category == QLoggingCategory::defaultCategory()->categoryName());
    }

    void CLogMessage::preformatted(const CStatusMessage &statusMessage)
    {
        if (statusMessage.isEmpty()) { return; } // just skip empty messages
        CLogMessage msg(statusMessage.getCategories());
        msg.m_severity = statusMessage.getSeverity();
        msg.m_message = statusMessage.getMessage();
    }

    void CLogMessage::preformatted(const CStatusMessageList &statusMessages)
    {
        for(const auto &msg : statusMessages)
        {
            preformatted(msg);
        }
    }
} // ns

//! \endcond
