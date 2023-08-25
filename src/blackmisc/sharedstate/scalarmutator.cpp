// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "blackmisc/sharedstate/scalarmutator.h"
#include "blackmisc/sharedstate/datalink.h"

namespace BlackMisc::SharedState
{
    void CGenericScalarMutator::initialize(IDataLink *dataLink)
    {
        dataLink->publish(m_mutator.data());
    }

    void CGenericScalarMutator::setValue(const CVariant &value)
    {
        m_mutator->postEvent(value);
    }
}
