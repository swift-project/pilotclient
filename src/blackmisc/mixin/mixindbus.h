/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_MIXIN_MIXINDBUS_H
#define BLACKMISC_MIXIN_MIXINDBUS_H

#include "blackmisc/metaclass.h"
#include "blackmisc/inheritancetraits.h"
#include "blackmisc/typetraits.h"
#include <QDBusArgument>
#include <type_traits>

namespace BlackMisc
{
    class CEmpty;

    /*!
     * Tag type signifying overloaded marshalling methods that preserve data at the expense of size.
     */
    class LosslessTag
    {};

    namespace Mixin
    {
        /*!
         * CRTP class template which will generate marshalling operators for a derived class with its own marshalling implementation.
         *
         * \tparam Derived Must implement public methods void marshallToDbus(QDBusArgument &arg) const and void unmarshallFromDbus(const QDBusArgument &arg).
         */
        template <class Derived, class...>
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
        template <class Derived>
        class DBusOperators<Derived, LosslessTag>
        {};

        /*!
         * CRTP class template from which a derived class can inherit common methods dealing with marshalling instances by metaclass.
         *
         * \see BLACKMISC_DECLARE_USING_MIXIN_DBUS
         */
        template <class Derived, class... Tags>
        class DBusByMetaClass : public DBusOperators<Derived, Tags...>
        {
        public:
            //! Marshall without begin/endStructure, for when composed within another object
            void marshallToDbus(QDBusArgument &arg, Tags...) const;

            //! Unmarshall without begin/endStructure, for when composed within another object
            void unmarshallFromDbus(const QDBusArgument &arg, Tags...);

        private:
            const Derived *derived() const;
            Derived *derived();

            template <typename T>
            static void baseMarshall(const T *base, QDBusArgument &arg);
            template <typename T>
            static void baseUnmarshall(T *base, const QDBusArgument &arg);
            static void baseMarshall(const void *, QDBusArgument &);
            static void baseUnmarshall(void *, const QDBusArgument &);
            static void baseMarshall(const CEmpty *, QDBusArgument &);
            static void baseUnmarshall(CEmpty *, const QDBusArgument &);
        };

        template <class Derived, class... Tags>
        void DBusByMetaClass<Derived, Tags...>::marshallToDbus(QDBusArgument &arg, Tags...) const
        {
            baseMarshall(static_cast<const TBaseOfT<Derived> *>(derived()), arg);
            introspect<Derived>().forEachMember([&, this](auto member) {
                if constexpr (!decltype(member)::has(MetaFlags<DisabledForMarshalling>()))
                {
                    const auto &value = member.in(*this->derived());
                    if constexpr (THasMarshallMethods<std::decay_t<decltype(value)>>::value)
                    {
                        if constexpr (member.has(MetaFlags<LosslessMarshalling>()))
                        {
                            value.marshallToDbus(arg, LosslessTag());
                        }
                        else { value.marshallToDbus(arg); }
                    }
                    else { arg << value; }
                }
            });
        }

        template <class Derived, class... Tags>
        void DBusByMetaClass<Derived, Tags...>::unmarshallFromDbus(const QDBusArgument &arg, Tags...)
        {
            baseUnmarshall(static_cast<TBaseOfT<Derived> *>(derived()), arg);
            introspect<Derived>().forEachMember([&, this](auto member) {
                if constexpr (!decltype(member)::has(MetaFlags<DisabledForMarshalling>()))
                {
                    auto &value = member.in(*this->derived());
                    if constexpr (THasMarshallMethods<std::decay_t<decltype(value)>>::value)
                    {
                        if constexpr (member.has(MetaFlags<LosslessMarshalling>()))
                        {
                            value.unmarshallFromDbus(arg, LosslessTag());
                        }
                        else { value.unmarshallFromDbus(arg); }
                    }
                    else { arg >> value; }
                }
            });
        }

        template <class Derived, class... Tags>
        const Derived *DBusByMetaClass<Derived, Tags...>::derived() const
        {
            return static_cast<const Derived *>(this);
        }

        template <class Derived, class... Tags>
        Derived *DBusByMetaClass<Derived, Tags...>::derived()
        {
            return static_cast<Derived *>(this);
        }

        template <class Derived, class... Tags>
        template <typename T>
        void DBusByMetaClass<Derived, Tags...>::baseMarshall(const T *base, QDBusArgument &arg)
        {
            base->marshallToDbus(arg, Tags()...);
        }

        template <class Derived, class... Tags>
        template <typename T>
        void DBusByMetaClass<Derived, Tags...>::baseUnmarshall(T *base, const QDBusArgument &arg)
        {
            base->unmarshallFromDbus(arg, Tags()...);
        }

        template <class Derived, class... Tags>
        void DBusByMetaClass<Derived, Tags...>::baseMarshall(const void *, QDBusArgument &)
        {}

        template <class Derived, class... Tags>
        void DBusByMetaClass<Derived, Tags...>::baseUnmarshall(void *, const QDBusArgument &)
        {}

        template <class Derived, class... Tags>
        void DBusByMetaClass<Derived, Tags...>::baseMarshall(const CEmpty *, QDBusArgument &)
        {}

        template <class Derived, class... Tags>
        void DBusByMetaClass<Derived, Tags...>::baseUnmarshall(CEmpty *, const QDBusArgument &)
        {}

        // *INDENT-OFF*
        /*!
         * When a derived class and a base class both inherit from Mixin::DBusByTuple,
         * the derived class uses this macro to disambiguate the inherited members.
         */
#define BLACKMISC_DECLARE_USING_MIXIN_DBUS(DERIVED, ...)                                                    \
    using ::BlackMisc::Mixin::DBusByMetaClass<DERIVED BLACK_TRAILING_VA_ARGS(__VA_ARGS__)>::marshallToDbus; \
    using ::BlackMisc::Mixin::DBusByMetaClass<DERIVED BLACK_TRAILING_VA_ARGS(__VA_ARGS__)>::unmarshallFromDbus;
        // *INDENT-ON*

    } // Mixin
} // BlackMisc

#endif // guard
