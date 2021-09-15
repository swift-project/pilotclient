/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NOTIFICATIONSOUNDS_H
#define BLACKMISC_NOTIFICATIONSOUNDS_H

#include "blackmisc/blackmiscexport.h"
#include <QMetaType>

namespace BlackMisc::Audio
{
    /*!
     * Simplified enums to play sounds.
     * \remarks Currently located in project BlackMisc (i.e. outside project BlackSound)
     *          as this allows to trigger sounds without using Multimedia libraries.
     */
    struct BLACKMISC_EXPORT CNotificationSounds
    {
        //! Play notification
        enum NotificationFlag
        {
            NoNotifications                   = 0,
            NotificationError                 = 1 << 0,
            NotificationLogin                 = 1 << 1,
            NotificationLogoff                = 1 << 2,
            NotificationTextMessageFrequency  = 1 << 12,
            NotificationTextMessagePrivate    = 1 << 3,
            NotificationTextMessageSupervisor = 1 << 4,
            NotificationTextMessageUnicom     = 1 << 13,
            NotificationTextCallsignMentioned = 1 << 5,
            NotificationNoAudioTransmission   = 1 << 6,
            NotificationAtcTunedIn            = 1 << 10,
            NotificationAtcTunedOut           = 1 << 11,
            PTTClickKeyDown                   = 1 << 7,
            PTTClickKeyUp                     = 1 << 8,
            PTTBlocked                        = 1 << 9,
            AFVClicked                        = 1 << 14,
            AFVBlocked                        = 1 << 15
        };
        Q_DECLARE_FLAGS(Notification, NotificationFlag)

        //! All text notification flags
        constexpr static Notification AllTextNotifications = Notification(
            NotificationTextMessageFrequency | NotificationTextMessagePrivate | NotificationTextCallsignMentioned | NotificationTextMessageSupervisor | NotificationTextMessageUnicom);

        //! All login notification flags
        constexpr static Notification AllLoginNotifications = Notification(NotificationLogin | NotificationLogoff);

        //! All PTT notification flags
        constexpr static Notification AllPTTNotifications = Notification(PTTBlocked | PTTClickKeyUp | PTTClickKeyDown);

        //! All AFV flags
        constexpr static Notification AllAfv = Notification(AFVClicked | AFVBlocked);

        //! All notification flags
        constexpr static Notification AllNotifications = Notification(NotificationError | AllTextNotifications | AllLoginNotifications | AllPTTNotifications | AllAfv);

        //! Default notification flags
        constexpr static Notification DefaultNotifications = Notification(AllNotifications & ~(NotificationTextMessageFrequency | NotificationTextMessageUnicom));

        //! As string
        static const QString &flagToString(NotificationFlag notification);

        //! As string
        static QString toString(Notification notification);
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::Audio::CNotificationSounds::Notification)
Q_DECLARE_METATYPE(BlackMisc::Audio::CNotificationSounds::NotificationFlag)

#endif // guard
