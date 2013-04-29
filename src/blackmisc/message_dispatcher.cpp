#include "blackmisc/message_handler.h"
#include "blackmisc/message_dispatcher.h"

namespace BlackMisc
{
SINGLETON_CLASS_IMPLEMENTATION(CMessageDispatcher)

void CMessageDispatcher::append(IMessage *message)
{
    m_messageQueue.enqueue(message);
}

void CMessageDispatcher::dispatch()
{
    IMessage *message = NULL;

    if (m_messageQueue.isEmpty())
        return;

    message = m_messageQueue.dequeue();

    if (message != NULL)
    {
        CTypeInfo typeinfo = CTypeInfo(typeid(*message));
        QList<CMessageHandler *> neededHandlers = m_messageHander.values(typeinfo);
        int handlerSize = neededHandlers.size();
        for (int i = 0; i < handlerSize; ++i)
            neededHandlers.at(i)->handleMessage(message);
    }
}

} // namespace BlackMisc
