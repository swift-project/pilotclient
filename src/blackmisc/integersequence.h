/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_INTEGERSEQUENCE_H
#define BLACKMISC_INTEGERSEQUENCE_H

#include <tuple>
#include <type_traits>
#include <cstddef>

namespace BlackMisc::Private
{
    //! \cond PRIVATE

    // Remove elements from an index_sequence for which a pack parameter fails to satisfy a given predicate.
    template <typename, typename T, bool...>
    struct MaskSequenceImpl
    {
        using type = T;
    };

    template <size_t I, size_t... Is, bool... Mask, size_t... Js>
    struct MaskSequenceImpl<std::index_sequence<I, Is...>, std::index_sequence<Js...>, true, Mask...>
    {
        using type = typename MaskSequenceImpl<std::index_sequence<Is...>, std::index_sequence<Js..., I>, Mask...>::type;
    };

    template <size_t I, size_t... Is, bool... Mask, size_t... Js>
    struct MaskSequenceImpl<std::index_sequence<I, Is...>, std::index_sequence<Js...>, false, Mask...>
    {
        using type = typename MaskSequenceImpl<std::index_sequence<Is...>, std::index_sequence<Js...>, Mask...>::type;
    };

    template <typename Seq, bool... Mask>
    using MaskSequence = typename MaskSequenceImpl<Seq, std::index_sequence<>, Mask...>::type;

    //! \endcond
} // namespace BlackMisc

#endif // guard
