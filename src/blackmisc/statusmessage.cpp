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
    void CStatusMessage::registerMetadata()
    {
        CValueObject<CStatusMessage>::registerMetadata();
        qRegisterMetaType<StatusSeverity>();
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
        m_redundant = CLogMessageHelper::hasRedundantFlag(context.category);
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
        if (this->m_redundant)
        {
            category = CLogMessageHelper::addRedundantFlag(category);
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

    QString CStatusMessage::getHumanReadableCategory() const
    {
        //! \todo This should me not hardcoded
        if (this->m_humanReadableCategory.isEmpty())
        {
            const QString cat(this->m_categories.toQString().toLower());
            // could als be subject of i18n
            // from sepcific to unspecific
            if (cat.isEmpty()) { this->m_humanReadableCategory = "None"; }
            else if (cat.contains(CLogCategory::validation().toQString())) { this->m_humanReadableCategory = "Validation"; }
            else if (cat.contains("contextaudio")) { this->m_humanReadableCategory = "Audio"; }
            else if (cat.contains("contextsimulator")) { this->m_humanReadableCategory = "Simulator"; }
            else if (cat.contains("contextnetwork")) { this->m_humanReadableCategory = "Network"; }
            else if (cat.contains("vatlib")) { this->m_humanReadableCategory = "VATSIM library"; }
            else if (cat.contains("blackmisc")) { this->m_humanReadableCategory = "Library"; }
            else if (cat.contains("blackcore")) { this->m_humanReadableCategory = "Core"; }
            else if (cat.contains("blackgui")) { this->m_humanReadableCategory = "GUI"; }
            else if (cat.contains("blacksound")) { this->m_humanReadableCategory = "GUI"; }
            else if (cat.contains("interpolator")) { this->m_humanReadableCategory = "Interpolator"; }
            else if (cat.contains("xplane")) { this->m_humanReadableCategory = "XPlane"; }
            else if (cat.contains("fsx")) { this->m_humanReadableCategory = "FSX"; }
            else if (cat.contains("fs9")) { this->m_humanReadableCategory = "FS9"; }
            else if (cat.contains("mapping") || cat.contains("matching")) { this->m_humanReadableCategory = "Model matching"; }

            else this->m_humanReadableCategory = "Misc.";
        }
        return this->m_humanReadableCategory;
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
        CLogCategory cat("swift.db");
        QString msgText(json.value("text").toString());
        QString severityText(json.value("severity").toString());
        QString typeText(json.value("type").toString());
        StatusSeverity severity = stringToSeverity(severityText);

        typeText = "swift.db.type." + typeText.toLower().remove(' ');

        CStatusMessage m({ cat, CLogCategory(typeText)}, severity, msgText);
        return m;
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
            return CVariant::from(this->getHumanReadableCategory());
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
}
