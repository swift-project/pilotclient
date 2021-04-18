/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_MIXIN_MIXINHASH_H
#define BLACKMISC_MIXIN_MIXINHASH_H

#include "blackmisc/metaclass.h"
#include "blackmisc/inheritancetraits.h"

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
#include <algorithm>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace BlackMisc
{
    class CEmpty;

    // Needed so that our qHash overload doesn't hide the qHash overloads in the global namespace.
    // This will be safe as long as no global qHash has the same signature as ours.
    // Alternative would be to qualify all our invokations of the global qHash as ::qHash.
    using ::qHash;

    namespace Mixin
    {
        /*!
         * CRTP class template from which a derived class can inherit common methods dealing with hashing instances by metaclass.
         *
         * \tparam Derived Must be registered with BLACK_DECLARE_TUPLE_CONVERSION.
         */
        template <class Derived>
        class HashByMetaClass
        {
        public:
            //! qHash overload, needed for storing value in a QSet.
            friend uint qHash(const Derived &value, uint seed = 0) // clazy:exclude=qhash-namespace
            {
                return ::qHash(hashImpl(value), seed);
            }

        private:
            static uint hashImpl(const Derived &value)
            {
                uint hash = baseHash(static_cast<const TBaseOfT<Derived> *>(&value));
                introspect<Derived>().forEachMember([ & ](auto member)
                {
                    if constexpr (!decltype(member)::has(MetaFlags<DisabledForHashing>()))
                    {
                        hash ^= qHash(member.in(value));
                    }
                });
                return hash;
            }

            template <typename T> static uint baseHash(const T *base) { return qHash(*base); }
            static uint baseHash(const void *) { return 0; }
            static uint baseHash(const CEmpty *) { return 0; }
        };
    }
} // namespace BlackMisc

#endif // BLACKMISC_DICTIONARY_H

