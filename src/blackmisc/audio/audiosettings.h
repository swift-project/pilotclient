/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AUDIO_SETTINGS_H
#define BLACKMISC_AUDIO_SETTINGS_H

#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/valueobject.h"

#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Audio
    {
        //! Value object encapsulating information of audio related settings.
        class BLACKMISC_EXPORT CSettings : public CValueObject<CSettings>
        {
        public:
            //! Ranges for audio
            //! @{
            static constexpr int InMax  =  100;
            static constexpr int InMin  =  0;
            static constexpr int OutMax =  100;
            static constexpr int OutMin =  0;
            //! @}

            //! Make sure the volume is within the range
            //! @{
            static int fixOutVolume(int v);
            static int fixInVolume(int v);
            //! @}

            //! Default constructor.
            CSettings();

            //! Notification flag (play notification?)
            bool isNotificationFlagSet(CNotificationSounds::NotificationFlag notification) const;

            //! Notification flag (play notification?)
            void setNotificationFlag(CNotificationSounds::NotificationFlag notification, bool value);

            //! Get notification
            CNotificationSounds::Notification getNotification() const { return static_cast<CNotificationSounds::Notification>(m_notification); }

            //! Set notification
            void setNotification(CNotificationSounds::Notification notification) { m_notification = static_cast<int>(notification); }

            //! Simplified functions
            //! @{
            bool textMessagePrivate()    const { return this->isNotificationFlagSet(CNotificationSounds::NotificationTextMessagePrivate); }
            bool textMessageSupervisor() const { return this->isNotificationFlagSet(CNotificationSounds::NotificationTextMessageSupervisor); }
            bool textCallsignMentioned() const { return this->isNotificationFlagSet(CNotificationSounds::NotificationTextCallsignMentioned); }
            bool noAudioTransmission()   const { return this->isNotificationFlagSet(CNotificationSounds::NotificationNoAudioTransmission); }
            bool pttClickDown()          const { return this->isNotificationFlagSet(CNotificationSounds::PTTClickKeyDown); }
            bool pttClickUp()            const { return this->isNotificationFlagSet(CNotificationSounds::PTTClickKeyUp); }
            bool pttBlocked()            const { return this->isNotificationFlagSet(CNotificationSounds::PTTBlocked); }

            bool afvClicked()            const { return this->isNotificationFlagSet(CNotificationSounds::AFVClicked); }
            bool afvBlocked()            const { return this->isNotificationFlagSet(CNotificationSounds::AFVBlocked); }
            //! @}

            //! Settings value
            CStatusMessage value(const QString &path, const QString &command, const CVariant &value, bool &changedFlag);

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

            //! Init with meaningful default values
            void initDefaultValues();

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            QString m_notificationSoundDir;
            int m_notification = static_cast<int>(CNotificationSounds::DefaultNotifications); //!< play notification for notification x, a little trick to use a string here (streamable, hashable, ..)
            int m_notificationVolume = 90;   //!< 0-90
            int m_outVolumeCom1      = 50;   //!< 0-100, AFV
            int m_outVolumeCom2      = 50;   //!< 0-100, AFV
            int m_inVolume           = 50;   //!< AFV range
            bool m_audioEffects      = true; //!< Audio effects en
            void initNotificationFlags();    //!< init flags

            BLACK_METACLASS(
                CSettings,
                BLACK_METAMEMBER(notificationSoundDir),
                BLACK_METAMEMBER(notification),
                BLACK_METAMEMBER(notificationVolume),
                BLACK_METAMEMBER(outVolumeCom1),
                BLACK_METAMEMBER(outVolumeCom2),
                BLACK_METAMEMBER(inVolume),
                BLACK_METAMEMBER(audioEffects)
            );
        };

        //! Audio related settings
        struct TSettings : public TSettingTrait<CSettings>
        {
            //! \copydoc BlackMisc::TSettingTrait::key
            static const char *key() { return "audio/%Application%/setup"; }

            //! \copydoc BlackMisc::TSettingTrait::humanReadable
            static const QString &humanReadable() { static const QString name("Audio"); return name; }

            //! \copydoc BlackMisc::TSettingTrait::isValid
            static bool isValid(const BlackMisc::Audio::CSettings &value, QString &)
            {
                Q_UNUSED(value)
                return true;
            }
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Audio::CSettings)

#endif // guard
