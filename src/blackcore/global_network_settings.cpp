/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "global_network_settings.h"
#include "blackmisc/network/user.h"

using namespace BlackMisc::Network;

namespace BlackCore
{
    CGlobalNetworkSettings::CGlobalNetworkSettings() :
        m_fsdSwiftServer("swift", "swift Testserver", "vatsim-germany.org", 6809,
                         CUser("1234567", "swift Test User", "", "123456"), true)
    { }

    const CGlobalNetworkSettings &CGlobalNetworkSettings::instance()
    {
        static const CGlobalNetworkSettings rs;
        return rs;
    }
}
