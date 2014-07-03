/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_PREDICATES_H
#define BLACKMISC_PREDICATES_H

#include "index_sequence.h"
#include <QObject>
#include <functional>
#include <algorithm>

namespace BlackMisc
{
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
            template <class T, class M> struct MemberTransform
            {
                M m;
                MemberTransform(M m_) : m(m_) {}
                auto operator()(const T &v) const -> decltype((v.*std::declval<M>())()) { return (v.*m)(); }
            };

            //! \private
            template <class T, class M> struct MemberValid
            {
                M m;
                MemberValid(M m_) : m(m_) {}
                bool operator()(const T &v) const { return (v.*m)().isValid(); }
            };

            //! \private
            template <class T, class M, class C> struct MemberIsAnyOf
            {
                M m;
                const C &c;
                MemberIsAnyOf(M m_, const C &c_) : m(m_), c(c_) {}
                bool operator()(const T &v) const { return c.contains((v.*m)()); }
            };

            //! \private
            template <class T> struct Equals
            {
                const T m_value;
                template <class U> Equals(U &&value) : m_value(std::forward<U>(value)) {}
                template <class U> bool operator ()(const U &other) const { return other == m_value; }
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
         * Transformation function object which returns the value returned by one of it's argument member functions.
         *
         * A lambda would usually be easier, but it is difficult to directly return a lambda from a function
         * without C++14 deduced return types.
         */
        template <class T, class R>
        auto MemberTransform(R(T::*memberFunc)() const) -> Private::MemberTransform<T, decltype(memberFunc)>
        {
            return { memberFunc };
        }

        /*!
         * Predicate which is true if the isValid() method of the value returned from one of its member functions returns true.
         *
         * A lambda would usually be easier, but it is difficult to directly return a lambda from a function
         * without C++14 deduced return types.
         */
        template <class T, class R>
        auto MemberValid(R(T::*memberFunc)() const) -> Private::MemberValid<T, decltype(memberFunc)>
        {
            return { memberFunc };
        }

        /*!
         * Predicate which is true if the value returned by its argument's member function can be found in a captured container.
         *
         * A lambda would usually be easier, but it is difficult to directly return a lambda from a function
         * without C++14 deduced return types.
         *
         * \warning The container is captured by reference, so be careful that it remains valid for the lifetime of the predicate.
         */
        template <class T, class R, class C>
        auto MemberIsAnyOf(R(T::*memberFunc)() const, const C &container) -> Private::MemberIsAnyOf<T, decltype(memberFunc), C>
        {
            return { memberFunc, container };
        }

        /*!
         * Predicate which is true if its argument compares equal with another, captured value.
         *
         * A lambda would usually be easier, but it is difficult to directly return a lambda from a function
         * without C++14 deduced return types. It is also a generic function object, which is only possible
         * with C++14 generic lambdas.
         */
        template <class T>
        auto Equals(T &&value) -> Private::Equals<typename std::decay<T>::type>
        {
            return { std::forward<T>(value) };
        }

    } //namespace Predicates

} //namespace BlackMisc

#endif //BLACKMISC_PREDICATES_H
