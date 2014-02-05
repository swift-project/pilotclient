#include "statusmessage.h"
#include "blackmiscfreefunctions.h"
#include <QMetaEnum>

namespace BlackMisc
{

    /*
     * Constructor
     */
    CStatusMessage::CStatusMessage(StatusType type, StatusSeverity severity, const QString &message)
        : m_type(type), m_severity(severity), m_message(message), m_timestamp(QDateTime::currentDateTimeUtc())
    {
        // void
    }

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
     * Compare
     */
    int CStatusMessage::compareImpl(const CValueObject &otherBase) const
    {
        const auto &other = static_cast<const CStatusMessage &>(otherBase);

        if (this->m_type < other.m_type) { return -1; }
        if (this->m_type > other.m_type) { return 1; }
        return this->m_message.compare(other.m_message);
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
     * Validation Unspecific info message
     */
    CStatusMessage CStatusMessage::getInfoMessage(const QString &message)
    {
        return CStatusMessage(CStatusMessage::TypeUnspecific, CStatusMessage::SeverityInfo, message);
    }

    /*
     * Hash
     */
    uint CStatusMessage::getValueHash() const
    {
        QList<uint> hashs;
        hashs << qHash(static_cast<qint32>(this->m_type));
        hashs << qHash(static_cast<qint32>(this->m_severity));
        hashs << qHash(this->m_message);
        hashs << qHash(this->m_timestamp);
        return BlackMisc::calculateHash(hashs, "CStatusMessage");
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
            return x; // just for compiler warning
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
     * Equal
     */
    bool CStatusMessage::operator ==(const CStatusMessage &other) const
    {
        return this->m_severity == other.m_severity &&
               this->m_type == other.m_type &&
               this->m_timestamp == other.m_timestamp &&
               this->m_message == other.m_message;
    }

    /*
     * Equal
     */
    bool CStatusMessage::operator !=(const CStatusMessage &other) const
    {
        return !(other == (*this));
    }

    /*
     * To DBus
     */
    void CStatusMessage::marshallToDbus(QDBusArgument &arg) const
    {
        arg << this->m_message;
        arg << static_cast<qint32>(this->m_type);
        arg << static_cast<qint32>(this->m_severity);
        arg << this->m_timestamp;
    }

    /*
     * From DBus
     */
    void CStatusMessage::unmarshallFromDbus(const QDBusArgument &arg)
    {
        qint32 type;
        qint32 severity;
        arg >> this->m_message;
        arg >> type;
        arg >> severity;
        arg >> m_timestamp;
        this->m_type = static_cast<StatusType>(type);
        this->m_severity = static_cast<StatusSeverity>(severity);
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
    void CStatusMessage::propertyByIndex(const QVariant &variant, int index)
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
}
