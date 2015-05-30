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

namespace BlackSound
{

    /*!
     * Simplified enums to play sounds.
     * \remarks Currently located in project BlackMisc (i.e. outside project BlackSound)
     *          as this allows to trigger sounds without using Multimedia libraries.
     */
    struct CNotificationSounds
    {
        //! How to play?
        enum PlayMode
        {
            Single,
            SingleWithAutomaticDeletion,
            EndlessLoop
        };

        //! Play notification
        enum Notification
        {
            NotificationError = 0,
            NotificationLogin,
            NotificationLogoff,
            NotificationTextMessagePrivate,
            NotificationVoiceRoomJoined,
            NotificationVoiceRoomLeft,
            NotificationsLoadSounds //!< end marker and force loading of sounds, keep as last element
        };
    };
} // ns

#endif // guard
