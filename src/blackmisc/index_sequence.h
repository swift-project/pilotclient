/* Copyright (C) 2014 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_INDEX_SEQUENCE_H
#define BLACKMISC_INDEX_SEQUENCE_H

#include <tuple>
#include <type_traits>

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
