/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/sharedstate/scalarobserver.h"
#include "blackmisc/sharedstate/datalink.h"

namespace BlackMisc::SharedState
{
    void CGenericScalarObserver::initialize(IDataLink *dataLink)
    {
        dataLink->subscribe(m_observer.data());
        m_observer->setEventSubscription(CVariant::from(CAnyMatch()));
        connect(dataLink->watcher(), &CDataLinkConnectionWatcher::connected, this, &CGenericScalarObserver::reconstruct);
        if (dataLink->watcher()->isConnected()) { reconstruct(); }
    }

    void CGenericScalarObserver::reconstruct()
    {
        m_observer->requestAsync({}, [this](const CVariant &value) { handleEvent(value); });
    }

    CVariant CGenericScalarObserver::value() const
    {
        QMutexLocker lock(&m_valueMutex);
        return m_value;
    }

    void CGenericScalarObserver::handleEvent(const CVariant &param)
    {
        QMutexLocker lock(&m_valueMutex);
        if (m_value == param) { return; }
        m_value = param;
        lock.unlock();
        onGenericValueChanged(param);
    }
}
