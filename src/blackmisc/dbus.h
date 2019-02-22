/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DBUS_H
#define BLACKMISC_DBUS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/inheritancetraits.h"
#include "blackmisc/typetraits.h"
#include <QDBusArgument>
#include <type_traits>

namespace BlackMisc
{
    class CEmpty;

    namespace Private
    {
        //! \cond PRIVATE
        template <class T, std::enable_if_t<THasMarshallMethods<T>::value, int> = 0>
        void marshallMember(QDBusArgument &arg, const T &value) { value.marshallToDbus(arg); }

        template <class T, std::enable_if_t<!THasMarshallMethods<T>::value, int> = 0>
        void marshallMember(QDBusArgument &arg, const T &value) { arg << value; }

        template <class T, std::enable_if_t<THasMarshallMethods<T>::value, int> = 0>
        void unmarshallMember(const QDBusArgument &arg, T &value) { value.unmarshallFromDbus(arg); }

        template <class T, std::enable_if_t<!THasMarshallMethods<T>::value, int> = 0>
        void unmarshallMember(const QDBusArgument &arg, T &value) { arg >> value; }
        //! \endcond
    }

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
         * CRTP class template from which a derived class can inherit common methods dealing with marshalling instances by metaclass.
         *
         * \see BLACKMISC_DECLARE_USING_MIXIN_DBUS
         */
        template <class Derived>
        class DBusByMetaClass : public DBusOperators<Derived>
        {
        public:
            //! Marshall without begin/endStructure, for when composed within another object
            void marshallToDbus(QDBusArgument &arg) const
            {
                baseMarshall(static_cast<const TBaseOfT<Derived> *>(derived()), arg);
                auto meta = introspect<Derived>().without(MetaFlags<DisabledForMarshalling>());
                meta.forEachMember([ &, this ](auto member) { Private::marshallMember(arg, member.in(*this->derived())); });
            }

            //! Unmarshall without begin/endStructure, for when composed within another object
            void unmarshallFromDbus(const QDBusArgument &arg)
            {
                baseUnmarshall(static_cast<TBaseOfT<Derived> *>(derived()), arg);
                auto meta = introspect<Derived>().without(MetaFlags<DisabledForMarshalling>());
                meta.forEachMember([ &, this ](auto member) { Private::unmarshallMember(arg, member.in(*this->derived())); });
            }

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }

            template <typename T> static void baseMarshall(const T *base, QDBusArgument &arg) { base->marshallToDbus(arg); }
            template <typename T> static void baseUnmarshall(T *base, const QDBusArgument &arg) { base->unmarshallFromDbus(arg); }
            static void baseMarshall(const void *, QDBusArgument &) {}
            static void baseUnmarshall(void *, const QDBusArgument &) {}
            static void baseMarshall(const CEmpty *, QDBusArgument &) {}
            static void baseUnmarshall(CEmpty *, const QDBusArgument &) {}
        };

        /*!
         * When a derived class and a base class both inherit from Mixin::DBusByTuple,
         * the derived class uses this macro to disambiguate the inherited members.
         */
#       define BLACKMISC_DECLARE_USING_MIXIN_DBUS(DERIVED)                          \
            using ::BlackMisc::Mixin::DBusByMetaClass<DERIVED>::marshallToDbus;     \
            using ::BlackMisc::Mixin::DBusByMetaClass<DERIVED>::unmarshallFromDbus;

    } // Mixin
} // BlackMisc

/*!
 * Operator for streaming enums to QDBusArgument.
 */
template <class E, std::enable_if_t<std::is_enum<E>::value, int> = 0>
QDBusArgument &operator <<(QDBusArgument &arg, const E &value)
{
    arg.beginStructure();
    arg << static_cast<int>(value);
    arg.endStructure();
    return arg;
}

/*!
 * Operator for streaming enums from QDBusArgument.
 */
template <class E, std::enable_if_t<std::is_enum<E>::value, int> = 0>
const QDBusArgument &operator >>(const QDBusArgument &arg, E &value)
{
    int temp;
    arg.beginStructure();
    arg >> temp;
    arg.endStructure();
    value = static_cast<E>(temp);
    return arg;
}

/*!
 * Operator for streaming pairs to QDBusArgument.
 */
template <class A, class B>
QDBusArgument &operator <<(QDBusArgument &arg, const std::pair<A, B> &pair)
{
    arg.beginStructure();
    arg << pair.first << pair.second;
    arg.endStructure();
    return arg;
}

/*!
 * Operator for streaming pairs from QDBusArgument.
 */
template <class A, class B>
const QDBusArgument &operator >>(const QDBusArgument &arg, std::pair<A, B> &pair)
{
    arg.beginStructure();
    arg >> pair.first >> pair.second;
    arg.endStructure();
    return arg;
}

/*!
 * Non member non-friend streaming for QPixmap
 */
const QDBusArgument &operator>>(const QDBusArgument &argument, QPixmap &pixmap);

/*!
 * Non member non-friend streaming for QPixmap
 */
QDBusArgument &operator<<(QDBusArgument &argument, const QPixmap &pixmap);

//! Windows: prevents unloading of QtDBus shared library until the process is terminated.
//! QtDBus must have been loaded already by the calling process.
//! Does nothing on non-Windows platforms.
BLACKMISC_EXPORT void preventQtDBusDllUnload();

#endif // guard
