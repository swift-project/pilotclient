// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "notificationplayer.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/swiftdirectories.h"
#include "blacksound/sampleprovider/samples.h"
#include <QTimer>
#include <QSound>

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackSound::SampleProvider;

namespace BlackSound
{
    CNotificationPlayer::CNotificationPlayer(QObject *parent) : QObject(parent)
    {
        // lazy init at play
    }

    void CNotificationPlayer::play(CNotificationSounds::NotificationFlag notification, int volume)
    {
        QSoundEffect *effect = m_effects.value(notification, nullptr);
        if (effect)
        {
            if (!m_playingEffect && effect->isLoaded() && !effect->isPlaying())
            {
                const int currentEffect = ++m_playingEffectCounter;
                const qreal v = volume / 100.0f;
                m_playingEffect = effect;
                effect->setVolume(v); // 0..1
                effect->play();

                /** for testing
                QString fn = effect->source().toLocalFile();
                QSound s(fn);
                s.play();
                **/

                // used for too long or hanging sounds
                QPointer<CNotificationPlayer> myself(this);
                QTimer::singleShot(3000, effect, [=] {
                    if (!myself || !m_playingEffect) { return; }
                    if (currentEffect != m_playingEffectCounter) { return; }
                    m_playingEffect->stop();
                });
            }
        }
    }

    void CNotificationPlayer::updateDirectory(const QString &directory)
    {
        if (directory == m_directory && !m_effects.isEmpty()) { return; }
        m_directory = directory;

        const QStringList types = QSoundEffect::supportedMimeTypes();
        CLogMessage(this).info(u"Notification mime types: %1") << types.join(", ");

        this->updateEffect(CNotificationSounds::NotificationError, directory, "error.wav");
        this->updateEffect(CNotificationSounds::NotificationLogin, directory, "login.wav");
        this->updateEffect(CNotificationSounds::NotificationLogoff, directory, "logoff.wav");
        this->updateEffect(CNotificationSounds::NotificationTextMessageFrequency, directory, "frequencymessage.wav");
        this->updateEffect(CNotificationSounds::NotificationTextMessagePrivate, directory, "privatemessage.wav");
        this->updateEffect(CNotificationSounds::NotificationTextMessageSupervisor, directory, "supervisormessage.wav");
        this->updateEffect(CNotificationSounds::NotificationTextMessageUnicom, directory, "unicommessage.wav");
        this->updateEffect(CNotificationSounds::NotificationTextCallsignMentioned, directory, "callsignmentioned.wav");
        this->updateEffect(CNotificationSounds::NotificationNoAudioTransmission, directory, "noaudiotransmission.wav");
        this->updateEffect(CNotificationSounds::NotificationAtcTunedIn, directory, "atctunedin.wav");
        this->updateEffect(CNotificationSounds::NotificationAtcTunedOut, directory, "atctunedout.wav");
        this->updateEffect(CNotificationSounds::PTTBlocked, directory, "pttblocked.wav");
        this->updateEffect(CNotificationSounds::PTTClickKeyDown, directory, "pttclick.wav");
        this->updateEffect(CNotificationSounds::PTTClickKeyUp, directory, "pttclick.wav");

        // CNotificationSounds::AFVBlocked is generated
        this->updateEffect(CNotificationSounds::AFVClicked, directory, Samples::fnClick());
    }

    void CNotificationPlayer::onPlayingChanged()
    {
        if (!m_playingEffect) { return; }
        if (m_playingEffect->isPlaying()) { return; }
        m_playingEffect.clear();
    }

    void CNotificationPlayer::updateEffect(CNotificationSounds::NotificationFlag f, const QString &directory, const QString &name)
    {
        QSoundEffect *e = nullptr;
        if (m_effects.contains(f)) { e = m_effects[f]; }
        if (e) { e->deleteLater(); }

        // file if existing
        const QUrl url = QUrl::fromLocalFile(CSwiftDirectories::soundFilePathOrDefaultPath(directory, name));
        if (url.isEmpty() || !url.isLocalFile())
        {
            // remove notification as not existing
            m_effects.remove(f);
            return;
        }

        // new effect
        // QString fn = url.toLocalFile();
        QSoundEffect *effect = new QSoundEffect(this);
        effect->setSource(url);
        effect->setLoopCount(1);
        effect->setMuted(false);
        m_effects[f] = effect;
        connect(effect, &QSoundEffect::playingChanged, this, &CNotificationPlayer::onPlayingChanged, Qt::QueuedConnection);
    }
} // ns
