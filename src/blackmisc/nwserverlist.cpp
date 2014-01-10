/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nwserverlist.h"

namespace BlackMisc
{
    namespace Network
    {
        /*
         * Empty constructor
         */
        CServerList::CServerList() { }

        /*
         * Construct from base class object
         */
        CServerList::CServerList(const CSequence<CServer> &other) :
            CSequence<CServer>(other)
        { }

        /*
         * Register metadata
         */
        void CServerList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CServer>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CServer>>();
            qRegisterMetaType<BlackMisc::CCollection<CServer>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CServer>>();
            qRegisterMetaType<CServerList>();
            qDBusRegisterMetaType<CServerList>();
        }

    } // namespace
} // namespace
