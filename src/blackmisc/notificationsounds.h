#ifndef BLACKMISC_NOTIFICATIONSOUNDS_H
#define BLACKMISC_NOTIFICATIONSOUNDS_H

namespace BlackSound
{

    /*!
     * Simplified enums to play sounds.
     * \remarks Currently located in project BlackMisc (i.e. outside project BlackSound)
     * as this allows to trigger sounds without using Multimedia libraries.
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
}

#endif // guard
