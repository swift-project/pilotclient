#include "statusmessages.h"
#include "statusmessage.h"

namespace BlackMisc
{
    /*
     * Messages by type
     */
    CStatusMessages CStatusMessages::findByType(CStatusMessage::StatusType type) const
    {
        CStatusMessages sm;
        foreach(CStatusMessage message, this->m_messages)
        {
            if (message.getType() == type)
            {
                sm.m_messages.append(message);
            }
        }
        return sm;
    }

    /*
     * Messages by severity
     */
    CStatusMessages CStatusMessages::findBySeverity(CStatusMessage::StatusSeverity severity) const
    {
        CStatusMessages sm;
        foreach(CStatusMessage message, this->m_messages)
        {
            if (message.getSeverity() == severity)
            {
                sm.m_messages.append(message);
            }
        }
        return sm;
    }

    /*
     * Size
     */
    int CStatusMessages::size() const
    {
        return this->m_messages.size();
    }

    /*
     * Append
     */
    void CStatusMessages::append(const CStatusMessage &message)
    {
        return this->m_messages.append(message);
    }

    /*
     * Metadata
     */
    void CStatusMessages::registerMetadata()
    {
        qRegisterMetaType<CStatusMessages>();
        qDBusRegisterMetaType<CStatusMessages>();
    }

    /*
     * To DBus
     */
    void CStatusMessages::marshallToDbus(QDBusArgument &arg) const
    {
        arg << this->m_messages;
    }

    /*
     * From DBus
     */
    void CStatusMessages::unmarshallFromDbus(const QDBusArgument &arg)
    {
        arg >> this->m_messages;
    }

    /*
     * To string
     */
    QString CStatusMessages::convertToQString(bool i18n) const
    {
        QString s = "{";
        foreach(CStatusMessage message, this->m_messages)
        {
            if (s.length() > 1) s.append(", ");
            s.append(message.toQString(i18n));
        }
        s.append("}");
        return s;
    }
}
