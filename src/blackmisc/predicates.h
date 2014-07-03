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

    } //namespace Predicates

} //namespace BlackMisc

#endif //BLACKMISC_PREDICATES_H
