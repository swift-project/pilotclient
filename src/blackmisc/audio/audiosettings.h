/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
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
#include "blackmisc/variant.h"

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

            //! Simplified functions @{
            bool textMessagePrivate() const { return this->isNotificationFlagSet(CNotificationSounds::NotificationTextMessagePrivate); }
            bool textCallsignMentioned() const { return this->isNotificationFlagSet(CNotificationSounds::NotificationTextCallsignMentioned); }
            bool pttClick() const { return this->isNotificationFlagSet(CNotificationSounds::PTTClick); }
            //! @}

            //! Settings value
            CStatusMessage value(const QString &path, const QString &command, const CVariant &value, bool &changedFlag);

            //! Init with meaningful default values
            void initDefaultValues();

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            int m_notification = static_cast<int>(CNotificationSounds::DefaultNotifications); //!< play notification for notification x, a little trick to use a string here (streamable, hashable, ..)
            void initNotificationFlags(); //!< init flags

            BLACK_METACLASS(
                CSettings,
                BLACK_METAMEMBER(notification)
            );
        };

        //! Audio related settings
        struct TSettings : public TSettingTrait<CSettings>
        {
            //! \copydoc BlackMisc::TSettingTrait::key
            static const char *key() { return "audio/setup"; }

            //! \copydoc BlackCore::TSettingTrait::humanReadable
            static const QString &humanReadable() { static const QString name("Audio"); return name; }

            //! \copydoc BlackMisc::TSettingTrait::isValid
            static bool isValid(const BlackMisc::Audio::CSettings &value) { Q_UNUSED(value); return true; }
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Audio::CSettings)

#endif // guard
