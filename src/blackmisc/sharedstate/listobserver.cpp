/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/sharedstate/listobserver.h"
#include "blackmisc/sharedstate/datalink.h"
#include "blackmisc/variantlist.h"

namespace BlackMisc
{
    namespace SharedState
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
            m_observer->requestAsync(m_observer->eventSubscription(), [this](const CVariant &list)
            {
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
            return m_list.removeIf([filter = m_observer->eventSubscription()](const CVariant &value)
            {
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
}
