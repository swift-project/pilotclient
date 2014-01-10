/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_application.h"
#include "coreruntime.h"
#include "blackmisc/settingutilities.h"

using namespace BlackMisc::Settings;
using namespace BlackMisc::Network;
using namespace BlackMisc;

namespace BlackCore
{

    /*
     * Init this context
     */
    CContextApplication::CContextApplication(CCoreRuntime *parent) : IContextApplication(parent)
    {
        // void
    }

    /*
     * Ping, is DBus alive?
     */
    qint64 CContextApplication::ping(qint64 token) const
    {
        return token;
    }
} // namespace
