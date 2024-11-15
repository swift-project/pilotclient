// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NOTIFICATIONSOUNDS_H
#define SWIFT_MISC_NOTIFICATIONSOUNDS_H

#include "misc/swiftmiscexport.h"
#include <QMetaType>

namespace swift::misc::audio
{
    /*!
     * Simplified enums to play sounds.
     * \remarks Currently located in project swift::misc (i.e. outside project swift::sound)
     *          as this allows to trigger sounds without using Multimedia libraries.
     */
    struct SWIFT_MISC_EXPORT CNotificationSounds
    {
        //! Play notification
        enum NotificationFlag
        {
            NoNotifications = 0,
            NotificationError = 1 << 0,
            NotificationLogin = 1 << 1,
            NotificationLogoff = 1 << 2,
            NotificationTextMessageFrequency = 1 << 12,
            NotificationTextMessagePrivate = 1 << 3,
            NotificationTextMessageSupervisor = 1 << 4,
            NotificationTextMessageUnicom = 1 << 13,
            NotificationTextCallsignMentioned = 1 << 5,
            // 1 << 6 previously used for NoAudioTransmission
            // 1 << 10 previously used for NotificationAtcTunedIn
            // 1 << 11 previously used for NotificationAtcTunedOut
            PTTClickKeyDown = 1 << 7,
            PTTClickKeyUp = 1 << 8,
            // 1 << 9 previously used for PTTBlocked

            // AFVClicked and AFVBlocked are not played back from the notification player itself (except for preview playback).
            // Instead, they are provided from swift::sound::sample_provider::Samples.
            // They are included here to allow to disable playing them.
            AFVClicked = 1 << 14,
            AFVBlocked = 1 << 15
        };
        Q_DECLARE_FLAGS(Notification, NotificationFlag)

        //! All text notification flags
        constexpr static Notification AllTextNotifications = Notification(
            NotificationTextMessageFrequency | NotificationTextMessagePrivate | NotificationTextCallsignMentioned | NotificationTextMessageSupervisor | NotificationTextMessageUnicom);

        //! All login notification flags
        constexpr static Notification AllLoginNotifications = Notification(NotificationLogin | NotificationLogoff);

        //! All PTT notification flags
        constexpr static Notification AllPTTNotifications = Notification(PTTClickKeyUp | PTTClickKeyDown);

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

Q_DECLARE_METATYPE(swift::misc::audio::CNotificationSounds::Notification)
Q_DECLARE_METATYPE(swift::misc::audio::CNotificationSounds::NotificationFlag)

#endif // SWIFT_MISC_NOTIFICATIONSOUNDS_H
