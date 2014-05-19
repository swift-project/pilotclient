#ifndef BLACKMISC_NOTIFICATIONSOUNDS_H
#define BLACKMISC_NOTIFICATIONSOUNDS_H

namespace BlackSound
{

    /*!
     * Simplified enums to play sound. Outside BlackSound as this allows
     * to trigger sounds without using Multimedia libraries.
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
            NotificationTextMessage,
        };
    };
}

#endif // guard
