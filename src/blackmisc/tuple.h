/* Copyright (C) 2021
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_TUPLE_H
#define BLACKMISC_TUPLE_H

#include <utility>
#include <stddef.h>

//! \cond
namespace BlackMisc::Private
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
}
//! \endcond

#endif
