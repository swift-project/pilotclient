// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIM_XSWIFTBUS_DBUSASYNCCALLBACKS_H
#define SWIFT_SIM_XSWIFTBUS_DBUSASYNCCALLBACKS_H

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
