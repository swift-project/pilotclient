/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "statusmessage.h"
#include "statusexception.h"
#include "blackmiscfreefunctions.h"
#include "propertyindex.h"
#include "iconlist.h"
#include "loghandler.h"
#include "logmessage.h"
#include <QMetaEnum>

namespace BlackMisc
{
    CStatusMessage::CStatusMessage(const CStatusMessage &other) :
        CValueObject(other),
        ITimestampBased(other)
    {
        *this = other;
    }

    CStatusMessage &CStatusMessage::operator =(const CStatusMessage &other)
    {
        if (this == &other) { return *this; }

        QReadLocker readLock(&other.m_lock);
        auto tuple = std::make_tuple(other.m_categories, other.m_severity, other.m_message, other.m_handledByObjects);
        readLock.unlock(); // avoid deadlock

        QWriteLocker writeLock(&this->m_lock);
        std::tie(m_categories, m_severity, m_message, m_handledByObjects) = tuple;
        return *this;
    }

    CStatusMessage::CStatusMessage(const QString &message)
        : m_message(message)
    {}

    CStatusMessage::CStatusMessage(StatusSeverity severity, const QString &message)
        : m_severity(severity), m_message(message)
    {}

    CStatusMessage::CStatusMessage(const CLogCategoryList &categories, StatusSeverity severity, const QString &message)
        : m_categories(categories), m_severity(severity), m_message(message)
    {}

    CStatusMessage::CStatusMessage(QtMsgType type, const QMessageLogContext &context, const QString &message)
        : CStatusMessage(message)
    {
        bool debug = CLogMessageHelper::hasDebugFlag(context.category);
        auto categories = CLogMessageHelper::stripFlags(context.category);
        m_categories = CLogCategoryList::fromQString(categories);

        switch (type)
        {
        default:
        case QtDebugMsg:
            this->m_severity = debug ? SeverityDebug : SeverityInfo;
            break;
        case QtWarningMsg:
            this->m_severity = SeverityWarning;
            break;
        case QtCriticalMsg:
        case QtFatalMsg:
            this->m_severity = SeverityError;
            break;
        }
    }

    void CStatusMessage::toQtLogTriple(QtMsgType *o_type, QString *o_category, QString *o_message) const
    {
        auto category = m_categories.toQString();
        if (this->m_severity == SeverityDebug && ! category.isEmpty())
        {
            category = CLogMessageHelper::addDebugFlag(category);
        }

        *o_category = category;
        *o_message = this->m_message;

        switch (this->m_severity)
        {
        default:
        case SeverityDebug:
        case SeverityInfo:
            *o_type = QtDebugMsg;
            break;
        case SeverityWarning:
            *o_type = QtWarningMsg;
            break;
        case SeverityError:
            *o_type = QtCriticalMsg;
            break;
        }
    }

    CStatusException CStatusMessage::asException() const
    {
        return CStatusException(*this);
    }

    void CStatusMessage::maybeThrow() const
    {
        if (! this->isEmpty())
        {
            throw this->asException();
        }
    }

    QString CStatusMessage::getHumanReadablePattern() const
    {
        QStringList patternNames(getHumanReadablePatterns());
        return patternNames.isEmpty() ? "" : patternNames.join(", ");
    }

    QStringList CStatusMessage::getHumanReadablePatterns() const
    {
        QStringList patternNames;
        for (const QString &name : CLogPattern::allHumanReadableNames())
        {
            if (CLogPattern::fromHumanReadableName(name).match(*this)) { patternNames.push_back(name); }
        }
        return patternNames;
    }

    void CStatusMessage::prependMessage(const QString &msg)
    {
        if (msg.isEmpty()) { return; }
        this->m_message = msg + this->m_message;
    }

    void CStatusMessage::appendMessage(const QString &msg)
    {
        if (msg.isEmpty()) { return; }
        this->m_message += msg;
    }

    void CStatusMessage::markAsHandledBy(const QObject *object) const
    {
        this->m_handledByObjects.push_back(quintptr(object));
    }

    bool CStatusMessage::wasHandledBy(const QObject *object) const
    {
        return this->m_handledByObjects.contains(quintptr(object));
    }

    QString CStatusMessage::convertToQString(bool /** i18n */) const
    {

        QString s("Category: ");
        s.append(this->m_categories.toQString());

        s.append(" Severity: ");
        s.append(QString::number(this->m_severity));

        s.append(" when: ");
        s.append(this->getFormattedUtcTimestampYmdhms());

        s.append(" ").append(this->m_message);
        return s;
    }

    const CIcon &CStatusMessage::convertToIcon(const CStatusMessage &statusMessage)
    {
        return convertToIcon(statusMessage.getSeverity());
    }

    const CIcon &CStatusMessage::convertToIcon(CStatusMessage::StatusSeverity severity)
    {
        switch (severity)
        {
        case SeverityDebug: return CIconList::iconByIndex(CIcons::StandardIconUnknown16); // TODO
        case SeverityInfo: return CIconList::iconByIndex(CIcons::StandardIconInfo16);
        case SeverityWarning: return CIconList::iconByIndex(CIcons::StandardIconWarning16);
        case SeverityError: return CIconList::iconByIndex(CIcons::StandardIconError16);
        default: return CIconList::iconByIndex(CIcons::StandardIconInfo16);
        }
    }

