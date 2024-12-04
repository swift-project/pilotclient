// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_NOTIFICATIONPLAYER_H
#define SWIFT_SOUND_NOTIFICATIONPLAYER_H

#include <QHash>
#include <QObject>
#include <QPointer>
#include <QSoundEffect>

#include "misc/audio/notificationsounds.h"
#include "sound/swiftsoundexport.h"

namespace swift::sound
{
    //! Player for notification sounds
    class SWIFT_SOUND_EXPORT CNotificationPlayer : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CNotificationPlayer(QObject *parent = nullptr);

        //! Destructor
        virtual ~CNotificationPlayer() override {}

        //! Play notification sound
        void play(swift::misc::audio::CNotificationSounds::NotificationFlag notification, int volume = 100);

        //! Update the directory
        void updateDirectory(const QString &directory);

    private:
        QHash<swift::misc::audio::CNotificationSounds::NotificationFlag, QSoundEffect *> m_effects;
        QString m_directory;
        QPointer<QSoundEffect> m_playingEffect;
        int m_playingEffectCounter = 0;

        //! Playing of effect has been changed
        void onPlayingChanged();

        //! Update an effect
        void updateEffect(swift::misc::audio::CNotificationSounds::NotificationFlag f, const QString &directory,
                          const QString &name);
    };
} // namespace swift::sound

#endif // SWIFT_SOUND_NOTIFICATIONPLAYER_H
