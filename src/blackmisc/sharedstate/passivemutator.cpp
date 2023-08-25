// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "blackmisc/sharedstate/passivemutator.h"

namespace BlackMisc::SharedState
{
    void CPassiveMutator::postEvent(const CVariant &param)
    {
        emit eventPosted(param);
    }
}
