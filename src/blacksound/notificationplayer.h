/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSOUND_NOTIFICATIONPLAYER_H
#define BLACKSOUND_NOTIFICATIONPLAYER_H

#include "blacksound/blacksoundexport.h"
#include "blackmisc/audio/notificationsounds.h"

#include <QObject>
#include <QHash>
#include <QSoundEffect>
#include <QPointer>

namespace BlackSound
{
    //! Player for notification sounds
    class BLACKSOUND_EXPORT CNotificationPlayer : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CNotificationPlayer(QObject *parent = nullptr);

        //! Destructor
        virtual ~CNotificationPlayer() override {}

        //! Play notification sound
        void play(BlackMisc::Audio::CNotificationSounds::NotificationFlag notification, int volume = 100);

        //! Update the directory
        void updateDirectory(const QString &directory);

    private:
        QHash<BlackMisc::Audio::CNotificationSounds::NotificationFlag, QSoundEffect *> m_effects;
        QString m_directory;
        QPointer<QSoundEffect> m_playingEffect;
        int m_playingEffectCounter = 0;

        //! Playing of effect has been changed
        void onPlayingChanged();

        //! Update an effect
        void updateEffect(BlackMisc::Audio::CNotificationSounds::NotificationFlag f, const QString &directory, const QString &name);
    };
} // ns

#endif // guard
