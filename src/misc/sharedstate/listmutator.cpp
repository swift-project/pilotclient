// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "misc/sharedstate/listmutator.h"

#include "misc/sharedstate/datalink.h"

namespace swift::misc::shared_state
{
    void CGenericListMutator::initialize(IDataLink *dataLink) { dataLink->publish(m_mutator.data()); }

    void CGenericListMutator::addElement(const CVariant &value) { m_mutator->postEvent(value); }
} // namespace swift::misc::shared_state
