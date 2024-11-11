// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "misc/sharedstate/scalarjournal.h"
#include "misc/sharedstate/datalink.h"

namespace swift::misc::shared_state
{
    void CGenericScalarJournal::initialize(IDataLink *dataLink)
    {
        dataLink->publish(m_mutator.data());
        dataLink->subscribe(m_observer.data());
        m_observer->setEventSubscription(CVariant::from(CAnyMatch()));
        m_mutator->postEvent(m_value);
    }

    CVariant CGenericScalarJournal::handleRequest(const CVariant &param)
    {
        Q_UNUSED(param)
        return m_value;
    }

    void CGenericScalarJournal::handleEvent(const CVariant &param)
    {
        m_value = param;
    }
}
