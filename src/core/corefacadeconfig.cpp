// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/corefacadeconfig.h"

namespace swift::core
{
    bool CCoreFacadeConfig::requiresDBusSever() const { return m_contextMode == LocalInDBusServer; }

    bool CCoreFacadeConfig::requiresDBusConnection() const { return m_contextMode == Remote; }
} // namespace swift::core
