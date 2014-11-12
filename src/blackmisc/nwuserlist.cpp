/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nwuserlist.h"
#include "predicates.h"

namespace BlackMisc
{
    namespace Network
    {
        /*
         * Default constructor
         */
        CUserList::CUserList() { }

        /*
         * Construct from base class object
         */
        CUserList::CUserList(const CSequence &other) : CSequence<CUser>(other)
        { }

        /*
         * Register metadata
         */
        void CUserList::registerMetadata()
        {
            qRegisterMetaType<CUserList>();
            qDBusRegisterMetaType<CUserList>();
            registerMetaValueType<CUserList>();
        }

    } // namespace
} // namespace
