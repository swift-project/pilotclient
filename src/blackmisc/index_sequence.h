/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_INDEX_SEQUENCE_H
#define BLACKMISC_INDEX_SEQUENCE_H

#include <tuple>
#include <type_traits>
#include <cstddef>

namespace BlackMisc
{
    namespace Private
    {

        // Inhibit doxygen warnings about missing documentation
        //! \cond PRIVATE

        // Our own implementation of std::index_sequence (because not implemented by MSVC2013)
        template <size_t... Is>
        struct index_sequence
        {
            static const size_t size = sizeof...(Is);
            typedef std::tuple<std::integral_constant<size_t, Is>...> tuple_type;
        };
        template <size_t I, size_t C, size_t... Is>
        struct GenSequence
        {
            typedef typename GenSequence<I + 1, C, Is..., I>::type type;
        };
        template <size_t C, size_t... Is>
        struct GenSequence<C, C, Is...>
        {
            typedef index_sequence<Is...> type;
        };
        template <size_t C>
        using make_index_sequence = typename GenSequence<0, C>::type;

        //! \endcond

    } // namespace Private

} // namespace BlackMisc

#endif // guard
