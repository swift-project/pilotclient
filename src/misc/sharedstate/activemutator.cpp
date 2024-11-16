// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "misc/sharedstate/activemutator.h"

namespace swift::misc::shared_state
{
    QFuture<CVariant> CActiveMutator::handleRequest(const CVariant &param) const
    {
        return m_requestHandler(param);
    }
} // namespace swift::misc::shared_state
