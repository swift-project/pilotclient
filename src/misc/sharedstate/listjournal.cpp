// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "misc/sharedstate/listjournal.h"

#include "misc/sharedstate/datalink.h"

namespace swift::misc::shared_state
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
} // namespace swift::misc::shared_state
