//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef MESSAGE_DISPATCHER_H
#define MESSAGE_DISPATCHER_H

#include <QQueue>
#include <QMultiMap>

#include "blackmisc/message.h"

#include "blackmisc/context.h"
#include "blackmisc/type_info.h"

namespace BlackMisc
{
    class CMessageHandler;

    class CMessageDispatcher
    {

        // safe singleton declaration
        SINGLETON_CLASS_DECLARATION(CMessageDispatcher)

        CMessageDispatcher() {}

    public:
        virtual ~CMessageDispatcher() {}

        void dispatch ();
        void append ( IMessage* message);

        bool empty ()
        {
            return m_messageQueue.size() == 0;
        }

        template <class T>
        void registerClass(T*, CTypeInfo);

    private:
        typedef QQueue<IMessage*>   TMessageQueue;
        TMessageQueue               m_messageQueue;

        typedef QMultiMap<CTypeInfo, CMessageHandler*> TMessageHandlerMap;
        TMessageHandlerMap          m_messageHander;
    };

    template <class T>
    void CMessageDispatcher::registerClass( T* object, CTypeInfo typeinfo)
    {
        m_messageHander.insert(typeinfo, object);
    }

} // namespace BlackMisc

#endif // MESSAGE_DISPATCHER_H
