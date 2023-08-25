// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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

    void CPassiveObserver::handleEvent(const CVariant &param) const
    {
        m_eventHandler(param);
    }
}
