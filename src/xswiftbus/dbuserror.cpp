// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "dbuserror.h"

namespace XSwiftBus
{

    CDBusError::CDBusError(const DBusError *error)
        : m_name(error->name), m_message(error->message)
    {}

} // namespace XSwiftBus
