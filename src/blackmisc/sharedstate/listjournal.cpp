/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/sharedstate/listjournal.h"
#include "blackmisc/sharedstate/datalink.h"

namespace BlackMisc::SharedState
{
    void CGenericListJournal::initialize(IDataLink *dataLink)
    {
        dataLink->publish(m_mutator.data());
        dataLink->subscribe(m_observer.data());
        m_observer->setEventSubscription(CVariant::from(CAnyMatch()));
    }

    CVariant CGenericListJournal::handleRequest(const CVariant &filter)
    {
        CVariantList copy = m_value;
        if (filter.isValid())
        {
            copy.removeIf([&filter](const CVariant &v) { return !filter.matches(v); });
        }
        return CVariant::from(copy);
    }

    void CGenericListJournal::handleEvent(const CVariant &param)
    {
        m_value.push_back(param);
    }
}
