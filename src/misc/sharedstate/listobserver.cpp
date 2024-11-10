// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "misc/sharedstate/listobserver.h"
#include "misc/sharedstate/datalink.h"
#include "misc/variantlist.h"

namespace swift::misc::shared_state
{
    void CGenericListObserver::initialize(IDataLink *dataLink)
    {
        dataLink->subscribe(m_observer.data());
        m_watcher = dataLink->watcher();
        connect(m_watcher, &CDataLinkConnectionWatcher::connected, this, &CGenericListObserver::reconstruct);
    }

    void CGenericListObserver::setFilter(const CVariant &filter)
    {
        m_observer->setEventSubscription(filter);
        if (m_watcher && m_watcher->isConnected()) { reconstruct(); }
    }

    void CGenericListObserver::reconstruct()
    {
        m_observer->requestAsync(m_observer->eventSubscription(), [this](const CVariant &list) {
            QMutexLocker lock(&m_listMutex);
            m_list = list.to<CVariantList>();
            lock.unlock();
            onGenericElementsReplaced(allValues());
        });
    }

    CVariantList CGenericListObserver::allValues() const
    {
        QMutexLocker lock(&m_listMutex);
        return m_list;
    }

    int CGenericListObserver::cleanValues()
    {
        QMutexLocker lock(&m_listMutex);
        return m_list.removeIf([filter = m_observer->eventSubscription()](const CVariant &value) {
            return !value.matches(filter);
        });
    }

    void CGenericListObserver::handleEvent(const CVariant &param)
    {
        QMutexLocker lock(&m_listMutex);
        m_list.push_back(param);
        lock.unlock();
        onGenericElementAdded(param);
    }
}
