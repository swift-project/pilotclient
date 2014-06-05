/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "voiceroomlist.h"
#include "predicates.h"

namespace BlackMisc
{
    namespace Audio
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

        /*
         * Two empty rooms
         */
        const CVoiceRoomList &CVoiceRoomList::twoEmptyRooms()
        {
            static CVoiceRoomList emptyRooms;
            if (emptyRooms.isEmpty())
            {
                emptyRooms.push_back(CVoiceRoom());
                emptyRooms.push_back(CVoiceRoom());
            }
            return emptyRooms;
        }

    } // namespace
} // namespace
