/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nwclientlist.h"
#include "predicates.h"

namespace BlackMisc
{
    namespace Network
    {
        /*
         * Default constructor
         */
        CClientList::CClientList() { }

        /*
         * Construct from base class object
         */
        CClientList::CClientList(const CSequence &other) : CSequence<CClient>(other)
        { }

        /*
         * Register metadata
         */
        void CClientList::registerMetadata()
        {
            qRegisterMetaType<CClientList>();
            qDBusRegisterMetaType<CClientList>();
        }

    } // namespace
} // namespace
