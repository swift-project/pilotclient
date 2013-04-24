//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef BLACKMISC_MESSAGE_FACTORY_H
#define BLACKMISC_MESSAGE_FACTORY_H

#include <QHash>
#include "blackmisc/message.h"
#include "blackmisc/gui_messages.h"
#include "blackmisc/debug.h"
#include "blackmisc/context.h"

#define REGISTER_MESSAGE(classname, msgName) new MessageCreatorImpl<classname>(#msgName);


namespace BlackMisc
{
    class IMessageCreator
    {
    public:
        IMessageCreator(const QString& messageID);

        virtual IMessage* create() = 0;
    };

    template <class T>
    class MessageCreatorImpl : public IMessageCreator
    {
    public:
        MessageCreatorImpl(const QString& messageID) : IMessageCreator(messageID) {}
        virtual IMessage* create() { return new T; }
    };

    class CMessageFactory : public QObject
    {
        Q_OBJECT

        // safe singleton declaration
        SINGLETON_CLASS_DECLARATION(CMessageFactory)

        CMessageFactory() { }
    public:

        virtual ~CMessageFactory();

        IMessage* create (const QString &messageID);

        void registerMessage(const QString &messageID, IMessageCreator* creator);
        static void registerMessages();

    private:

        typedef QHash<QString, IMessageCreator*> TMessageCreatorHash;
        TMessageCreatorHash m_creators;
    };

} // namespace BlackMisc

#endif // BLACKMISC_MESSAGE_FACTORY_H
