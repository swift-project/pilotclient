/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

/*!
    \file
*/

#ifndef BLACKMISC_PREDICATES_H
#define BLACKMISC_PREDICATES_H

#include "indexsequence.h"
#include <QObject>
#include <functional>
#include <algorithm>

namespace BlackMisc
{
    class CPropertyIndexVariantMap;

    namespace Predicates
    {

        namespace Private
        {

            //! \private
            template <class...> struct MemberEqual;

            //! \private
            template <class M, class V> struct MemberEqual<M, V>
            {
                M m;
                V v;
                MemberEqual(M m_, V v_) : m(m_), v(v_) {}
                template <class T> bool operator()(const T &obj) const { return (obj.*m)() == v; }
            };

            //! \private
            template <class M, class V, class... Tail> struct MemberEqual<M, V, Tail...>
            {
                MemberEqual<M, V> head;
                MemberEqual<Tail...> tail;
                MemberEqual(M m, V v, Tail... tail_) : head(m, v), tail(tail_...) {}
                template <class T> bool operator()(const T &obj) const { return head(obj) && tail(obj); }
            };

            //! \private
            template <class...> struct MemberLess;

            //! \private
            template <class M> struct MemberLess<M>
            {
                M m;
                MemberLess(M m_) : m(m_) {}
                template <class T> bool operator()(const T &a, const T &b) const { return (a.*m)() < (b.*m)(); }
                template <class T> bool isStable(const T &a, const T &b) const { return (a.*m)() != (b.*m)(); }
            };

            //! \private
            template <class M, class... Tail> struct MemberLess<M, Tail...>
            {
                MemberLess<M> head;
                MemberLess<Tail...> tail;
                MemberLess(M m, Tail... tail_) : head(m), tail(tail_...) {}
                template <class T> bool operator()(const T &a, const T &b) const { return head.isStable(a, b) ? head(a, b) : tail(a, b); }
            };

            //! \private
            template <class T> struct Equals
            {
                const T m_value;
                template <class U> Equals(U &&value, int dummy) : m_value(std::forward<U>(value)) { Q_UNUSED(dummy); }
                template <class U> bool operator ()(const U &other) const { return other == m_value; }
            };

            //! \private
            struct Matches
            {
                const CPropertyIndexVariantMap &m_map;
                Matches(const CPropertyIndexVariantMap &map) : m_map(map) {}
                template <class T> bool operator()(const T &value) const;
            };

        } //namespace Private

        /*!
         * \brief Predicate which tests whether some member functions return some values.
         * \param vs Pairs of { pointer to member function of T, value to compare it against }.
         * \return A unary functor whose operator() which will perform the actual test.
         */
        template <class... Ts>
        typename Private::MemberEqual<Ts...> MemberEqual(Ts... vs)
        {
            return typename Private::MemberEqual<Ts...>(vs...);
        }

        /*!
         * \brief Predicate which compares the return values of some member functions of two objects.
         * \param vs Pointers to member functions of T.
         * \return A binary functor whose operator() which will perform the actual test.
         */
        template <class... Ts>
        typename Private::MemberLess<Ts...> MemberLess(Ts... vs)
        {
            return typename Private::MemberLess<Ts...>(vs...);
        }

        /*!
         * Returns a function object that returns the value returned by one of it's argument member functions.
         */
        template <class T, class R>
        auto MemberTransform(R(T::*memberFunc)() const)
        {
            return [memberFunc](const T &object) { return (object.*memberFunc)(); };
        }

        /*!
         * Returns a predicate that returns true if the isValid() method of the value returned from one of its member functions returns true.
         */
        template <class T, class R>
        auto MemberValid(R(T::*memberFunc)() const)
        {
            return [memberFunc](const T &object) { return (object.*memberFunc)().isValid(); };
        }

        /*!
         * Returns a predicate that returns true if the value returned by its argument's member function can be found in a captured container.
         * \warning The container is captured by reference, so be careful that it remains valid for the lifetime of the predicate.
         */
        template <class T, class R, class C>
        auto MemberIsAnyOf(R(T::*memberFunc)() const, const C &container)
        {
            return [memberFunc, &container](const T &object) { return container.contains((object.*memberFunc)()); };
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
         * Returns a predicate that returns true if its argument matches a captured CPropertyIndexVariantMap.
         */
        inline auto Matches(const CPropertyIndexVariantMap &map) -> Private::Matches
        {
            return { map };
        }

    } //namespace Predicates

} //namespace BlackMisc

#endif //BLACKMISC_PREDICATES_H
