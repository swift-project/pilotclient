// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "registermetadata.h"

#include "fs9client.h"
#include "fs9host.h"

namespace swift::simplugin::fs9
{
    void registerMetadata()
    {
        qRegisterMetaType<CFs9Host::HostStatus>();
        qRegisterMetaType<CFs9Client::ClientStatus>();
    }
} // namespace swift::simplugin::fs9
