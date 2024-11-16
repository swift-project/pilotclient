// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AUDIO_SETTINGS_H
#define SWIFT_MISC_AUDIO_SETTINGS_H

#include <QMetaType>
#include <QString>

#include "misc/audio/notificationsounds.h"
#include "misc/metaclass.h"
#include "misc/settingscache.h"
#include "misc/statusmessage.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::audio, CSettings)

namespace swift::misc::audio
{
    //! Value object encapsulating information of audio related settings.
    class SWIFT_MISC_EXPORT CSettings : public CValueObject<CSettings>
    {
    public:
        //! @{
        //! Ranges for audio
        static constexpr int InMax = 100;
        static constexpr int InMin = 0;
        static constexpr int OutMax = 100;
        static constexpr int OutMin = 0;
        //! @}

        //! @{
        //! Make sure the volume is within the range
        static int fixOutVolume(int v);
        static int fixInVolume(int v);
        //! @}

        //! Default constructor.
        CSettings() = default;

        //! Notification flag (play notification?)
        bool isNotificationFlagSet(CNotificationSounds::NotificationFlag notification) const;

        //! Notification flag (play notification?)
        void setNotificationFlag(CNotificationSounds::NotificationFlag notification, bool value);

        //! Get notification
        CNotificationSounds::Notification getNotification() const { return static_cast<CNotificationSounds::Notification>(m_notification); }

        //! Set notification
        void setNotification(CNotificationSounds::Notification notification) { m_notification = static_cast<int>(notification); }

        //! @{
        //! Simplified functions
        bool textMessagePrivate() const { return this->isNotificationFlagSet(CNotificationSounds::NotificationTextMessagePrivate); }
        bool textMessageSupervisor() const { return this->isNotificationFlagSet(CNotificationSounds::NotificationTextMessageSupervisor); }
        bool textCallsignMentioned() const { return this->isNotificationFlagSet(CNotificationSounds::NotificationTextCallsignMentioned); }
        bool pttClickDown() const { return this->isNotificationFlagSet(CNotificationSounds::PTTClickKeyDown); }
        bool pttClickUp() const { return this->isNotificationFlagSet(CNotificationSounds::PTTClickKeyUp); }

        bool afvClicked() const { return this->isNotificationFlagSet(CNotificationSounds::AFVClicked); }
        bool afvBlocked() const { return this->isNotificationFlagSet(CNotificationSounds::AFVBlocked); }
        //! @}

        //! Directory
        void setNotificationSoundDirectory(const QString &dir);

        //! Notification directory
        const QString &getNotificationSoundDirectory() const { return m_notificationSoundDir; }

        //! get existing notifcation settings directory or default swift directory
        const QString &getNotificationSoundDirectoryOrDefault() const;

        //! Get existing file path for the given file path, either in the settings specific or default dir
        //! \remark pass file only, like "foo.wav"
        //! \return complete path like "/mydir/foo.wav" or empty if file not exists
        QString getNotificationFilePath(const QString &fileName) const;

        //! Set volume (notifications)
        void setNotificationVolume(int volume);

        //! Get volume (notifications)
        int getNotificationVolume() const { return m_notificationVolume; }

        //! Set volume (audio) 0..100
        void setOutVolume(int volume);

        //! Get volume (audio) 0..100
        int getOutVolume() const { return m_outVolume; }

        //! Set volume for com1 (audio) 0..100
        void setOutVolumeCom1(int volume);

        //! Get volume for com1 (audio) 0..100
        int getOutVolumeCom1() const { return m_outVolumeCom1; }

        //! Set volume for com2 (audio) 0..100
        void setOutVolumeCom2(int volume);

        //! Get volume for com2 (audio) 0..100
        int getOutVolumeCom2() const { return m_outVolumeCom2; }

        //! Set mic.volume 0..100
        void setInVolume(int volume);

        //! Get mic.volume (audio 0..100)
        int getInVolume() const { return m_inVolume; }

        //! Audio effects enabled?
        bool isAudioEffectsEnabled() const { return m_audioEffects; }

        //! Audio effects
        void setAudioEffectsEnabled(bool enabled) { m_audioEffects = enabled; }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        QString m_notificationSoundDir;
        int m_notification = static_cast<int>(CNotificationSounds::DefaultNotifications); //!< play notification for notification x, a little trick to use a string here (streamable, hashable, ..)
        int m_notificationVolume = 90; //!< 0-90
        int m_outVolume = 40; //!< 0-100, AFV
        int m_outVolumeCom1 = 100; //!< 0-100, AFV
        int m_outVolumeCom2 = 100; //!< 0-100, AFV
        int m_inVolume = 50; //!< AFV range
        bool m_audioEffects = true; //!< Audio effects en

        SWIFT_METACLASS(
            CSettings,
            SWIFT_METAMEMBER(notificationSoundDir),
            SWIFT_METAMEMBER(notification),
            SWIFT_METAMEMBER(notificationVolume),
            SWIFT_METAMEMBER(outVolume),
            SWIFT_METAMEMBER(outVolumeCom1),
            SWIFT_METAMEMBER(outVolumeCom2),
            SWIFT_METAMEMBER(inVolume),
            SWIFT_METAMEMBER(audioEffects));
    };

    //! Audio related settings
    struct TSettings : public TSettingTrait<CSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "audio/%Application%/setup"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Audio");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::isValid
        static bool isValid(const swift::misc::audio::CSettings &value, QString &)
        {
            Q_UNUSED(value)
            return true;
        }
    };
} // namespace swift::misc::audio

Q_DECLARE_METATYPE(swift::misc::audio::CSettings)

#endif // SWIFT_MISC_AUDIO_SETTINGS_H
