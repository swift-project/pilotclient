/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vvoiceroomlist.h"
#include "predicates.h"

namespace BlackMisc
{
    namespace Voice
    {
        /*
         * Default constructor
         */
        CVoiceRoomList::CVoiceRoomList() { }

        /*
         * Construct from base class object
         */
        CVoiceRoomList::CVoiceRoomList(const CSequence &other) :
            CSequence(other)
        { }

        /*
         * Register metadata
         */
        void CVoiceRoomList::registerMetadata()
        {
            qRegisterMetaType<CVoiceRoomList>();
            qDBusRegisterMetaType<CVoiceRoomList>();
        }

    } // namespace
} // namespace
