// SPDX-FileCopyrightText: Copyright (C) 2021 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_TUPLE_H
#define SWIFT_MISC_TUPLE_H

#include <stddef.h>

#include <utility>

//! \cond
namespace swift::misc::private_ns
{
    /*
     * Own minimal implementation of std::tuple with very limited functionality.
     * Only used to reduce compile time for the metaclass system.
     */

    template <size_t I, typename T>
    struct tuple_part
    {
        T value;
    };

    template <typename...>
    struct tuple_impl;

    template <size_t... Is, typename... Ts>
    struct tuple_impl<std::index_sequence<Is...>, Ts...> : public tuple_part<Is, Ts>...
    {
        template <typename F>
        void for_each(F &&visitor) const
        {
            (static_cast<void>(visitor(static_cast<const tuple_part<Is, Ts> &>(*this).value)), ...);
        }
    };

    template <typename... Ts>
    struct tuple : public tuple_impl<std::make_index_sequence<sizeof...(Ts)>, Ts...>
    {};
} // namespace swift::misc::private_ns
//! \endcond

#endif
