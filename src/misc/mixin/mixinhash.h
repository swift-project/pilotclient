// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_MIXIN_MIXINHASH_H
#define SWIFT_MISC_MIXIN_MIXINHASH_H

#include <algorithm>
#include <initializer_list>
#include <type_traits>
#include <utility>

#include <QDBusArgument>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonValueRef>
#include <QList>
#include <QMap>
#include <QString>
#include <QtDebug>
#include <QtGlobal>

#include "misc/inheritancetraits.h"
#include "misc/metaclass.h"

namespace swift::misc
{
    class CEmpty;

    // Needed so that our qHash overload doesn't hide the qHash overloads in the global namespace.
    // This will be safe as long as no global qHash has the same signature as ours.
    // Alternative would be to qualify all our invokations of the global qHash as ::qHash.
    using ::qHash;

    namespace mixin
    {
        /*!
         * CRTP class template from which a derived class can inherit common methods dealing with hashing instances by
         * metaclass.
         *
         * \tparam Derived Must be registered with SWIFT_DECLARE_TUPLE_CONVERSION.
         */
        template <class Derived>
        class HashByMetaClass
        {
        public:
            //! qHash overload, needed for storing value in a QSet.
            friend size_t qHash(const Derived &value, size_t seed = 0) // clazy:exclude=qhash-namespace
            {
                return ::qHash(hashImpl(value), seed);
            }

        private:
            static size_t hashImpl(const Derived &value);

            template <typename T>
            static size_t baseHash(const T *base)
            {
                return qHash(*base);
            }
            static size_t baseHash(const void *);
            static size_t baseHash(const CEmpty *);
        };

        template <class Derived>
        size_t HashByMetaClass<Derived>::hashImpl(const Derived &value)
        {
            size_t hash = baseHash(static_cast<const TBaseOfT<Derived> *>(&value));
            introspect<Derived>().forEachMember([&](auto member) {
                if constexpr (!decltype(member)::has(MetaFlags<DisabledForHashing>()))
                {
                    hash ^= qHash(member.in(value));
                }
            });
            return hash;
        }

        template <class Derived>
        size_t HashByMetaClass<Derived>::baseHash(const void *)
        {
            return 0;
        }

        template <class Derived>
        size_t HashByMetaClass<Derived>::baseHash(const CEmpty *)
        {
            return 0;
        }
    } // namespace mixin
} // namespace swift::misc

#endif // SWIFT_MISC_MIXIN_MIXINHASH_H
