//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <QMap>
#include "blackmisc/message.h"
#include "blackmisc/debug.h"
#include "blackmisc/type_info.h"
#include "blackmisc/message_dispatcher.h"

namespace BlackMisc
{

    class IFunctionHandler
    {
    public:
        virtual ~IFunctionHandler() {}
        void exec (const IMessage* message)
        {
            call (message);
        }

    private:
        virtual void call (const IMessage*) = 0;
    };

    template <class T, class MessageT>
    class MemberFunctionHandler :   public IFunctionHandler
    {
    public:
        typedef void (T::*MemberFunction)(MessageT*);
        MemberFunctionHandler(T* instance, MemberFunction memfunc)  :   m_instance(instance), m_function(memfunc) {}

        void call(const IMessage* message)
        {
            (m_instance->*m_function)(static_cast<MessageT*>(message));
        }

    private:
        T* m_instance;
        MemberFunction m_function;
    };


    class CMessageHandler
    {
    public:
        ~CMessageHandler();

        void handleMessage(const IMessage* message);

        template <class T, class MessageT>
        void registerMessageFunction(T*, void (T::*memfunc)(MessageT*));

    private:
        typedef QMap<CTypeInfo, IFunctionHandler*> TFunctionHandlerMap;
        TFunctionHandlerMap m_messagehandler;
    };

    template <class T, class MessageT>
    void CMessageHandler::registerMessageFunction(T* obj, void (T::*memfunc)(MessageT*))
    {
        CTypeInfo typeinfo = CTypeInfo(typeid(MessageT));
        m_messagehandler[typeinfo]= new MemberFunctionHandler<T, MessageT>(obj, memfunc);
        CMessageDispatcher::getInstance().registerClass(obj, typeinfo);
    }


} // namespace BlackMisc

#endif // MESSAGE_HANDLER_H
