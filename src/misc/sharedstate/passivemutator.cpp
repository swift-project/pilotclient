// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "misc/sharedstate/passivemutator.h"

namespace swift::misc::shared_state
{
    void CPassiveMutator::postEvent(const CVariant &param)
    {
        emit eventPosted(param);
    }
} // namespace swift::misc::shared_state
