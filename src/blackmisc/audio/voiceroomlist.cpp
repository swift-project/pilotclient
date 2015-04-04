/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "voiceroomlist.h"
#include "blackmisc/predicates.h"

namespace BlackMisc
{
    namespace Audio
    {

        CVoiceRoomList::CVoiceRoomList() { }

        CVoiceRoomList::CVoiceRoomList(const CSequence &other) :
            CSequence(other)
        { }

        void CVoiceRoomList::registerMetadata()
        {
            qRegisterMetaType<CVoiceRoomList>();
            qDBusRegisterMetaType<CVoiceRoomList>();
            registerMetaValueType<CVoiceRoomList>();
        }

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