    CStatusMessage CStatusMessage::fromDatabaseJson(const QJsonObject &json)
    {
        QString msgText(json.value("text").toString());
        QString severityText(json.value("severity").toString());
        QString typeText(json.value("type").toString());
        StatusSeverity severity = stringToSeverity(severityText);

        typeText = "swift.db.type." + typeText.toLower().remove(' ');
        CStatusMessage m({ CLogCategory::swiftDbWebservice(), CLogCategory(typeText)}, severity, msgText);
        return m;
    }

    void CStatusMessage::registerMetadata()
    {
        CValueObject<CStatusMessage>::registerMetadata();
        qRegisterMetaType<StatusSeverity>();
    }

    CStatusMessage::StatusSeverity CStatusMessage::stringToSeverity(const QString &severity)
    {
        // pre-check
        QString severityString(severity.trimmed().toLower());
        if (severityString.isEmpty()) { return SeverityInfo; }

        // hard check
        if (severityString.compare(severityToString(SeverityDebug), Qt::CaseInsensitive) == 0) { return SeverityDebug; }
        if (severityString.compare(severityToString(SeverityInfo), Qt::CaseInsensitive) == 0) { return SeverityInfo; }
        if (severityString.compare(severityToString(SeverityWarning), Qt::CaseInsensitive) == 0) { return SeverityWarning; }
        if (severityString.compare(severityToString(SeverityError), Qt::CaseInsensitive) == 0) { return SeverityError; }

        // not found yet, lenient checks
        QChar s = severityString.at(0);
        if (s == 'd') { return SeverityDebug; }
        if (s == 'i') { return SeverityInfo; }
        if (s == 'w') { return SeverityWarning; }
        if (s == 'e') { return SeverityError; }

        return SeverityInfo;
    }

    const QString &CStatusMessage::severityToString(CStatusMessage::StatusSeverity severity)
    {
        switch (severity)
        {
        case SeverityDebug:
            {
                static QString d("debug");
                return d;
            }
        case SeverityInfo:
            {
                static QString i("info");
                return i;
            }
        case SeverityWarning:
            {
                static QString w("warning");
                return w;
            }
        case SeverityError:
            {
                static QString e("error");
                return e;
            }
        default:
            static QString x("unknown severity");
            qFatal("Unknown severity");
            return x; // just for compiler warning
        }
    }

    QString CStatusMessage::severitiesToString(const QSet<CStatusMessage::StatusSeverity> &severities)
    {
        auto minmax = std::minmax_element(severities.begin(), severities.end());
        auto min = *minmax.first;
        auto max = *minmax.second;
        if (min == SeverityDebug && max == SeverityError)
        {
            return "all severities";
        }
        if (min == SeverityDebug)
        {
            return "at or below " + severityToString(max);
        }
        if (max == SeverityError)
        {
            return "at or above " + severityToString(min);
        }
        auto list = severities.toList();
        std::sort(list.begin(), list.end());
        QStringList ret;
        std::transform(list.cbegin(), list.cend(), std::back_inserter(ret), severityToString);
        return ret.join("|");
    }

    const QString &CStatusMessage::getSeverityAsString() const
    {
        return severityToString(this->m_severity);
    }

    const QStringList &CStatusMessage::allSeverityStrings()
    {
        static const QStringList all { severityToString(SeverityDebug), severityToString(SeverityInfo), severityToString(SeverityWarning), severityToString(SeverityError) };
        return all;
    }

    CVariant CStatusMessage::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        if (index.isMyself()) { return CVariant::from(*this); }
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexMessage:
            return CVariant::from(this->m_message);
        case IndexSeverity:
            return CVariant::from(this->m_severity);
        case IndexSeverityAsString:
            return CVariant::from(this->getSeverityAsString());
        case IndexCategories:
            return CVariant::from(this->m_categories.toQString());
        case IndexCategoryHumanReadable:
            return CVariant::from(this->getHumanReadablePattern());
        default:
            return CValueObject::propertyByIndex(index);
        }
    }

    void CStatusMessage::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
    {
        if (index.isMyself()) { (*this) = variant.to<CStatusMessage>(); return; }
        if (ITimestampBased::canHandleIndex(index)) { ITimestampBased::setPropertyByIndex(variant, index); return; }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexMessage:
            this->m_message = variant.value<QString>();
            break;
        case IndexSeverity:
            this->m_severity = variant.value<StatusSeverity>();
            break;
        case IndexCategories:
            this->m_categories = variant.value<CLogCategoryList>();
            break;
        default:
            CValueObject::setPropertyByIndex(variant, index);
            break;
        }
    }

    QString CStatusMessage::toHtml() const
    {
        QString html;
        if (this->isEmpty()) return html;
        switch (this->getSeverity())
        {
        case SeverityInfo:
            break;
        case SeverityWarning:
            html = "<font color=\"yellow\">";
            break;
        case SeverityError:
            html = "<font color=\"red\">";
            break;
        case SeverityDebug:
            break;
        default:
            break;
        }
        html.append(this->getMessage());
        if (this->getSeverity() == SeverityInfo) return html;
        html.append("</font>");
        return html;
    }
} // ns
