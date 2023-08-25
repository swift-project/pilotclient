// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "blackmisc/sharedstate/listmutator.h"
#include "blackmisc/sharedstate/datalink.h"

namespace BlackMisc::SharedState
{
    void CGenericListMutator::initialize(IDataLink *dataLink)
    {
        dataLink->publish(m_mutator.data());
    }

    void CGenericListMutator::addElement(const CVariant &value)
    {
        m_mutator->postEvent(value);
    }
}
