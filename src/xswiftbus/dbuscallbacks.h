/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKSIM_XSWIFTBUS_DBUSASYNCCALLBACKS_H
#define BLACKSIM_XSWIFTBUS_DBUSASYNCCALLBACKS_H

#include <dbus/dbus.h>
#include <functional>

namespace XSwiftBus
{
    //! \cond PRIVATE
    template <typename T>
    class DBusAsyncCallbacks
    {
    public:
        DBusAsyncCallbacks() = default;
        DBusAsyncCallbacks(const std::function<dbus_bool_t(T *)> &add,
                           const std::function<void(T *)> &remove,
                           const std::function<void(T *)> &toggled)
            : m_addHandler(add), m_removeHandler(remove), m_toggledHandler(toggled)
        {}

        static dbus_bool_t add(T *watch, void *refcon)
        {
            return static_cast<DBusAsyncCallbacks *>(refcon)->m_addHandler(watch);
        }

        static void remove(T *watch, void *refcon)
        {
            return static_cast<DBusAsyncCallbacks *>(refcon)->m_removeHandler(watch);
        }

        static void toggled(T *watch, void *refcon)
        {
            return static_cast<DBusAsyncCallbacks *>(refcon)->m_toggledHandler(watch);
        }

    private:
        std::function<dbus_bool_t(T *)> m_addHandler;
        std::function<void(T *)> m_removeHandler;
        std::function<void(T *)> m_toggledHandler;
    };
    //! \endcond

}

#endif // guard
