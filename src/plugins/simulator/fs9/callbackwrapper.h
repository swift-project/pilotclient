/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKSIMPLUGIN_FS9_CALLBACK_WRAPPER_H
#define BLACKSIMPLUGIN_FS9_CALLBACK_WRAPPER_H

#include <QObject>
#include <QPointer>
#include <dplay8.h>
#include <functional>

namespace BlackSimPlugin::Fs9
{
    //! Template, wrapping the C-style DirectPlay handler callback to a class member
    template <class Object, class ReturnType, class Argument1, class Argument2>
    struct CallbackWrapper
    {
        //! Typedef to a MemberFunction
        using MemberFunction = ReturnType (Object::*)(Argument1, Argument2);

        //! Constructor
        CallbackWrapper(Object *obj, MemberFunction memberFunction) : m_object(obj), m_memberFunction(memberFunction)
        {}

        //! FS9 message handler callback
        static ReturnType WINAPI messageHandler(void *userContext, Argument1 arg1, Argument2 arg2)
        {
            CallbackWrapper *_this = static_cast<CallbackWrapper *>(userContext);
            Object *obj = _this->m_object;
            MemberFunction func = _this->m_memberFunction;
            ReturnType result = (obj->*func)(arg1, arg2);
            return result;
        }

    private:
        QPointer<Object> m_object;
        MemberFunction m_memberFunction;
    };
}

#endif // guard
