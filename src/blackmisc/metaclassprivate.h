/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_METACLASSPRIVATE_H
#define BLACKMISC_METACLASSPRIVATE_H

#include "integersequence.h"
#include <type_traits>
#include <functional>
#include <QString>

//! \cond PRIVATE

// GCC 4.9 doesn't utilize constexpr in the standard library
#if ! (defined(Q_CC_GNU) && __GNUC__ <= 4)
#define BLACK_HAS_CONSTEXPR_STDLIB
#endif

// Work around MinGW problem with combination of constexpr and extern template
#if defined(Q_OS_WIN) && defined(Q_CC_GNU)
#define BLACK_NO_EXPORT_CONSTEXPR constexpr inline __attribute__((gnu_inline))
#else
#define BLACK_NO_EXPORT_CONSTEXPR constexpr
#endif

// MSVC, GCC, Clang all have non-standard extensions for skipping trailing
// commas in variadic macros, but the MSVC extension differs from the others.
#ifdef Q_CC_MSVC
#define BLACK_TRAILING_VA_ARGS(...) ,__VA_ARGS__
#else
#define BLACK_TRAILING_VA_ARGS(...) ,##__VA_ARGS__
#endif

// Work around MSVC constexpr bug
// https://connect.microsoft.com/VisualStudio/feedback/details/2028721
#ifdef Q_CC_MSVC
#define BLACK_DECLTYPE_AUTO auto
#define BLACK_TRAILING_RETURN(EXPR) -> decltype(EXPR)
#else
#define BLACK_DECLTYPE_AUTO decltype(auto)
#define BLACK_TRAILING_RETURN(EXPR)
#endif

namespace BlackMisc
{
    namespace Private
    {
#ifdef BLACK_HAS_CONSTEXPR_STDLIB
        using std::tuple;
        using std::tuple_size;
        using std::get;
        using std::make_tuple;
#else // Own implementation of tuple, because the one in GCC 4.9 is not constexpr.
        template <size_t I, typename Head, typename... Tail>
        struct conslist;

        template <size_t I, typename Head>
        struct conslist<I, Head>
        {
            constexpr conslist() {}
            constexpr conslist(const Head &v) : m_obj(v) {}
            constexpr const Head &get(std::integral_constant<size_t, I>) const { return m_obj; }
            Head m_obj;
        };

        template <size_t I, typename Head, typename... Tail>
        struct conslist : public conslist<I + 1, Tail...>
        {
            constexpr conslist() {}
            constexpr conslist(const Head &v, const Tail &... vs) : conslist<I + 1, Tail...>(vs...), m_obj(v) {}
            constexpr const Head &get(std::integral_constant<size_t, I>) const { return m_obj; }
            using conslist<I + 1, Tail...>::get;
            Head m_obj;
        };

        template <typename... Ts>
        struct tuple
        {
            constexpr tuple() {}
            constexpr tuple(const Ts &... vs) : m_conslist(vs...) {}
            conslist<0, Ts...> m_conslist;
            constexpr static size_t c_size = sizeof...(Ts);
        };

        template <typename T>
        struct tuple_size : public std::integral_constant<size_t, T::c_size> {};

        template <size_t I, typename T>
        constexpr decltype(auto) get(T &&tuple) { return std::forward<T>(tuple).m_conslist.get(std::integral_constant<size_t, I>()); }

        template <typename... Ts>
        constexpr auto make_tuple(Ts &&... vs) { return tuple<std::decay_t<Ts>...>(std::forward<Ts>(vs)...); }
#endif // ! BLACK_HAS_CONSTEXPR_STDLIB

        // Helper for case insensitive comparisons.
        template <typename T>
        struct CaseInsensitiveWrapper
        {
            const T &m_ref;

            explicit CaseInsensitiveWrapper(const T &ref) : m_ref(ref) {}

            friend int compare(CaseInsensitiveWrapper a, CaseInsensitiveWrapper b)
            {
                return a.m_ref.compare(b.m_ref, Qt::CaseInsensitive);
            }
            friend bool operator ==(CaseInsensitiveWrapper a, CaseInsensitiveWrapper b)
            {
                return compare(a, b) == 0;
            }
            friend bool operator <(CaseInsensitiveWrapper a, CaseInsensitiveWrapper b)
            {
                return compare(a, b) < 0;
            }
        };
        template <typename T>
        auto caseAwareWrap(std::false_type, const T &value)
        {
            return std::cref(value);
        }
        template <typename T>
        auto caseAwareWrap(std::true_type, const T &value)
        {
            return CaseInsensitiveWrapper<T>(value);
        }
    }
}

//! \endcond

#endif
