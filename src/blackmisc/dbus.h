/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DBUS_H
#define BLACKMISC_DBUS_H

#include "blackmisc/tuple.h"
#include "blackmisc/inheritance_traits.h"
#include <QDBusArgument>
#include <type_traits>

namespace BlackMisc
{
    namespace Mixin
    {

        /*!
         * CRTP class template which will generate marshalling operators for a derived class with its own marshalling implementation.
         *
         * \tparam Derived Must implement public methods void marshallToDbus(QDBusArgument &arg) const and void unmarshallFromDbus(const QDBusArgument &arg).
         */
        template <class Derived>
        class DBusOperators
        {
        public:
            //! Unmarshalling operator >>, DBus to object
            friend const QDBusArgument &operator>>(const QDBusArgument &arg, Derived &obj)
            {
                arg.beginStructure();
                obj.unmarshallFromDbus(arg);
                arg.endStructure();
                return arg;
            }

            //! Marshalling operator <<, object to DBus
            friend QDBusArgument &operator<<(QDBusArgument &arg, const Derived &obj)
            {
                arg.beginStructure();
                obj.marshallToDbus(arg);
                arg.endStructure();
                return arg;
            }
        };

        /*!
         * CRTP class template from which a derived class can inherit common methods dealing with marshalling instances by metatuple.
         *
         * \tparam Derived Must be registered with BLACK_DECLARE_TUPLE_CONVERSION.
         *
         * \see BLACKMISC_DECLARE_USING_MIXIN_DBUS
         */
        template <class Derived>
        class DBusByTuple : public DBusOperators<Derived>, private Private::EncapsulationBreaker
        {
        public:
            //! Marshall without begin/endStructure, for when composed within another object
            void marshallToDbus(QDBusArgument &arg) const
            {
                baseMarshall(static_cast<const BaseOfT<Derived> *>(derived()), arg);
                using BlackMisc::operator<<;
                arg << Private::EncapsulationBreaker::toMetaTuple(*derived());
            }

            //! Unmarshall without begin/endStructure, for when composed within another object
            void unmarshallFromDbus(const QDBusArgument &arg)
            {
                baseUnmarshall(static_cast<BaseOfT<Derived> *>(derived()), arg);
                using BlackMisc::operator>>;
                arg >> Private::EncapsulationBreaker::toMetaTuple(*derived());
            }

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }

            template <typename T> static void baseMarshall(const T *base, QDBusArgument &arg) { base->marshallToDbus(arg); }
            template <typename T> static void baseUnmarshall(T *base, const QDBusArgument &arg) { base->unmarshallFromDbus(arg); }
            static void baseMarshall(const void *, QDBusArgument &) {}
            static void baseUnmarshall(void *, const QDBusArgument &) {}
        };

        /*!
         * When a derived class and a base class both inherit from Mixin::DBusByTuple,
         * the derived class uses this macro to disambiguate the inherited members.
         */
#       define BLACKMISC_DECLARE_USING_MIXIN_DBUS(DERIVED)                          \
            using ::BlackMisc::Mixin::DBusByTuple<DERIVED>::marshallToDbus;         \
            using ::BlackMisc::Mixin::DBusByTuple<DERIVED>::unmarshallFromDbus;

    } // Mixin
} // BlackMisc

/*!
 * Non-member non-friend operator for streaming enums to QDBusArgument.
 *
 * \param argument
 * \param enumType
 * \return
 * \remarks Currently outside namespace for OSX build, see https://dev.vatsim-germany.org/issues/184
 */
template <class ENUM> typename std::enable_if<std::is_enum<ENUM>::value, QDBusArgument>::type const &
operator>>(const QDBusArgument &argument, ENUM &enumType)
{
    uint e;
    argument >> e;
    enumType = static_cast<ENUM>(e);
    return argument;
}

#endif // guard
