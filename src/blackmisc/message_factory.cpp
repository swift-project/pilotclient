#include "blackmisc/message_factory.h"

namespace BlackMisc
{
    SINGLETON_CLASS_IMPLEMENTATION(CMessageFactory)

    IMessageCreator::IMessageCreator(const QString &messageID)
    {
        CMessageFactory::getInstance().registerMessage(messageID, this);
    }

    IMessage* CMessageFactory::create(const QString &messageID)
    {
        if (m_creators.contains(messageID))
        {
            IMessageCreator* creator = m_creators.value(messageID);
            IMessage* msg = creator->create();
            return msg;
        }
        else
            return (IMessage*)NULL;
    }

    CMessageFactory::~CMessageFactory()
    {
        TMessageCreatorHash::iterator it = m_creators.begin();
        while (it != m_creators.end())
        {
            IMessageCreator* creator = it.value();
            if (creator)
                delete creator;
            ++it;
        }
    }

    void CMessageFactory::registerMessage(const QString &messageID, IMessageCreator *creator)
    {
        m_creators.insert(messageID, creator);
    }

    void CMessageFactory::registerMessages()
    {
        REGISTER_MESSAGE(TestMessage, MSG_ID_TestMessage);
		REGISTER_MESSAGE(MSG_CONNECT_TO_VATSIM, MSG_ID_CONNECT_TO_VATSIM);
		REGISTER_MESSAGE(MSG_CHAT_MESSAGE, MSG_ID_CHAT_MESSAGE);
    }

} // namespace BlackMisc
