#include "blackmisc/message.h"

namespace BlackMisc
{
    IMessage::IMessage(QString& id)
    {
        m_message_id = id;
    }

    QString IMessage::getID() const
    {
        return m_message_id;
    }

} // namespace BlackMisc
