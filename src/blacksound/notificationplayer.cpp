/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "notificationplayer.h"
#include "blackmisc/fileutils.h"

using namespace BlackMisc;
using namespace BlackMisc::Audio;

namespace BlackSound
{
    CNotificationPlayer::CNotificationPlayer(QObject *parent) :
        QObject(parent)
    {
        QSoundEffect *effect = new QSoundEffect(this);
        effect->setSource(CFileUtils::soundFileQUrl("error.wav"));
        m_effects[CNotificationSounds::NotificationError] = effect;

        effect = new QSoundEffect(this);
        effect->setSource(CFileUtils::soundFileQUrl("login.wav"));
        m_effects[CNotificationSounds::NotificationLogin] = effect;

        effect = new QSoundEffect(this);
        effect->setSource(CFileUtils::soundFileQUrl("logoff.wav"));
        m_effects[CNotificationSounds::NotificationLogoff] = effect;

        effect = new QSoundEffect(this);
        effect->setSource(CFileUtils::soundFileQUrl("privatemessage.wav"));
        m_effects[CNotificationSounds::NotificationTextMessagePrivate] = effect;

        effect = new QSoundEffect(this);
        effect->setSource(CFileUtils::soundFileQUrl("supervisormessage.wav"));
        m_effects[CNotificationSounds::NotificationTextMessageSupervisor] = effect;

        effect = new QSoundEffect(this);
        effect->setSource(CFileUtils::soundFileQUrl("callsignmentioned.wav"));
        m_effects[CNotificationSounds::NotificationTextCallsignMentioned] = effect;

        effect = new QSoundEffect(this);
        effect->setSource(CFileUtils::soundFileQUrl("voiceroomjoined.wav"));
        m_effects[CNotificationSounds::NotificationVoiceRoomJoined] = effect;

        effect = new QSoundEffect(this);
        effect->setSource(CFileUtils::soundFileQUrl("voiceroomleft.wav"));
        m_effects[CNotificationSounds::NotificationVoiceRoomLeft] = effect;

        effect = new QSoundEffect(this);
        effect->setSource(CFileUtils::soundFileQUrl("noaudiotransmission.wav"));
        m_effects[CNotificationSounds::NotificationNoAudioTransmission] = effect;

        effect = new QSoundEffect(this);
        effect->setSource(CFileUtils::soundFileQUrl("pttclick.wav"));
        m_effects[CNotificationSounds::PTTClickKeyDown] = effect;

        effect = new QSoundEffect(this);
        effect->setSource(CFileUtils::soundFileQUrl("pttclick.wav"));
        m_effects[CNotificationSounds::PTTClickKeyUp] = effect;
    }

    void CNotificationPlayer::play(Audio::CNotificationSounds::NotificationFlag notification, int volume) const
    {
        QSoundEffect *effect = m_effects.value(notification, nullptr);
        if (effect)
        {
            effect->setVolume(volume / 100.0);
            effect->play();
        }
    }
} // ns
