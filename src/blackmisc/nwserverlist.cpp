/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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
