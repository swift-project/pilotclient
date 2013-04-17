//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef BLACKMISC_MESSAGE_DISPATCHER_H
#define BLACKMISC_MESSAGE_DISPATCHER_H

#include "blackmisc/message.h"
#include "blackmisc/context.h"
#include "blackmisc/type_info.h"
#include <QQueue>
#include <QMultiMap>

namespace BlackMisc
{
    class CMessageHandler;

    class CMessageDispatcher : public QObject
    {
        Q_OBJECT

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
        void registerClass(T* object, CTypeInfo typeinfo)
        {
            m_messageHander.insert(typeinfo, object);
        }

    private:
        typedef QQueue<IMessage*>   TMessageQueue;
        TMessageQueue               m_messageQueue;

        typedef QMultiMap<CTypeInfo, CMessageHandler*> TMessageHandlerMap;
        TMessageHandlerMap          m_messageHander;
    };

} // namespace BlackMisc

#endif // BLACKMISC_MESSAGE_DISPATCHER_H
