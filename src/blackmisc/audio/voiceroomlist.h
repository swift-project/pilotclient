/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AUDIO_VOICEROOMLIST_H
#define BLACKMISC_AUDIO_VOICEROOMLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"
#include "voiceroom.h"

#include <QMetaType>

namespace BlackMisc
{
    namespace Audio
    {
        class CVoiceRoom;

        //! Value object encapsulating a list of voice rooms.
        class BLACKMISC_EXPORT CVoiceRoomList :
            public CSequence<CVoiceRoom>,
            public BlackMisc::Mixin::MetaType<CVoiceRoomList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CVoiceRoomList)
            using CSequence::CSequence;

            //! Default constructor.
            CVoiceRoomList();

            //! Construct from a base class object.
            CVoiceRoomList(const CSequence &other);

            //! Count how many rooms can be talked to
            int countCanTalkTo() const;

            //! Frequently needed for voice room resolutions
            static const CVoiceRoomList &twoEmptyRooms();
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Audio::CVoiceRoomList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Audio::CVoiceRoom>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Audio::CVoiceRoom>)

#endif //guard
