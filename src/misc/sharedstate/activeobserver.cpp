// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "misc/sharedstate/activeobserver.h"

namespace swift::misc::shared_state
{
    CVariant CActiveObserver::request(const CVariant &param)
    {
        CPromise<CVariant> promise;
        emit requestPosted(param, promise);
        return promise.future().result();
    }

    void CActiveObserver::requestAsync(const CVariant &param, std::function<void(const CVariant &)> callback)
    {
        CPromise<CVariant> promise;
        emit requestPosted(param, promise);
        doAfter(promise.future(), this, [callback = std::move(callback), weakRef = weakRef()](const QFuture<CVariant> &reply) {
            const auto lock = weakRef.lock();
            if (lock) { callback(reply.result()); }
        });
    }
} // namespace swift::misc::shared_state
