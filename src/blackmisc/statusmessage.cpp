/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "statusmessage.h"
#include "blackmiscfreefunctions.h"
#include "propertyindex.h"
#include "iconlist.h"
#include "loghandler.h"
#include "logmessage.h"
#include <QMetaEnum>

namespace BlackMisc
{

    /*
     * Constructors
     */
    CStatusMessage::CStatusMessage()
        : m_timestamp(QDateTime::currentDateTimeUtc())
    {}

    CStatusMessage::CStatusMessage(const QString &message)
        : m_message(message), m_timestamp(QDateTime::currentDateTimeUtc())
    {}

    CStatusMessage::CStatusMessage(StatusSeverity severity, const QString &message)
        : m_severity(severity), m_message(message), m_timestamp(QDateTime::currentDateTimeUtc())
    {}

    CStatusMessage::CStatusMessage(const QString &category, StatusSeverity severity, const QString &message)
        : m_category(category), m_severity(severity), m_message(message), m_timestamp(QDateTime::currentDateTimeUtc())
    {}

    CStatusMessage::CStatusMessage(QtMsgType type, const QMessageLogContext &context, const QString &message)
        : CStatusMessage(context.category, SeverityInfo, message)
    {
        m_redundant = CLogMessageHelper::hasRedundantFlag(m_category);
        bool debug = CLogMessageHelper::hasDebugFlag(m_category);
        m_category = CLogMessageHelper::stripFlags(m_category);

        switch(type)
        {
        default:
        case QtDebugMsg:
            if (debug)
                this->m_severity = SeverityDebug;
            else
                this->m_severity = SeverityInfo;
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

    /*
     * Equal?
     */
    bool CStatusMessage::operator ==(const CStatusMessage &other) const
    {
        if (this == &other) return true;
        return TupleConverter<CStatusMessage>::toTuple(*this) == TupleConverter<CStatusMessage>::toTuple(other);
    }

    /*
     * Unequal?
     */
    bool CStatusMessage::operator !=(const CStatusMessage &other) const
    {
        return !((*this) == other);
    }

    /*
     * Conversion
     */
    void CStatusMessage::toQtLogTriple(QtMsgType *o_type, QString *o_category, QString *o_message) const
    {
        QString category = m_category;
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

    /*
     * To string
     */
    QString CStatusMessage::convertToQString(bool /** i18n */) const
    {

        QString s("Category: ");
        s.append(this->m_category);

        s.append(" Severity: ");
        s.append(QString::number(this->m_severity));

        s.append(" when: ");
        s.append(this->m_timestamp.toString("yyyy-MM-dd HH:mm::ss"));

        s.append(" ").append(this->m_message);
        return s;
    }

    /*
     *  Pixmap
     */
    const CIcon &CStatusMessage::convertToIcon(const CStatusMessage &statusMessage)
    {
        switch (statusMessage.getSeverity())
        {
        case SeverityDebug: return CIconList::iconForIndex(CIcons::StandardIconUnknown16); // TODO
        case SeverityInfo: return CIconList::iconForIndex(CIcons::StandardIconInfo16);
        case SeverityWarning: return CIconList::iconForIndex(CIcons::StandardIconWarning16);
        case SeverityError: return CIconList::iconForIndex(CIcons::StandardIconError16);
        default: return CIconList::iconForIndex(CIcons::StandardIconInfo16);
        }
    }

    /*
     *  Severity
     */
    const QString &CStatusMessage::getSeverityAsString() const
    {
        switch (this->m_severity)
        {
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

    /*
     * Property by index
     */
    QVariant CStatusMessage::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        if (index.isMyself()) { return this->toQVariant(); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexMessage:
            return QVariant(this->m_message);
        case IndexSeverity:
            return QVariant(static_cast<uint>(this->m_severity));
        case IndexSeverityAsString:
            return QVariant(this->getSeverityAsString());
        case IndexTimestamp:
            return QVariant(this->m_timestamp);
        case IndexTimestampFormatted:
            {
                if (this->m_timestamp.isNull() || !this->m_timestamp.isValid()) return "";
                return this->m_timestamp.toString("HH:mm::ss.zzz");
            }
        case IndexCategory:
            return QVariant(this->m_category);
        default:
            break;
        }

        Q_ASSERT_X(false, "CStatusMessage", "index unknown");
        QString m = QString("no property, index ").append(index.toQString());
        return QVariant::fromValue(m);
    }

    /*
     * Set property as index
     */
    void CStatusMessage::setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index)
    {
        if (index.isMyself())
        {
            this->convertFromQVariant(variant);
            return;
        }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexMessage:
            this->m_message = variant.value<QString>();
            break;
        case IndexTimestamp:
            this->m_timestamp = variant.value<QDateTime>();
            break;
        case IndexSeverity:
            this->m_severity = static_cast<StatusSeverity>(variant.value<uint>());
            break;
        case IndexCategory:
            this->m_category = variant.value<QString>();
            break;
        default:
            CValueObject::setPropertyByIndex(variant, index);
            break;
        }
    }

    /*
     * Message as HTML
     */
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
