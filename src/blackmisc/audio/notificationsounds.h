/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NOTIFICATIONSOUNDS_H
#define BLACKMISC_NOTIFICATIONSOUNDS_H

#include "blackmisc/blackmiscexport.h"
#include <QMetaType>

namespace BlackMisc
{
    namespace Audio
    {
        /*!
         * Simplified enums to play sounds.
         * \remarks Currently located in project BlackMisc (i.e. outside project BlackSound)
         *          as this allows to trigger sounds without using Multimedia libraries.
         */
        struct BLACKMISC_EXPORT CNotificationSounds
        {
            //! How to play?
            enum PlayMode
            {
                Single,
                SingleWithAutomaticDeletion,
                EndlessLoop
            };

            //! Play notification
            enum NotificationFlag
            {
                NoNotifications                   = 0,
                NotificationError                 = 1 << 0,
                NotificationLogin                 = 1 << 1,
                NotificationLogoff                = 1 << 2,
                NotificationTextMessagePrivate    = 1 << 3,
                NotificationTextCallsignMentioned = 1 << 4,
                NotificationVoiceRoomJoined       = 1 << 5,
                NotificationVoiceRoomLeft         = 1 << 6,
                PTTClick                          = 1 << 7,
                LoadSounds                        = 1 << 8, //!< end marker and force loading of sounds, keep as last element
                AllTextNotifications              = NotificationTextMessagePrivate | NotificationTextCallsignMentioned,
                AllLoginNotifications             = NotificationLogin | NotificationLogoff,
                AllVoiceRoomNotifications         = NotificationVoiceRoomJoined | NotificationVoiceRoomLeft,
                AllNotifications                  = NotificationError | AllTextNotifications | AllLoginNotifications | AllVoiceRoomNotifications,
                DefaultNotifications              = NotificationError | NotificationTextMessagePrivate | AllLoginNotifications | AllVoiceRoomNotifications,
                All                               = AllNotifications  | PTTClick
            };
            Q_DECLARE_FLAGS(Notification, NotificationFlag)

            //! As string
            static const QString &flagToString(NotificationFlag notification);

            //! As string
            static const QString toString(Notification notification);
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Audio::CNotificationSounds::PlayMode)
Q_DECLARE_METATYPE(BlackMisc::Audio::CNotificationSounds::Notification)
Q_DECLARE_METATYPE(BlackMisc::Audio::CNotificationSounds::NotificationFlag)

#endif // guard
