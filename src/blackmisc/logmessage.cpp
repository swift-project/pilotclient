/* Copyright (C) 2014
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "logmessage.h"
#include "blackmiscfreefunctions.h"

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

    CLogMessage &CLogMessage::validation(const CStatusMessage &statusMessage)
    {
        switch (statusMessage.getSeverity())
        {
        case CStatusMessage::SeverityDebug:
        case CStatusMessage::SeverityInfo:
            return validationInfo(statusMessage.getMessage());
        case CStatusMessage::SeverityWarning:
            return validation(statusMessage.getMessage());
        case CStatusMessage::SeverityError:
            return error(statusMessage.getMessage());
        default:
            return validationInfo(statusMessage.getMessage());
        }
    }

    void CLogMessage::validations(const CStatusMessageList &statusMessages)
    {
        foreach(CStatusMessage msg, statusMessages)
        {
            validation(msg);
        }
    }

    CLogMessage::operator CStatusMessage()
    {
        m_redundant = true;
        return { m_categories, m_severity, message() };
    }

    CLogMessage::operator CVariant()
    {
        return CVariant::from(static_cast<CStatusMessage>(*this));
    }

    CLogMessage::~CLogMessage()
    {
        // ostream(encodedCategory()) << message(); // QDebug::operator<<(QString) puts quote characters around the message

        // FIXME hack to avoid putting quote characters around the message
        // should be safe, but still it's horrible, we could directly call qt_message_output instead
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
            if (m_redundant) { category = CLogMessageHelper::addRedundantFlag(category); }
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

    QString CLogMessage::message() const
    {
        if (m_message.isEmpty())
        {
            return m_args.join(" ");
        }
        else
        {
            // TODO would like to have a QString::arg(QStringList) overload
            switch (m_args.size())
            {
            case 0: return m_message;
            case 1: return m_message.arg(m_args[0]);
            case 2: return m_message.arg(m_args[0], m_args[1]);
            case 3: return m_message.arg(m_args[0], m_args[1], m_args[2]);
            case 4: return m_message.arg(m_args[0], m_args[1], m_args[2], m_args[3]);
            case 5: return m_message.arg(m_args[0], m_args[1], m_args[2], m_args[3], m_args[4]);
            case 6: return m_message.arg(m_args[0], m_args[1], m_args[2], m_args[3], m_args[4], m_args[5]);
            case 7: return m_message.arg(m_args[0], m_args[1], m_args[2], m_args[3], m_args[4], m_args[5], m_args[6]);
            case 8: return m_message.arg(m_args[0], m_args[1], m_args[2], m_args[3], m_args[4], m_args[5], m_args[6], m_args[7]);
            default: qWarning("Too many arguments");
            case 9: return m_message.arg(m_args[0], m_args[1], m_args[2], m_args[3], m_args[4], m_args[5], m_args[6], m_args[7], m_args[8]);
            }
        }
    }

    bool hasFlag(const QString &category, const QString &flag)
    {
        return category.section("/", 1, -1).split("/").contains(flag);
    }
    QString addFlag(QString category, const QString &flag)
    {
        if (category.isEmpty() || hasFlag(category, flag)) return category;
        return category + "/" + flag;
    }
    QString CLogMessageHelper::addRedundantFlag(const QString &category) { return addFlag(category, "redundant"); }
    QString CLogMessageHelper::addDebugFlag(const QString &category) { return addFlag(category, "debug"); }
    QString CLogMessageHelper::stripFlags(const QString &category) { return category.section("/", 0, 1); }
    bool CLogMessageHelper::hasRedundantFlag(const QString &category) { return hasFlag(category, "redundant"); }
    bool CLogMessageHelper::hasDebugFlag(const QString &category)
    {
        return hasFlag(category, "debug") || category.isEmpty()
               || (QLoggingCategory::defaultCategory() && category == QLoggingCategory::defaultCategory()->categoryName());
    }

}
