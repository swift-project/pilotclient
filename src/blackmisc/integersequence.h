// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
