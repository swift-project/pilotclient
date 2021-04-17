/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_INVOKE_H
#define BLACKMISC_INVOKE_H

#include "blackmisc/typetraits.h"
#include "blackmisc/integersequence.h"
#include "blackmisc/promise.h"
#include <QMetaObject>
#include <QtGlobal>
#include <QThread>
#include <QTimer>
#include <tuple>

namespace BlackMisc
{
    //! \cond PRIVATE
    namespace Private
    {

        // Like invoke() but ignores the first argument if callable is not a member function. For uniform calling of callables with slot semantics.
        template <typename F, typename T, typename U, size_t... Is>
        decltype(auto) invokeSlotImpl(F ptr, T *object, U tuple, std::index_sequence<Is...>, std::true_type)
        {
            Q_UNUSED(tuple); // in case the pack expansion is empty
            return (object->*ptr)(std::forward<std::tuple_element_t<Is, U>>(std::get<Is>(tuple))...);
        }
        template <typename F, typename T, typename U, size_t... Is>
        decltype(auto) invokeSlotImpl(F &&func, T *, U tuple, std::index_sequence<Is...>, std::false_type)
        {
            Q_UNUSED(tuple); // in case the pack expansion is empty
            return std::forward<F>(func)(std::forward<std::tuple_element_t<Is, U>>(std::get<Is>(tuple))...);
        }
        template <typename F, typename T, typename... Ts>
        decltype(auto) invokeSlot(F &&func, T *object, Ts &&... args)
        {
            using seq = MaskSequence < std::make_index_sequence<sizeof...(Ts)>, ! TIsQPrivateSignal<std::decay_t<Ts>>::value... >;
            return invokeSlotImpl(std::forward<F>(func), object, std::forward_as_tuple(std::forward<Ts>(args)...), seq(), std::is_member_pointer<std::decay_t<F>>());
        }

        // Like QMetaObject::invokeMethod but the return value is accessed through a QFuture, and extra arguments can be provided.
        template <typename T, typename F, typename... Ts>
        auto invokeMethod(T *object, F &&func, Ts &&... args)
        {
            const auto invoker = [](auto &&... x) { return Private::invokeSlot(std::forward<decltype(x)>(x)...); };
            auto method = std::bind(invoker, std::forward<F>(func), object, std::forward<Ts>(args)...);
            CPromise<decltype(std::move(method)())> promise;
            QMetaObject::invokeMethod(object, [promise, method = std::move(method)]() mutable { promise.setResultFrom(std::move(method)); });
            return promise.future();
        }

    }
    //! \endcond
}

#endif
