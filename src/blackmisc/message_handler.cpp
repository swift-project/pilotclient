#include "blackmisc/message_handler.h"

namespace BlackMisc
{
    CMessageHandler::~CMessageHandler()
    {
        TFunctionHandlerMap::iterator it = m_messagehandler.begin();
        while (it != m_messagehandler.end())
        {
            delete it.value();
            ++it;
        }
        m_messagehandler.clear();
    }

    void CMessageHandler::handleMessage(const IMessage * message)
    {
        TFunctionHandlerMap::iterator it = m_messagehandler.find(CTypeInfo(typeid(*message)));
        if (it != m_messagehandler.end())
        {
            it.value()->exec(message);
        }
    }

} // namespace BlackMisc
