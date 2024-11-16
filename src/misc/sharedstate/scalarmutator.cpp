// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "misc/sharedstate/scalarmutator.h"

#include "misc/sharedstate/datalink.h"

namespace swift::misc::shared_state
{
    void CGenericScalarMutator::initialize(IDataLink *dataLink) { dataLink->publish(m_mutator.data()); }

    void CGenericScalarMutator::setValue(const CVariant &value) { m_mutator->postEvent(value); }
} // namespace swift::misc::shared_state
