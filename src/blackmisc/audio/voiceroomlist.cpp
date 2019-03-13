/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/audio/voiceroom.h"
#include "blackmisc/audio/voiceroomlist.h"

#include <QString>
#include <tuple>

namespace BlackMisc
{
    namespace Audio
    {

        CVoiceRoomList::CVoiceRoomList() { }

        CVoiceRoomList::CVoiceRoomList(const CSequence &other) :
            CSequence(other)
        { }

        int CVoiceRoomList::countCanTalkTo() const
        {
            int c = 0;
            for (const CVoiceRoom &r : *this)
            {
                if (r.canTalkTo()) { c++; }
            }
            return c;
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
