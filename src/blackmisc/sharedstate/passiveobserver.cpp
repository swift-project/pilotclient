/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/sharedstate/passiveobserver.h"

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::SharedState, CAnyMatch)

namespace BlackMisc::SharedState
{
    void CPassiveObserver::setEventSubscription(const CVariant &param)
    {
        QMutexLocker lock(&m_eventSubscriptionMutex);
        m_eventSubscription = param;
        lock.unlock();
        emit eventSubscriptionChanged(param);
    }

    CVariant CPassiveObserver::eventSubscription() const
    {
        QMutexLocker lock(&m_eventSubscriptionMutex);
        return m_eventSubscription;
    }

    void CPassiveObserver::handleEvent(const CVariant& param) const
    {
        m_eventHandler(param);
    }
}
