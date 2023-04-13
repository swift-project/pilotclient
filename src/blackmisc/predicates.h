/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PREDICATES_H
#define BLACKMISC_PREDICATES_H

#include "blackmisc/algorithm.h"
#include <functional>
#include <algorithm>

namespace BlackMisc
{
    class CPropertyIndexVariantMap;

    namespace Predicates
    {
        /*!
         * Predicate which tests whether some member functions return some values.
         * \param vs Pairs of { pointer to member function of T, value to compare it against }.
         * \return A unary functor whose operator() which will perform the actual test.
         */
        template <class... Ts>
        auto MemberEqual(Ts... vs)
        {
            return [vs...](const auto &object) {
                bool equal = true;
                tupleForEachPair(std::make_tuple(vs...), [&](auto member, const auto &value) { equal = equal && std::invoke(member, object) == value; });
                return equal;
            };
        }

        /*!
         * Predicate which compares the return values of some member functions of two objects.
         * \param vs Pointers to member functions of T.
         * \return A binary functor whose operator() which will perform the actual test.
         */
        template <class... Ts>
        auto MemberLess(Ts... vs)
        {
            return [vs...](const auto &a, const auto &b) {
                return std::forward_as_tuple(std::invoke(vs, a)...) < std::forward_as_tuple(std::invoke(vs, b)...);
            };
        }

        /*!
         * Returns a function object that returns the value returned by one of it's argument member functions.
         */
        template <class T>
        auto MemberTransform(T memberFunc)
        {
            return [memberFunc](const auto &object) { return std::invoke(memberFunc, object); };
        }

        /*!
         * Returns a predicate that returns true if the isValid() method of the value returned from one of its member functions returns true.
         */
        template <class T>
        auto MemberValid(T memberFunc)
        {
            return [memberFunc](const auto &object) { return std::invoke(memberFunc, object).isValid(); };
        }

        /*!
         * Returns a predicate that returns true if the value returned by its argument's member function can be found in a captured container.
         * \warning The container is captured by reference, so be careful that it remains valid for the lifetime of the predicate.
         */
        template <class T, class C>
        auto MemberIsAnyOf(T memberFunc, const C &container)
        {
            return [memberFunc, &container](const auto &object) { return container.contains(std::invoke(memberFunc, object)); };
        }

        /*!
         * Returns a predicate that returns true if its argument compares equal with another, captured value.
         */
        template <class T>
        auto Equals(T &&value)
        {
            return [value = std::forward<T>(value)](const T &object) { return object == value; };
        }

        /*!
         * Returns a predicate that returns true if its arguments compare equal to each other, considering only the captured members.
         */
        template <class... Ts>
        auto EqualsByMembers(Ts... vs)
        {
            return [vs...](const auto &a, const auto &b) {
                return ((std::invoke(vs, a) == std::invoke(vs, b)) && ...);
            };
        }

    } // namespace Predicates
} // namespace BlackMisc

#endif // guard
