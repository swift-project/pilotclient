/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_INTEGERSEQUENCE_H
#define BLACKMISC_INTEGERSEQUENCE_H

#include <tuple>
#include <type_traits>
#include <cstddef>

#if ! (defined(Q_CC_GNU) && __GNUC__ <= 4)
//! \private
#define BLACK_HAS_INTEGER_SEQUENCE
#endif

namespace BlackMisc
{

    //! \cond PRIVATE
    namespace Private
    {

#ifdef BLACK_HAS_INTEGER_SEQUENCE
        template <typename T, T... Is>
        using integer_sequence = std::integer_sequence<T, Is...>;
        template <typename T, T C>
        using make_integer_sequence = std::make_integer_sequence<T, C>;
#else // Our own implementation of std::integer_sequence (because not implemented by GCC 4.9)
        template <typename T, T... Is>
        struct integer_sequence
        {
            static const size_t size = sizeof...(Is);
            typedef std::tuple<std::integral_constant<T, Is>...> tuple_type;
        };
        template <typename T, T I, T C, T... Is>
        struct GenSequence
        {
            typedef typename GenSequence<T, I + 1, C, Is..., I>::type type;
        };
        template <typename T, T C, T... Is>
        struct GenSequence<T, C, C, Is...>
        {
            typedef integer_sequence<T, Is...> type;
        };
        template <typename T, T C>
        using make_integer_sequence = typename GenSequence<T, 0, C>::type;
#endif // ! BLACK_HAS_INTEGER_SEQUENCE
        template <size_t... Is>
        using index_sequence = integer_sequence<size_t, Is...>;
        template <size_t C>
        using make_index_sequence = make_integer_sequence<size_t, C>;

        // Remove elements from an index_sequence for which a pack parameter fails to satisfy a given predicate.
        template <typename, typename T, bool...>
        struct MaskSequenceImpl
        {
            using type = T;
        };

        template <size_t I, size_t... Is, bool... Mask, size_t... Js>
        struct MaskSequenceImpl<index_sequence<I, Is...>, index_sequence<Js...>, true, Mask...>
        {
            using type = typename MaskSequenceImpl<index_sequence<Is...>, index_sequence<Js..., I>, Mask...>::type;
        };

        template <size_t I, size_t... Is, bool... Mask, size_t... Js>
        struct MaskSequenceImpl<index_sequence<I, Is...>, index_sequence<Js...>, false, Mask...>
        {
            using type = typename MaskSequenceImpl<index_sequence<Is...>, index_sequence<Js...>, Mask...>::type;
        };

        template <typename Seq, bool... Mask>
        using MaskSequence = typename MaskSequenceImpl<Seq, Private::index_sequence<>, Mask...>::type;

    } // namespace Private
    //! \endcond

} // namespace BlackMisc

#endif // guard
