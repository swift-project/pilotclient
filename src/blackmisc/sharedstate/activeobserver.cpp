/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/sharedstate/activeobserver.h"

namespace BlackMisc::SharedState
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
        doAfter(promise.future(), this, [callback = std::move(callback), weakRef = weakRef()](const CVariant &reply) {
            const auto lock = weakRef.lock();
            if (lock) { callback(reply); }
        });
    }
}
