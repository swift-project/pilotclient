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
#include "iconlist.h"
#include <QMetaEnum>

namespace BlackMisc
{

    /*
     * Constructor
     */
    CStatusMessage::CStatusMessage(StatusType type, StatusSeverity severity, const QString &message)
        : m_type(type), m_severity(severity), m_message(message), m_timestamp(QDateTime::currentDateTimeUtc())
    {  }

    /*
     * Constructor
     */
    CStatusMessage::CStatusMessage(StatusType type, StatusSeverity severity, const char *message)
        : m_type(type), m_severity(severity), m_message(QString(message)), m_timestamp(QDateTime::currentDateTimeUtc())
    {  }

    /*
     * To string
     */
    QString CStatusMessage::convertToQString(bool /** i18n */) const
    {

        QString s("Index: ");
        s.append(QString::number(this->m_type));

        s.append(" Severity: ");
        s.append(QString::number(this->m_severity));

        s.append(" when: ");
        s.append(this->m_timestamp.toString("yyyy-MM-dd HH:mm::ss"));

        s.append(" ").append(this->m_message);
        return s;
    }

    /*
     * metaTypeId
     */
    int CStatusMessage::getMetaTypeId() const
    {
        return qMetaTypeId<CStatusMessage>();
    }

    /*
     * is a
     */
    bool CStatusMessage::isA(int metaTypeId) const
    {
        if (metaTypeId == qMetaTypeId<CStatusMessage>()) { return true; }
        return this->CValueObject::isA(metaTypeId);
    }

    /*
     * Metadata
     */
    void CStatusMessage::registerMetadata()
    {
        qRegisterMetaType<CStatusMessage>();
        qDBusRegisterMetaType<CStatusMessage>();
    }

    /*
     * Validation Error
     */
    CStatusMessage CStatusMessage::getValidationError(const QString &message)
    {
        return CStatusMessage(CStatusMessage::TypeValidation, CStatusMessage::SeverityError, message);
    }

    /*
     * Unspecific info message
     */
    CStatusMessage CStatusMessage::getInfoMessage(const QString &message, StatusType type)
    {
        return CStatusMessage(type, CStatusMessage::SeverityInfo, message);
    }

    /*
     * Unspecific warning message
     */
    CStatusMessage CStatusMessage::getWarningMessage(const QString &message, StatusType type)
    {
        return CStatusMessage(type, CStatusMessage::SeverityWarning, message);
    }

    /*
     * Unspecific error message
     */
    CStatusMessage CStatusMessage::getErrorMessage(const QString &message, StatusType type)
    {
        return CStatusMessage(type, CStatusMessage::SeverityError, message);
    }

    /*
     *  Pixmap
     */
    const CIcon &CStatusMessage::convertToIcon(const CStatusMessage &statusMessage)
    {
        switch (statusMessage.getSeverity())
        {
        case SeverityInfo: return CIconList::iconForIndex(CIcons::StandardIconInfo16);
        case SeverityWarning: return CIconList::iconForIndex(CIcons::StandardIconWarning16);
        case SeverityError: return CIconList::iconForIndex(CIcons::StandardIconError16);
        default: return CIconList::iconForIndex(CIcons::StandardIconInfo16);
        }
    }

    /*
     * Type
     */
    const QString &CStatusMessage::getTypeAsString() const
    {
        switch (this->m_type)
        {
        case TypeAudio:
            {
                static QString t("audio");
                return t;
            }
        case TypeCore:
            {
                static QString t("core");
                return t;
            }
        case TypeGui:
            {
                static QString t("gui");
                return t;
            }
        case TypeSettings:
            {
                static QString t("settings");
                return t;
            }
        case TypeSimulator:
            {
                static QString t("simulator");
                return t;
            }
        case TypeStdoutRedirect:
            {
                static QString t("redirection");
                return t;
            }
        case TypeTrafficNetwork:
            {
                static QString t("traffic network");
                return t;
            }
        case TypeUnknown:
            {
                static QString t("unknown");
                return t;
            }
        case TypeUnspecific:
            {
                static QString t("unspecific");
                return t;
            }
        case TypeValidation:
            {
                static QString t("validation");
                return t;
            }
        default:
            static QString x("unknown type");
            qFatal("Unknown type");
            return x;
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
     * Compare
     */
    int CStatusMessage::compareImpl(const CValueObject &otherBase) const
    {
        const auto &other = static_cast<const CStatusMessage &>(otherBase);
        return compare(TupleConverter<CStatusMessage>::toTuple(*this), TupleConverter<CStatusMessage>::toTuple(other));
    }

    /*
     * Marshall to DBus
     */
    void CStatusMessage::marshallToDbus(QDBusArgument &argument) const
    {
        argument << TupleConverter<CStatusMessage>::toTuple(*this);
    }

    /*
     * Unmarshall from DBus
     */
    void CStatusMessage::unmarshallFromDbus(const QDBusArgument &argument)
    {
        argument >> TupleConverter<CStatusMessage>::toTuple(*this);
    }

    /*
     * Hash
     */
    uint CStatusMessage::getValueHash() const
    {
        return qHash(TupleConverter<CStatusMessage>::toTuple(*this));
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
     * Equal
     */
    bool CStatusMessage::operator !=(const CStatusMessage &other) const
    {
        return !(other == (*this));
    }

    /*
     * Property by index
     */
    QVariant CStatusMessage::propertyByIndex(int index) const
    {
        switch (index)
        {
        case IndexMessage:
            return QVariant(this->m_message);
        case IndexSeverity:
            return QVariant(static_cast<uint>(this->m_severity));
        case IndexSeverityAsString:
            return QVariant(this->getSeverityAsString());
        case IndexTimestamp:
            return QVariant(this->m_timestamp);
        case IndexType:
            return QVariant(static_cast<uint>(this->m_type));
        case IndexTypeAsString:
            return QVariant(this->getTypeAsString());
        default:
            break;
        }

        Q_ASSERT_X(false, "CStatusMessage", "index unknown");
        QString m = QString("no property, index ").append(QString::number(index));
        return QVariant::fromValue(m);
    }

    /*
     * Set property as index
     */
    void CStatusMessage::setPropertyByIndex(const QVariant &variant, int index)
    {
        switch (index)
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
        case IndexType:
            this->m_type = static_cast<StatusType>(variant.value<uint>());
            break;
        default:
            Q_ASSERT_X(false, "CStatusMessage", "index unknown (setter)");
            break;
        }
    }

    /*
     * Property as string by index
     */
    QString CStatusMessage::propertyByIndexAsString(int index, bool i18n) const
    {
        QVariant qv = this->propertyByIndex(index);
        switch (index)
        {
        case IndexTimestamp:
            {
                QDateTime dt = qv.value<QDateTime>();
                if (dt.isNull() || !dt.isValid()) return "";
                return dt.toString("HH:mm::ss.zzz");
                break;
            }
        default:
            break;
        }
        return BlackMisc::qVariantToString(qv, i18n);
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
        case SeverityError:
            html = "<font color=\"red\">";
        }
        html.append(this->getMessage());
        if (this->getSeverity() == SeverityInfo) return html;
        html.append("</font>");
        return html;
    }
}
