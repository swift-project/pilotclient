/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_PREDICATES_H
#define BLACKMISC_PREDICATES_H

#include <functional>
#include <QObject>

namespace BlackMisc
{
    namespace Predicates
    {

        namespace Private
        {

            //! \private
            template <class...> struct MemberEqual;

            //! \private
            template <class T, class M, class V> struct MemberEqual<T, M, V>
            {
                M m;
                V v;
                MemberEqual(M m_, V v_) : m(m_), v(v_) {}
                bool operator()(const T &obj) const { return (obj.*m)() == v; }
            };

            //! \private
            template <class T, class M, class V, class... Tail> struct MemberEqual<T, M, V, Tail...>
            {
                MemberEqual<T, M, V> head;
                MemberEqual<T, Tail...> tail;
                MemberEqual(M m, V v, Tail... tail_) : head(m, v), tail(tail_...) {}
                bool operator()(const T &obj) const { return head(obj) && tail(obj); }
            };

            //! \private
            template <class...> struct MemberLess;

            //! \private
            template <class T, class M> struct MemberLess<T, M>
            {
                M m;
                MemberLess(M m_) : m(m_) {}
                bool operator()(const T &a, const T &b) const { return (a.*m)() < (b.*m)(); }
                bool isStable(const T &a, const T &b) const { return (a.*m)() != (b.*m)(); }
            };

            //! \private
            template <class T, class M, class... Tail> struct MemberLess<T, M, Tail...>
            {
                MemberLess<T, M> head;
                MemberLess<T, Tail...> tail;
                MemberLess(M m, Tail... tail_) : head(m), tail(tail_...) {}
                bool operator()(const T &a, const T &b) const { return head.isStable(a, b) ? head(a, b) : tail(a, b); }
            };

        } //namespace Private

        /*!
         * \brief Predicate which tests whether some member functions return some values.
         * \param vs Pairs of { pointer to member function of T, value to compare it against }.
         * \return A unary functor whose operator() which will perform the actual test.
         */
        template <class T, class... Ts>
        typename Private::MemberEqual<T, Ts...> MemberEqual(Ts... vs)
        {
            return typename Private::MemberEqual<T, Ts...>(vs...);
        }

        /*!
         * \brief Predicate which compares the return values of some member functions of two objects.
         * \param vs Pointers to member functions of T.
         * \return A binary functor whose operator() which will perform the actual test.
         */
        template <class T, class... Ts>
        typename Private::MemberLess<T, Ts...> MemberLess(Ts... vs)
        {
            return typename Private::MemberLess<T, Ts...>(vs...);
        }

    } //namespace Predicates

} //namespace BlackMisc

#endif //BLACKMISC_PREDICATES_H
