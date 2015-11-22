/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "registermetadata.h"
#include "fs9_host.h"
#include "fs9_client.h"

namespace BlackSimPlugin
{
    namespace Fs9
    {
        void registerMetadata()
        {
            qRegisterMetaType<CFs9Host::HostStatus>();
            qRegisterMetaType<CFs9Client::ClientStatus>();
        }
    }
}
