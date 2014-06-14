/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "plugin.h"

namespace XBus
{

    CPlugin::CPlugin()
    {
    }

    void CPlugin::startServer(const QString &address)
    {
        Q_ASSERT(! m_server);
        m_server = new BlackCore::CDBusServer(address, this);
    }

}
