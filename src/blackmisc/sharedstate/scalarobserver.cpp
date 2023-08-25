// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
