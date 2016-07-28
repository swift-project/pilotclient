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
        template <size_t I, typename T>
        struct tuple_part
        {
            constexpr tuple_part() {}
            constexpr tuple_part(const T &element) : m_element(element) {}
            T m_element;
        };

        template <typename... Ts>
        struct tuple_impl;
        template <typename... Ts, size_t... Is>
        struct tuple_impl<index_sequence<Is...>, Ts...> : public tuple_part<Is, Ts>...
        {
            constexpr tuple_impl() {}
            constexpr tuple_impl(const Ts &... vs) : tuple_part<Is, Ts>(vs)... {}
        };

        template <typename... Ts>
        struct tuple : public tuple_impl<make_index_sequence<sizeof...(Ts)>, Ts...>
        {
            constexpr tuple() {}
            constexpr tuple(const Ts &... vs) : tuple_impl<make_index_sequence<sizeof...(Ts)>, Ts...>(vs...) {}
            constexpr static size_t c_size = sizeof...(Ts);
        };

        template <size_t I, typename T>
        constexpr decltype(auto) get_impl(tuple_part<I, T> &part) { return (part.m_element); }
        template <size_t I, typename T>
        constexpr decltype(auto) get_impl(tuple_part<I, T> &&part) { return std::move(part.m_element); }
        template <size_t I, typename T>
        constexpr decltype(auto) get_impl(const tuple_part<I, T> &part) { return (part.m_element); }
        template <size_t I, typename T>
        constexpr decltype(auto) get_impl(const tuple_part<I, T> &&part) { return std::move(part.m_element); }

        template <size_t I, typename T>
        constexpr decltype(auto) get(T &&tuple) { return get_impl<I>(std::forward<T>(tuple)); }

        template <typename T>
        struct tuple_size : public std::integral_constant<size_t, T::c_size> {};

        template <typename... Ts>
        constexpr auto make_tuple(Ts &&... vs) { return tuple<std::decay_t<Ts>...>(std::forward<Ts>(vs)...); }
#endif // ! BLACK_HAS_CONSTEXPR_STDLIB
    }
}

//! \endcond

#endif
